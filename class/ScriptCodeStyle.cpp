
#include "scriptcommon.h"
#include "EMicroCodeType.h"
#include "ScriptExecBlock.h"
#include "ScriptRunState.h"
#include "ScriptClassMgr.h"

#include "ScriptCodeStyle.h"
#include "ScriptGlobalVarMgr.h"

namespace zlscript
{
#define BLOCK_SCRIPTVAR_RELEASE(pVar) {\
			if (pVar)\
			{\
				pBlock->ReleaseVar((CBaseVar*)pVar); \
				pVar = nullptr; \
			}\
	}\

#define BLOCK_SCRIPTVAR_COPY_VAR(pVar1,pVar2) {\
		if (pVar2)\
		{\
			if (pVar1 && pVar1->GetType() == pVar2->GetType())\
			{\
				pVar1->Set(pVar2);\
			}\
			else \
			{\
				BLOCK_SCRIPTVAR_RELEASE(pVar1);\
				pVar1 = pBlock->NewVar(pVar2->GetType());\
				pVar1->Set(pVar2);\
			}\
		}\
	}

	bool CBaseExeCode::MakeParamInfo(CScriptExecBlock* pBlock, CExeParamInfo& info)
	{
		switch (info.nType)
		{
		case E_VAR_SCOPE_CONST:
		{
			info.isNeedRelease = false;
			info.pVar = pBlock->GetConstVar(info.dwPos);
		}
		break;
		case E_VAR_SCOPE_GLOBAL:
		{
			const CBaseVar* pGVar = CScriptGlobalVarMgr::GetInstance()->Get(info.dwPos);
			info.isNeedRelease = true;
			CBaseVar* pTemp = nullptr;
			BLOCK_SCRIPTVAR_COPY_VAR(pTemp, pGVar);
			info.pVar = pTemp;
			CScriptGlobalVarMgr::GetInstance()->Revert(info.dwPos);
		}
		break;
		case E_VAR_SCOPE_LOACL:
		{
			info.isNeedRelease = false;
			info.pVar = pBlock->GetLoaclVar(info.dwPos);
		}
		break;
		case E_VAR_SCOPE_REGISTER_STACK:
		{
			info.isNeedRelease = true;
			STACK_POP(pBlock->registerStack, info.pVar);
		}
		break;
		case E_VAR_SCOPE_REGISTER:
		{
			info.isNeedRelease = false;
			info.pVar = pBlock->fixedRegister[info.dwPos];
		}
		break;
		default:
			return false;
		}
		return true;
	}

	void CBaseExeCode::ReleaseParamInfo(CScriptExecBlock* pBlock, CExeParamInfo& info)
	{
		if (info.isNeedRelease)
		{
			pBlock->ReleaseVar((CBaseVar*)info.pVar);
			info.pVar = nullptr;
		}

	}
	int CUnaryOperExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		MakeParamInfo(pBlock, param);
		if (param.pVar)
		{
			if (resultVarType == -1)
			{
				resultVarType = param.pVar->GetType();
			}
			if (registerIndex < R_SIZE)
			{
				CBaseVar* pVar = pBlock->fixedRegister[registerIndex];
				if (pVar == nullptr ||
					pVar->GetType() != resultVarType)
				{
					pBlock->ReleaseVar(pVar);
					pBlock->fixedRegister[registerIndex] = pBlock->NewVar(resultVarType);
				}
				if (oper(param.pVar, pBlock->fixedRegister[registerIndex]) == false)
				{
					Clear(pBlock);
					return CScriptExecBlock::ERESULT_ERROR;
				}
			}
			else
			{
				CBaseVar* result = pBlock->NewVar(resultVarType);
				if (oper(param.pVar, result) == false)
				{
					Clear(pBlock);
					return CScriptExecBlock::ERESULT_ERROR;
				}
				STACK_PUSH_MOVE(pBlock->registerStack, result);
			}
		}
		else
		{
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		Clear(pBlock);
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CUnaryOperExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "UNARY_OPER\tregister %d\tparam(%d,%d)", registerIndex, param.nType, param.dwPos);
		return strbuff;
	}

	void CUnaryOperExeCode::Clear(CScriptExecBlock* pBlock)
	{
		ReleaseParamInfo(pBlock,param);
	}

	int CUnaryOperGroupExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		MakeParamInfo(pBlock, param);
		if (param.pVar)
		{
			if (registerIndex < R_SIZE)
			{
				auto it = operGroup->find(param.pVar->GetType());
				if (it != operGroup->end())
				{
					UnaryOperFun& fun = it->second.fun;
					CBaseVar* pVar = pBlock->fixedRegister[registerIndex];
					if (pVar == nullptr ||
						pVar->GetType() != it->second.resultVarType)
					{
						pBlock->ReleaseVar(pVar);
						pBlock->fixedRegister[registerIndex] = pBlock->NewVar(it->second.resultVarType);
					}
					if (fun(pVar, pBlock->fixedRegister[registerIndex]) == false)
					{
						Clear(pBlock);
						return CScriptExecBlock::ERESULT_ERROR;
					}
				}
				else
				{
					Clear(pBlock);
					return CScriptExecBlock::ERESULT_ERROR;
				}
			}
			else
			{
				auto it = operGroup->find(param.pVar->GetType());
				if (it != operGroup->end())
				{
					CBaseVar* result = pBlock->NewVar(it->second.resultVarType);
					UnaryOperFun& fun = it->second.fun;
					if (fun(param.pVar, result) == false)
					{
						Clear(pBlock);
						return CScriptExecBlock::ERESULT_ERROR;
					}

					STACK_PUSH_MOVE(pBlock->registerStack,result);
				}
				else
				{
					Clear(pBlock);
					return CScriptExecBlock::ERESULT_ERROR;
				}
			}

		}
		else
		{
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		Clear(pBlock);
		return CScriptExecBlock::ERESULT_CONTINUE;
	}

	std::string CUnaryOperGroupExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "UNARY_OPER_GROUP\tregister %d\tparam(%d,%d)", registerIndex, param.nType, param.dwPos);
		return strbuff;
	}

	void CUnaryOperGroupExeCode::Clear(CScriptExecBlock* pBlock)
	{
		ReleaseParamInfo(pBlock,param);
	}

	int CBinaryOperExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		MakeParamInfo(pBlock, rightParam);
		MakeParamInfo(pBlock, leftParam);
		
		if (resultVarType == -1)
		{
			resultVarType = leftParam.pVar->GetType();
		}
		if (registerIndex < R_SIZE)
		{
			CBaseVar* pVar = pBlock->fixedRegister[registerIndex];
			if (pVar == nullptr ||
				pVar->GetType() != resultVarType)
			{
				pBlock->ReleaseVar(pVar);
				pBlock->fixedRegister[registerIndex] = pBlock->NewVar(resultVarType);

			}
			if (oper(leftParam.pVar, rightParam.pVar, pBlock->fixedRegister[registerIndex]) == false)
			{
				Clear(pBlock);
				return CScriptExecBlock::ERESULT_ERROR;
			}
		}
		else
		{
			CBaseVar* result = nullptr;
			if (leftParam.pVar)
			{
				result = pBlock->NewVar(resultVarType);
			}
			if (oper(leftParam.pVar, rightParam.pVar, result) == false)
			{
				Clear(pBlock);
				return CScriptExecBlock::ERESULT_ERROR;
			}
			STACK_PUSH_MOVE(pBlock->registerStack, result);
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		Clear(pBlock);
		return CScriptExecBlock::ERESULT_CONTINUE;

	}

	std::string CBinaryOperExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "BINARY_OPER\t register %d\tleftParam(%d,%d)\trightParam(%d,%d)\t",
			registerIndex, leftParam.nType, leftParam.dwPos, rightParam.nType, rightParam.dwPos);
		return strbuff;
	}

	void CBinaryOperExeCode::Clear(CScriptExecBlock* pBlock)
	{
		ReleaseParamInfo(pBlock,leftParam);
		ReleaseParamInfo(pBlock,rightParam);
	}


	int CBinaryOperGroupExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		MakeParamInfo(pBlock, rightParam);
		MakeParamInfo(pBlock, leftParam);
		if (leftParam.pVar == nullptr || rightParam.pVar == nullptr)
		{
			Clear(pBlock);
			return CScriptExecBlock::ERESULT_ERROR;
		}
		union {
			struct
			{
				int type1;
				int type2;
			};
			__int64 index;
		} trans;
		trans.type1 = leftParam.pVar->GetType();
		trans.type2 = rightParam.pVar->GetType();

		auto it = operGroup->find(trans.index);
		if (it != operGroup->end())
		{
			BinaryOperFun& fun = it->second.fun;
			if (registerIndex < R_SIZE)
			{
				CBaseVar* pVar = pBlock->fixedRegister[registerIndex];
				if (pVar == nullptr ||
					pVar->GetType() != it->second.resultVarType)
				{
					pBlock->ReleaseVar(pVar);
					pBlock->fixedRegister[registerIndex] = pBlock->NewVar(it->second.resultVarType);
				}
				if (fun(leftParam.pVar, rightParam.pVar, pBlock->fixedRegister[registerIndex]) == false)
				{
					Clear(pBlock);
					return CScriptExecBlock::ERESULT_ERROR;
				}
			}
			else
			{
				CBaseVar* result = nullptr;
				if (leftParam.pVar)
				{
					result = pBlock->NewVar(it->second.resultVarType);
				}
				if (fun(leftParam.pVar, rightParam.pVar, result) == false)
				{
					Clear(pBlock);
					return CScriptExecBlock::ERESULT_ERROR;
				}
				STACK_PUSH_MOVE(pBlock->registerStack, result);
			}
		}
		else
		{
			Clear(pBlock);
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		Clear(pBlock);
		return CScriptExecBlock::ERESULT_CONTINUE;
	}

	std::string CBinaryOperGroupExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "BINARY_OPER_GROUP\t register %d\tleftParam(%d,%d)\trightParam(%d,%d)\t",
			registerIndex, leftParam.nType, leftParam.dwPos, rightParam.nType, rightParam.dwPos);
		return strbuff;
	}

	void CBinaryOperGroupExeCode::Clear(CScriptExecBlock* pBlock)
	{
		ReleaseParamInfo(pBlock,leftParam);
		ReleaseParamInfo(pBlock,rightParam);
	}
	int CVoluationExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		MakeParamInfo(pBlock, param);

		if (cType == E_VAR_SCOPE_GLOBAL)
		{
			CScriptGlobalVarMgr::GetInstance()->Set(dwPos, param.pVar);
		}
		else if (cType == E_VAR_SCOPE_LOACL)
		{
			pBlock->SetLoaclVar(dwPos, param.pVar);
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		ReleaseParamInfo(pBlock,param);
		return CScriptExecBlock::ERESULT_CONTINUE;
	}

	std::string CVoluationExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "VOLUATION tregister %d\tparam(%d,%d)",registerIndex, param.nType, param.dwPos);
		return strbuff;
	}

	int CGetClassParamExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		int nResult = CScriptExecBlock::ERESULT_CONTINUE;

		MakeParamInfo(pBlock, param);
		CPointVar* pVar = dynamic_cast<CPointVar*>((CBaseVar*)param.pVar);
		if (pVar == nullptr)
		{
			ReleaseParamInfo(pBlock,param);
			return CScriptExecBlock::ERESULT_ERROR;
		}


		//PointVarInfo pointVal = GetPointIndex_StackVar(&m_register[code.cSign]);
		CScriptPointInterface* pPoint = pVar->ToPoint();
		if (pPoint)
		{
			pPoint->Lock();
			auto pAttribute = pPoint->GetAttribute(dwPos);
			if (pAttribute)
			{
				CBaseVar* pAttrVar = pAttribute->ToScriptVal();
				if (pAttrVar)
				{
					if (registerIndex < R_SIZE)
					{
						//CBaseVar* pResult = pBlock->fixedRegister[registerIndex];
						BLOCK_SCRIPTVAR_COPY_VAR(pBlock->fixedRegister[registerIndex], pAttrVar);
					}
					else
					{
						CBaseVar* pResult = pBlock->NewVar(pAttrVar->GetType());
						pResult->Set(pAttrVar);
						STACK_PUSH_MOVE(pBlock->registerStack, pResult);
					}

				}
			}
			else
			{
				//TODO 报错
				nResult = CScriptExecBlock::ERESULT_ERROR;
			}
			pPoint->UnLock();
		}

		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		ReleaseParamInfo(pBlock,param);
		return nResult;
	}
	std::string CGetClassParamExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "GetClassParam\tregister %d\tParam:(%d,%d)\tpos:%d",registerIndex, (int)param.nType, (int)param.dwPos, (int)dwPos);
		return strbuff;
	}
	int CSetClassParamExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		int nResult = CScriptExecBlock::ERESULT_CONTINUE;
		MakeParamInfo(pBlock, valInfo);
		MakeParamInfo(pBlock, param);
		CPointVar* pVar = dynamic_cast<CPointVar*>((CBaseVar*)param.pVar);
		if (pVar == nullptr)
		{
			Clear(pBlock);
			return CScriptExecBlock::ERESULT_ERROR;
		}
		CScriptPointInterface* pPoint = pVar->ToPoint();
		if (pPoint)
		{
			pPoint->Lock();
			auto pAttribute = pPoint->GetAttribute(dwPos);
			if (pAttribute)
			{
				//CBaseVar* pAttrVar = nullptr;
				//STACK_POP(pBlock->registerStack, pAttrVar);
				pAttribute->SetVal(valInfo.pVar);
			}
			else
			{
				//TODO 报错
				nResult = CScriptExecBlock::ERESULT_ERROR;
			}
			pPoint->UnLock();
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		Clear(pBlock);
		return nResult;
	}
	std::string CSetClassParamExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "SetClassParam\tregister\tParam:(%d,%d)\tpos:%d", registerIndex,(int)param.nType, (int)param.dwPos, (int)dwPos);
		return strbuff;
	}
	void CSetClassParamExeCode::Clear(CScriptExecBlock* pBlock)
	{
		ReleaseParamInfo(pBlock,param);
		ReleaseParamInfo(pBlock,valInfo);
	}
	int CCallBackExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		int nResult = CScriptExecBlock::ERESULT_CONTINUE;

		CACHE_NEW(CScriptCallState, pCallState, pBlock->m_pMaster);
		if (pCallState)
		{
			//设置参数,倒序，方便从堆栈中取值
			for (int i = (int)params.size() - 1; i >= 0; i--)
			{
				MakeParamInfo(pBlock, params[i]);
			}
			for (unsigned int i = 0; i < params.size(); i++)
			{
				STACK_PUSH_COPY(pCallState->m_stackRegister, params[i].pVar);
			}

			//运行回调函数
			switch (pBlock->m_pMaster->CallFun_CallBack(pBlock->m_pMaster->m_pMachine, unFunIndex, pCallState))
			{
			case ECALLBACK_ERROR:
				nResult = CScriptExecBlock::ERESULT_ERROR;
				break;
			case ECALLBACK_WAITING:
				nResult = CScriptExecBlock::ERESULT_WAITING;
				break;
				//case ECALLBACK_CALLSCRIPTFUN:

				//	nResult = CScriptExecBlock::ERESULT_CALLSCRIPTFUN;
				//	break;
			case ECALLBACK_NEXTCONTINUE:
				nResult = CScriptExecBlock::ERESULT_NEXTCONTINUE;
				break;
			case ECALLBACK_FINISH:
				if (pCallState->GetResult())
				{
					if (registerIndex < R_SIZE)
					{
						BLOCK_SCRIPTVAR_COPY_VAR(pBlock->fixedRegister[registerIndex], pCallState->GetResult());
					}
					else
					{
						CBaseVar* pResult = pBlock->NewVar(pCallState->GetResult()->GetType());
						pResult->Set(pCallState->GetResult());
						STACK_PUSH_MOVE(pBlock->registerStack, pResult);
					}
				}
				break;
			}
			//执行完将结果放入寄存器
			//pBlock->m_register[cResultRegister] = pCallState->GetResult();
		}
		CACHE_DELETE(pCallState);

		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		Clear(pBlock);
		return nResult;
	}
	std::string CCallBackExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "CallBack\tregister %d\tFunIndex:%d\tParmSize:%d", registerIndex,(int)unFunIndex, (int)params.size());
		return strbuff;
	}
	void CCallBackExeCode::Clear(CScriptExecBlock* pBlock)
	{
		for (unsigned int i = 0; i < params.size(); i++)
		{
			ReleaseParamInfo(pBlock,params[i]);
		}
	}
	int CCallScriptExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		int nResult = CScriptExecBlock::ERESULT_CONTINUE;
		//设置参数,倒序，方便从堆栈中取值
		for (int i = (int)params.size() - 1; i >= 0; i--)
		{
			MakeParamInfo(pBlock, params[i]);
		}
		if (paramsCache.size() != params.size())
		{
			paramsCache.resize(params.size());
		}
		for (unsigned int i = 0; i < params.size(); i++)
		{
			paramsCache[i] = params[i].pVar;
		}
		//运行回调函数
		switch (pBlock->m_pMaster->CallFun_Script(pBlock->m_pMaster->m_pMachine, unFunIndex, paramsCache, registerIndex))
		{
		case ECALLBACK_ERROR:
			nResult = CScriptExecBlock::ERESULT_ERROR;
			break;
		case ECALLBACK_WAITING:
			nResult = CScriptExecBlock::ERESULT_WAITING;
			break;
		case ECALLBACK_NEXTCONTINUE:
			nResult = CScriptExecBlock::ERESULT_NEXTCONTINUE;
			break;
		}

		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		Clear(pBlock);
		return nResult;
	}
	std::string CCallScriptExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "CallScript\tregister %d\tFunIndex:%d\tParmSize:%d",registerIndex, (int)unFunIndex, (int)params.size());
		return strbuff;
	}
	void CCallScriptExeCode::Clear(CScriptExecBlock* pBlock)
	{
		for (unsigned int i = 0; i < params.size(); i++)
		{
			ReleaseParamInfo(pBlock,params[i]);
		}
	}
	int CJumpExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		if (pNextPoint)
		{
			if (this->pJumpCode)
				*pNextPoint = this->pJumpCode;
			else
				*pNextPoint = m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CJumpExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "JUMP\tIndex:%d", (int)pJumpCode ? pJumpCode->nCodeIndex : 0);
		return strbuff;
	}
	bool CJumpExeCode::CheckCond(const CBaseVar* pVar)
	{
		if (pVar == nullptr)
		{
			return false;
		}
		if (pVar->GetType() == CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			const CIntVar* pIntVar = dynamic_cast<const CIntVar*>(pVar);
			if (pIntVar && pIntVar->ToInt() != 0)
			{
				return true;
			}
		}
		else if (pVar->GetType() == CScriptClassInfo<CFloatVar>::GetInstance().nClassType)
		{
			const CFloatVar* pFloatVar = dynamic_cast<const CFloatVar*>(pVar);
			if (pFloatVar && pFloatVar->ToFloat() != 0)
			{
				return true;
			}
		}
		else if (pVar->GetType() == CScriptClassInfo<CStringVar>::GetInstance().nClassType)
		{
			const CStringVar* pIntVar = dynamic_cast<const CStringVar*>(pVar);
			if (pIntVar && !pIntVar->ToString().empty())
			{
				return true;
			}
		}
		else
		{
			const CPointVar* pPoint = dynamic_cast<const CPointVar*>(pVar);
			if (pPoint && pPoint->ToPoint())
			{
				return true;
			}
		}
		return false;
	}
	int CJumpTrueExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		MakeParamInfo(pBlock, condParam);
		if (CheckCond(condParam.pVar))
		{
			if (pNextPoint)
			{
				*pNextPoint = this->pJumpCode;
			}
		}
		else
		{
			if (pNextPoint)
			{
				*pNextPoint = this->m_pNext;
			}
		}
		ReleaseParamInfo(pBlock,condParam);
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CJumpTrueExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "JUMP_TRUE\tcondParam(%d,%d)\tIndex:%d", condParam.nType, condParam.dwPos,(int)pJumpCode ? pJumpCode->nCodeIndex : 0);
		return strbuff;
	}
	int CJumpFalseExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		MakeParamInfo(pBlock, condParam);
		if (!CheckCond(condParam.pVar))
		{
			if (pNextPoint)
			{
				*pNextPoint = this->pJumpCode;
			}
		}
		else
		{
			if (pNextPoint)
			{
				*pNextPoint = this->m_pNext;
			}
		}
		ReleaseParamInfo(pBlock,condParam);
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CJumpFalseExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "JUMP_FALSE\tcondParam(%d,%d)\tIndex:%d", condParam.nType, condParam.dwPos, (int)pJumpCode ? pJumpCode->nCodeIndex : 0);
		return strbuff;
	}
	int CReturnExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		if (pNextPoint)
		{
			*pNextPoint = nullptr;
		}
		if (bNeedReturnVar)
		{
			MakeParamInfo(pBlock, returnParam);
			if (returnParam.pVar)
			{
				CBaseVar* pResult = pBlock->NewVar(returnParam.pVar->GetType());
				pResult->Set(returnParam.pVar);
				STACK_PUSH_MOVE(pBlock->registerStack, pResult);
			}
			//STACK_PUSH_COPY(pBlock->registerStack, returnParam.pVar);
			ReleaseParamInfo(pBlock,returnParam);
		}

		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CReturnExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "RETURN\tparam(%d,%d) ", returnParam.nType, returnParam.dwPos);
		return strbuff;
	}
	int CCallClassFunExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		int nResult = CScriptExecBlock::ERESULT_CONTINUE;
		MakeParamInfo(pBlock, object);
		for (int i = (int)params.size() - 1; i >= 0; i--)
		{
			MakeParamInfo(pBlock, params[i]);
		}
		const CPointVar* pPointVar = dynamic_cast<const CPointVar*>(object.pVar);
		if (pPointVar == nullptr)
		{
			Clear(pBlock);
			return CScriptExecBlock::ERESULT_ERROR;
		}
		CACHE_NEW(CScriptCallState, pCallState, pBlock->m_pMaster);
		if (pCallState)
		{
			//设置参数
			for (unsigned int i = 0; i < params.size(); i++)
			{
				STACK_PUSH_COPY(pCallState->m_stackRegister, params[i].pVar);
			}

			CScriptPointInterface* pPoint = pPointVar->ToPoint();
			if (pPoint)
			{
				switch (pPoint->RunFun(funIndex, pCallState))
				{
				case ECALLBACK_ERROR:
					nResult = CScriptExecBlock::ERESULT_ERROR;
					break;
				case ECALLBACK_WAITING:
					nResult = CScriptExecBlock::ERESULT_WAITING;
					break;
				case ECALLBACK_NEXTCONTINUE:
					nResult = CScriptExecBlock::ERESULT_NEXTCONTINUE;
					break;
				case ECALLBACK_FINISH:
					//执行完将结果放入寄存器
					if (pCallState->GetResult())
					{
						if (registerIndex < R_SIZE)
						{
							BLOCK_SCRIPTVAR_COPY_VAR(pBlock->fixedRegister[registerIndex], pCallState->GetResult());
						}
						else
						{
							CBaseVar* pResult = pBlock->NewVar(pCallState->GetResult()->GetType());
							pResult->Set(pCallState->GetResult());
							STACK_PUSH_MOVE(pBlock->registerStack, pResult);
						}
					}
					break;
				}
				if (pNextPoint)
				{
					*pNextPoint = m_pNext;
				}
			}
			else
			{
				nResult = CScriptExecBlock::ERESULT_ERROR;
			}
		}
		CACHE_DELETE(pCallState);
		Clear(pBlock);
		return nResult;
	}
	std::string CCallClassFunExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "CALL_CLASS_FUN\t reigster %d\tType:%d\tdwPos:%d\tFunIndex:%d\tParmSize:%d", registerIndex,(int)object.nType, (int)object.dwPos, (int)funIndex, (int)params.size());
		return strbuff;
	}
	void CCallClassFunExeCode::Clear(CScriptExecBlock* pBlock)
	{
		ReleaseParamInfo(pBlock,object);
		for (unsigned int i = 0; i < params.size(); i++)
		{
			ReleaseParamInfo(pBlock,params[i]);
		}
	}
	int CNewClassExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		int nResult = CScriptExecBlock::ERESULT_ERROR;
		auto pNewPoint = CScriptVarTypeMgr::GetInstance()->NewObject(dwClassIndex);
		if (pNewPoint)
		{
			CPointVar* pVar = (CPointVar*)CScriptVarTypeMgr::GetInstance()->GetVar(dwClassIndex);
			if (pVar)
			{
				pVar->Set(pNewPoint);
				if (registerIndex < R_SIZE)
				{
					BLOCK_SCRIPTVAR_COPY_VAR(pBlock->fixedRegister[registerIndex], pVar);
					CScriptVarTypeMgr::GetInstance()->ReleaseVar(pVar);
				}
				else
				{
					STACK_PUSH_MOVE(pBlock->registerStack, pVar);
				}
				nResult = CScriptExecBlock::ERESULT_CONTINUE;
			}
		}
		if (pNextPoint)
		{
			*pNextPoint = m_pNext;
		}
		return nResult;
	}
	std::string CNewClassExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "NEW_CLASS\tregister:%d\tClassIndex:%d", (int)registerIndex, (int)dwClassIndex);
		return strbuff;
	}
	//	int CReleaseClassExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	//	{
	//		PointVarInfo pointVal = GetPoint_StackVar(&pBlock->m_register[cVarRegister]);
	//		CScriptBasePointer* pPoint = pointVal.pPoint;
	//		if (pPoint)
	//		{
	//			CBaseScriptClassMgr* pMgr = CScriptSuperPointerMgr::GetInstance()->GetClassMgr(pPoint->GetType());
	//			if (pMgr)
	//			{
	//				pMgr->Release(pPoint);
	//			}
	//		}
	//		if (pNextPoint)
	//		{
	//			*pNextPoint = m_pNext;
	//		}
	//		return CScriptExecBlock::ERESULT_CONTINUE;
	//	}
	//	std::string CReleaseClassExeCode::GetCodeString()
	//	{
	//		char strbuff[128] = { 0 };
	//		sprintf(strbuff, "RELEASE_CLASS\tVarRegister:%d", (int)cVarRegister);
	//		return strbuff;
	//	}
		int CBreakExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
		{
			if (pNextPoint)
			{
				if (this->pJumpCode)
					*pNextPoint = this->pJumpCode;
				else
					*pNextPoint = m_pNext;
			}
			return CScriptExecBlock::ERESULT_CONTINUE;
		}
	
		std::string CBreakExeCode::GetCodeString()
		{
			char strbuff[128] = { 0 };
			sprintf(strbuff, "BREAK\tIndex:%d", (int)pJumpCode ? pJumpCode->nCodeIndex : 0);
			return strbuff;
		}
	
		int CContinueExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
		{
			if (pNextPoint)
			{
				if (this->pJumpCode)
					*pNextPoint = this->pJumpCode;
				else
					*pNextPoint = m_pNext;
			}
			return CScriptExecBlock::ERESULT_CONTINUE;
		}
		std::string CContinueExeCode::GetCodeString()
		{
			char strbuff[128] = { 0 };
			sprintf(strbuff, "CONTINUE\tIndex:%d", (int)pJumpCode ? pJumpCode->nCodeIndex : 0);
			return strbuff;
		}
	int CSignExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		if (pNextPoint)
		{
			*pNextPoint = m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}

	std::string CSignExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "SIGN");
		return strbuff;
	}

	CExeCodeMgr CExeCodeMgr::s_Instance;
	CExeCodeMgr::CExeCodeMgr()
	{
	}
	CExeCodeMgr::~CExeCodeMgr()
	{
	}
	void CExeCodeMgr::Clear()
	{
		for (auto it = m_listExeCode.begin(); it != m_listExeCode.end(); it++)
		{
			CBaseExeCode* pCode = *it;
			if (pCode)
			{
				delete pCode;
			}
		}
		m_listExeCode.clear();
	}




}