# zlscript
自制的一个脚本语言系统

#### 概况：

​	本脚本系统编写的目的，是为了更好的辅助c++的开发，降低c++代码之间的耦合度，方便灵活的调用基于c++编写的模块。

​	本脚本系统以脚本函数为基本执行单位，使用线程池来异步执行脚本，所以对应c++模块要考虑多线程执行。

​	脚本函数需要执行时，会放入等待执行队列，线程池中的执行线程会从队列中抽取等待执行的脚本进行执行；如果脚本执行时遇到暂停，会移出执行线程，等待条件满足后，又会放入等待执行队列，等待执行线程抽取执行。

#### 语法：

​    基本上与c/c++的语法相似。
​    有三种基本类型,可自动转换，转换时以操作符左侧的类型为基准:
​                  int: 64位整形
​                  float: 64位浮点
​                  string: 字符串
​    运算符支持: 加'+',减'-',乘'*',除'/',求余'%',赋值'=';
​               等于'==',不等于'!=',大于'>',大于等于'>=',小于'<',小于等于'<=';
​    以及位运算符: 与'$',或'|',异或'^'。
​    

语句最后必须以';'号结束。

###### 函数定义：

c/c++的语法相似，例:

```
      int Fun(int a, int b)
      {
        return a+b;
      }
```

###### 循环：

目前只支持while语句，与c/c++不同，必须用'{}'将循环体包裹,例：

```
      int Fun(int a, int b)
      {
        int i = 0;
        while(a > 0)
        {
          i = i + b;
          a = a - 1;
        }
        return i;
      }
```

######  选择分支：

目前只支持if-else，与c/c++不同，必须用'{}'将分支语句包裹,例：

```
      int Fun(int a, int b)
      {
        int i = 0;
        if (a < 0)
        {
          i = 1;
        }
        else if (a < b)
        {
          i = 2;
        }
        else
        {
          i = 3;
        }
        return i;
      }
```



######    回调函数:

​      指定格式的c/c++函数，可以注册给脚本使用，调用起来与普通的脚本函数一样，例:print为一个默认的回调函数:

```
      void Fun1()
      {
        print("hello world");
      }
```

​      默认的回调函数都在ScriptCallBackFunion.h/cpp中，也可以自定义回调函数，并注册。
​      c/c++中:
​      定义回调函数格式:
$$
int [函数名](CScriptVirtualMachine* pMachine, CScriptRunState* pState);
$$
​      注册回调函数:
$$
CScriptCallBackFunion::GetInstance()->RegisterFun([脚本中的函数名],(C_CallBackScriptFunion)[函数名]);
$$
​      

######    类的调用:

​       可以将指定格式的c++类，注册后交与脚本使用。
​       首先，c++类必须继承CScriptPointInterface，例:
​       **c++代码**:

```c++
          class CTest : public CScriptPointInterface
          {
          };
```

​       然后，需要声明和定义，脚本可以调用的类函数,格式为
$$
int [函数名](CScriptRunState* pState)
$$
,例:
       **c++代码**:

```c++
          class CTest : public CScriptPointInterface
          {
          public:
            int Fun2Script(CScriptRunState* pState)
            {
              if (pState == nullptr)
              {
                return ECALLBACK_ERROR;//如果函数执行的状态机不存在，返回错误
              }
              int nVal1 = pState->PopIntVarFormStack();//取一个int类型的参数
              int nVal2 = pState->PopIntVarFormStack();//取一个int类型的参数

              pState->ClearFunParam();//清空多余的函数参数(如果调用函数时参数填多了的话)
              pState->PushVarToStack(nVal1 + nVal2);//压入返回值
              return ECALLBACK_FINISH;//函数调用完成
           }
        };
```

​        在编译脚本的代码前，必须要注册类名和类的函数名以便编译器识别:
​        **c++代码**:

```c++
      RegisterClassType("CTest", CTest);//注册CTest类
      RegisterClassFunName(Fun, CTest);//注册CTest类的Fun函数
```

​        最后，在类实例化的时候，需要将类实例的指针和函数指针传给脚本执行器,可以写在类的构造函数里:
​        **c++代码**:

```c++
          class CTest : public CScriptPointInterface
          {
          public:
            CTest()
            {
              AddClassObject(CScriptPointInterface::GetScriptPointIndex(), this);//注册类实例指针
	  		  RegisterClassFun(Fun, this, &CTest::Fun2Script);//注册类函数指针
            }
          public:
            int Fun2Script(CScriptRunState* pState);
          };
```

​        **完整c++代码：**

```c++
		class CTest : public CScriptPointInterface
        {
        public:
            CTest()
            {
				AddClassObject(CScriptPointInterface::GetScriptPointIndex(), this);//注册类实例指针
				RegisterClassFun(Fun, this, &CTest::Fun2Script);//注册类函数指针
            }
            static void init()//在程序初始化时，读取编译脚本之前，运行此函数
            {
                RegisterClassType("CTest", CTest);//为编译器注册CTest类
      			RegisterClassFunName(Fun, CTest);//为编译器注册CTest类的Fun函数
            }
        public:
            int Fun2Script(CScriptRunState* pState)
            {
              	if (pState == nullptr)
              	{
              	  return ECALLBACK_ERROR;//如果函数执行的状态机不存在，返回错误
              	}
              	int nVal1 = pState->PopIntVarFormStack();//取一个int类型的参数
              	int nVal2 = pState->PopIntVarFormStack();//取一个int类型的参数

              	pState->ClearFunParam();//清空多余的函数参数(如果调用函数时参数填多了的话)
              	pState->PushVarToStack(nVal1 + nVal2);//压入返回值
              	return ECALLBACK_FINISH;//函数调用完成
           }
        };
```

​		在脚本中就可以这样使用:

```
          int Fun(int a, int b)
          {
            CTest pTest = new CTest;
            int nResult = pTest->Fun(a,b);
            delete pTest;
            return nResult;
          }
```

​        注1:可以使用'new''delete'在脚本中实例化和释放类，也可以创建一个回调函数来实例化类，并在C++中自己对类实例进行管理。
​        注2:如果在脚本外类实例被释放掉的话，脚本内调用这个类实例的函数会停止执行，并返回错误。

​		注3:现在可以注册抽象类，抽象类无法使用new''delete'指令，需要创建专门的回调函数。

​		注4:为了一定程度支持多线程，脚本调用类函数时会加锁，也就是同一时间一个类对象在脚本中只能调用一个类函数。