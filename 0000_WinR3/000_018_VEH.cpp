#include "common.h"

//				 1. VEH的使用
//				 2. SEH的基本使用




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				 1. VEH的使用

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static LONG CALLBACK ExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	TCHAR str[100] = { 0 };
	wsprintf(str, TEXT("Exception EIP:%p"), (UINT64)pExceptionInfo->ExceptionRecord->ExceptionAddress);
	MessageBox(NULL, str, TEXT("title"), 0);

#ifdef _WIN64
	// 可以获取报异常的EIP位置
	if (pExceptionInfo->ExceptionRecord->ExceptionAddress == (PVOID)0x401053)
	{
		// 让EIP跳过当前指令,到下一个执行继续执行
		pExceptionInfo->ContextRecord->Rip += 6;
		TCHAR str1[100] = { 0 };
		wsprintf(str1, TEXT("EIP跳转到:%p"), pExceptionInfo->ContextRecord->Rip);
		MessageBox(NULL, str1, TEXT("title"), 0);

		// 让线程继续执行
		return EXCEPTION_CONTINUE_EXECUTION;
	}
#else
	// 可以获取报异常的EIP位置
	if (pExceptionInfo->ExceptionRecord->ExceptionAddress == (PVOID)0x401053)
	{
		// 让EIP跳过当前指令,到下一个执行继续执行
		pExceptionInfo->ContextRecord->Eip += 6;
		TCHAR str1[100] = { 0 };
		wsprintf(str1, TEXT("EIP跳转到:%x"), pExceptionInfo->ContextRecord->Eip);
		MessageBox(NULL, str1, TEXT("title"), 0);

		// 让线程继续执行
		return EXCEPTION_CONTINUE_EXECUTION;
	}
#endif
	// 调用下一个异常处理
	return EXCEPTION_CONTINUE_SEARCH;
}

static void s1()
{
	AddVectoredExceptionHandler(1/*首个处理*/, ExceptionHandler);
}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				 2. SEH的基本使用

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s2()
{
	__try {
		printf("in try 1\n");
		__leave;			// 脱离本次try
		printf("1 try end\n");
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		printf("in except\n");
	}

	printf("after try\n");
	__try {
		printf("in try 2\n");
		__leave;			// 脱离本次try
		printf("2 try end\n");
	}
	__finally {
		printf("in finally\n");
	}
}








void p000_018()
{
	s2();
}