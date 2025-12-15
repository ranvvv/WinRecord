#include"../common.h"

//					1. 线程的创建和等待
//					2. 线程管理
//					3. context与挂起/恢复
//					4. 远程线程
//					5. 纤程
//					6. QueueApc
//					7. C兼容线程


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					1. 线程的创建和等待

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


static DWORD WINAPI ThreadProc_s1(LPVOID lpParameter)
{
	DWORD param = (DWORD)(UINT64)lpParameter;

	printf("new thread begin %x \n", param);
	int i = 0;
	for (i = 0; i < 3; i++)
	{
		printf("new thread: %x\n", i);
		Sleep(1000);
	}
	return 0;
}

static void s1()
{
	DWORD result;
	DWORD threadID = 0;
	LPVOID threadParam = (LPVOID)0x111;
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc_s1, threadParam, 0, &threadID);
	if (!hThread)
	{
		printf("CreateThread error: %d\n", GetLastError());
		return;
	}
	/*
	CreateThread(
	1. LPSECURITY_ATTRIBUTES lpThreadAttributes, 		// 安全描述符  NULL则不让继承
	2. NULL SIZE_T dwStackSize,   						// 初始堆栈大小  0 默认堆栈
	3. LPTHREAD_START_ROUTINE lpStartAddress, 			// 代码起始点,真正执行的代码函数指针
	4. LPVOID lpParameter,   					 		// 线程函数参数指针,注意参数生命周期
	5. DWORD dwCreationFlags,  							// 创建线程的标识,
														//  0: 可被调度,
														//  CREATE_SUSPENDED: 挂起状态创建 挂起就是让线程处于不可调度状态, 系统不会给这个线程分配CPU, 可用ResumeThread恢复
	6. LPDWORD lpThreadId  								//  OUT指针 保存线程id
	);
	*/

	CloseHandle(hThread);


	hThread = OpenThread(THREAD_ALL_ACCESS | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_QUERY_INFORMATION, false, threadID);
	if (!hThread)
	{
		printf("OpenThread failed (%d).\n", GetLastError());
		return;
	}

	result = WaitForSingleObject(hThread, INFINITE);	// 等待线程,线程结束后才会返回.
	switch (result)
	{
	case WAIT_TIMEOUT:
	{
		printf("WaitForSingleObject timed out.\n");
		break;
	}
	case WAIT_FAILED:
	{
		printf("WaitForSingleObject failed (%d).\n", GetLastError());
		return;
		break;
	}
	case WAIT_OBJECT_0:
	{
		printf("WaitForSingleObject succeeded.\n");
		break;
	}
	default:
	{
		printf("WaitForSingleObject unknown result.\n");
		break;
	}
	}

	printf("get new thread end\n");

	CloseHandle(hThread);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					2. 线程管理函数

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s2()
{

	ExitThread(0);
	TerminateThread(GetCurrentThread(), 0);

	GetThreadPriority(GetCurrentThread());
	GetThreadPriorityBoost(GetCurrentThread(), NULL);
	Sleep(1000);
	SleepEx(1000, true/*可被APC唤醒*/);
	SuspendThread(GetCurrentThread());
	SwitchToThread();
	ResumeThread(GetCurrentThread());
	GetExitCodeThread(GetCurrentThread(), NULL);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					3. context与挂起/恢复

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


static DWORD WINAPI ThreadProc_s3(LPVOID lpParameter)
{
#ifdef _WIN64
	DWORD param = (DWORD)(UINT64)lpParameter;
#else
	DWORD param = (DWORD)lpParameter;
#endif
	printf("new thread begin \n");
	int i = 0;
	for (i = 0; i < 3; i++)
	{
		printf("new thread: %x\n", i);
		Sleep(1000);
	}
	return 0;
}

static void s3()
{
	DWORD result;
	DWORD threadID = 0;
	LPVOID threadParam = (LPVOID)0x111;
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc_s3, threadParam/*NULL*/, 0/*CREATE_SUSPENDED*/, &threadID);
	if (!hThread)
	{
		printf("CreateThread error: %d\n", GetLastError());
		return;
	}

	Sleep(1000);

	//暂停线程
	result = SuspendThread(hThread);
	if (result == -1)
	{
		printf("SuspendThread failed (%d).\n", GetLastError());
		return;
	}


	CONTEXT context = { 0 };//定义线程信息结构
	context.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;	// 要获取所有值

	// 获取  线程上下文环境
	if (!GetThreadContext(hThread, &context))
	{
		printf("GetThreadContext failed (%d).\n", GetLastError());
		return;
	}

#ifdef _WIN64
	printf("thread RIP:0x%I64x\n", context.Rip);
	printf("thread Rbx:0x%I64x\n", context.Rbx);
	printf("thread Rax:0x%I64x\n", context.Rax);

	/*
	// 修改线程上下文环境.可以修改线程的执行流程.挂起来, 改变, 再恢复.线程级别的hook
	context.Rip = 0x1111111111111111;
	if (!SetThreadContext(hThread, &context))
	{
		printf("SetThreadContext failed (%d).\n", GetLastError());
		return;
	}
	*/
#else
	printf("thread EIP:0x%08x\n", context.Eip);
	printf("thread Ebx:0x%08x\n", context.Ebx);
	printf("thread Eax:0x%08x\n", context.Eax);

	/*
	// 修改线程上下文环境.可以修改线程的执行流程.     挂起来,改变,再恢复. 线程级别的hook
	context.Eip = 0x11111111;
	if(!SetThreadContext(hThread, &context))
	{
		printf("SetThreadContext failed (%d).\n", GetLastError());
		return;
	}
	*/
#endif

	// 恢复执行
	result = ResumeThread(hThread);
	if (result == -1)
	{
		printf("ResumeThread failed (%d).\n", GetLastError());
		return;
	}

	result = WaitForSingleObject(hThread, INFINITE);	// 等待线程,线程结束后才会返回.
	if (result == WAIT_FAILED)	//WAIT_OBJECT_0    WAIT_TIMEOUT    WAIT_ABANDONED
	{
		printf("WaitForSingleObject failed (%d).\n", GetLastError());
		return;
	}

	CloseHandle(hThread);
}





// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					4. 远程线程

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


static DWORD WINAPI ThreadProc_s4(LPVOID lpParameter)
{
	DWORD param = (DWORD)(UINT64)lpParameter;

	printf("new thread begin %x \n", param);
	int i = 0;
	for (i = 0; i < 3; i++)
	{
		printf("new thread: %x\n", i);
		Sleep(1000);
	}
	return 0;
}

static void s4()
{
	HANDLE hThread = CreateRemoteThread(GetCurrentProcess(), NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc_s4, (LPVOID)0x888, 0, NULL);
	if (!hThread)
	{
		printf("CreateRemoteThread failed (%d).\n", GetLastError());
		return;
	}
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					5. 纤程

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static LPVOID lpFbMain;

static void PfiberStartRoutine(LPVOID lpFiberParameter)
{
	printf("child fiber run \n");
	SwitchToFiber(lpFbMain);
}

static void s5()
{
	LPVOID lpFb = CreateFiber(0, (LPFIBER_START_ROUTINE)PfiberStartRoutine, (LPVOID)0x888);
	if (!lpFb)
	{
		printf("CreateFiber failed (%d).\n", GetLastError());
		return;
	}

	printf("thread to fiber \n");
	lpFbMain = ConvertThreadToFiber((LPVOID)0x1);
	printf("begin \n");
	SwitchToFiber(lpFb);
	printf("end \n");
	ConvertFiberToThread();
	printf("fiber to thread \n");

	DeleteFiber(lpFb);
}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					6. QueueApc

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static VOID WINAPI APCProc(LPVOID dwParam)    // APC回调函数
{
	printf("apc run ========== \n");
	//ExitThread(0);
}

//线程函数
static DWORD WINAPI ThreadProc_s6(LPVOID lpParameter)
{
	while (1)
	{
		//Sleep(1000);   					// sleep是不可唤醒的. 所以不会改变UserApcPending
		SleepEx(1000, TRUE);				// ex可以开启UserApcPending , 所以如果用Ex了  APC就能执行
		printf("thread proc: \n");
	}
	return 0;	// 线程返回值
}


static void s6()
{
	// 创建线程代码
	DWORD ThreadId;
	HANDLE hThread;
	hThread = CreateThread(NULL, 0, ThreadProc_s6, NULL, 0, &ThreadId);   // 刚创建的线程UserApcPending 是为1 的,所以可以执行
	if (!hThread)
	{
		printf("CreateThread error: %d\n", GetLastError());
		return;
	}

#if 0
	// 1.不sleep的情况,新线程的UserApcPending=1,所以只要这里执行的够快的话,新线程就有机会执行APC函数.
	QueueUserAPC((PAPCFUNC)APCProc, hThread, 0);

#else
	// 2. sleep 3秒,保证子线程已经跑起来了,再插入,APC就不会执行了.因为子线程已经运行一会了初始化时的UserApcPending已经被消耗了.
	Sleep(3000);
	QueueUserAPC((PAPCFUNC)APCProc, hThread, 0);
#endif // 1

	system("pause");
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					7. C兼容线程

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

unsigned __stdcall ThreadProc_s7(void* param)
{
	for (int i = 0; i < 10; i++)
	{
		printf("new thread: %d\n", i);
		Sleep(1000);
	}
	_endthreadex(0);
	return 0;
}


static void s7()
{
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void*))ThreadProc_s7, NULL, 0, NULL);
	if (hThread == (HANDLE)-1)
	{
		char buf[1024];
		strerror_s(buf, 1024, errno);
		printf("CreateThread error: %s\n", buf);
		return;
	}
}










// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_003()
{
	s7();
}
