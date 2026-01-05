#include <windows.h>
#include <stdio.h>


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
