#include<windows.h>
#include<stdio.h>

// a.def   导出配置文件
#if 0
EXPORTS
Test3 @15
#endif

// 通过声明其为导出函数，在编译时会自动生成一个导出表
__declspec(dllexport) void Test1();				// 声明其为CPP导出函数
EXTERN_C __declspec(dllexport) void Test2();	// 声明其为C导出函数

// cpp导出函数
void Test1()
{
	printf("Test1\n");
}

// c导出函数
EXTERN_C void Test2()
{
	printf("Test2\n");
}

// 配置文件中声明的导出函数
void Test3()
{
	printf("Test3\n");
}

// DLL入口点函数
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:		// DLL被加载时执行，可以用来初始化DLL
		MessageBoxA(0, 0, 0, 0);
		//printf("DLL_PROCESS_ATTACH\n");
		break;	
	case DLL_PROCESS_DETACH:		// DLL被卸载时执行，可以用来清理DLL
		//printf("DLL_PROCESS_DETACH\n");
		break;
	case DLL_THREAD_ATTACH:			// 线程执行，可以用来初始化线程
		//printf("DLL_THREAD_ATTACH\n");
		break;
	case DLL_THREAD_DETACH:			// 卸载线程执行，可以用来清理线程
		// printf("DLL_THREAD_DETACH\n");
		break;
	}
	return TRUE;
}

#if 0
// 动态链接库的使用
static void s1()
{
	// 方式1: 手动加载
	typedef void (*PFUNC)();
	HMODULE hModule = LoadLibraryA("TestDLL.dll");
	PFUNC pFunc = (PFUNC)GetProcAddress(hModule, "Test1");
	if (pFunc)
	{
		pFunc();
	}
	FreeLibrary(hModule);
}
#endif

#if 0
// 方式2: 自动加载
#pragma comment(lib, "TestDLL.lib")	// 将dll的lib文件加入到项目中
__declspec(dllimport) void Test1();				// 声明其为CPP导入函数
EXTERN_C __declspec(dllimport) void Test2();	// 声明其为C导入函数
void Test3();	// 配置文件中声明的导出函数

static void s2()
{
	Test1();
	Test2();
	Test3();
}
#endif
