#include<windows.h>
#include<stdio.h>

// 动态链接库的使用
static void s1()
{
	// 方式1: 手动加载
	typedef void (*PFUNC)();
	HMODULE hModule = LoadLibraryA("../TestDll/dist/TestDLL.dll");
	PFUNC pFunc = (PFUNC)GetProcAddress(hModule, "Test1");
	if (pFunc)
	{
		pFunc();
	}
	FreeLibrary(hModule);
}

#if 0
// 方式2: 自动加载
#pragma comment(lib, "./TestDll/dist/TestDLL.lib")	// 将dll的lib文件加入到项目中
__declspec(dllimport) void Test1();					// 声明其为CPP导入函数
EXTERN_C __declspec(dllimport) void Test2();		// 声明其为C导入函数
void Test3();	// 配置文件中声明的导出函数

//  需要把TestDll.dll 拷贝到工作目录下

static void s2()
{
	Test1();
	Test2();
	Test3();
}
#endif


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void p000_011()
{
	//s2();
}
