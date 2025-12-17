#include"../common.h"

//					1. 线程安全问题
//					2. 临界区    CRITICAL_SECTION
//					3. 互斥体
//					4. 事件: 互斥
//					5. 事件: 通知
//					6. 事件: 有序
//					7. 信号量
//					8. 定时器的手工重置和自动重置




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					1. 线程安全问题

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static int g_tickets_s1 = 1000;

static DWORD WINAPI ThreadProc_s1(LPVOID lpParameter)
{
	DWORD param = (DWORD)(UINT64)lpParameter;

	while (g_tickets_s1 > 0)
	{
		// 即便是单线程,也是有线程安全问题的,可能在这里发生线程切换,但是此时g_tickets已经减没了.
		// 再减就出负数了.
		printf("thread:%d 还有%d张票\n", param, g_tickets_s1);
		g_tickets_s1--;
		printf("thread:%d 卖出一张,还有%d张票\n", param, g_tickets_s1);
	}
	return 0;
	/*
	thread:2 卖出一张,还有2张票
	thread:2 还有2张票
	thread:2 卖出一张,还有1张票
	thread:2 还有1张票
	thread:2 卖出一张,还有0张票
	thread:1 卖出一张,还有447张票	这里就错了
	ok
	console end
	*/
}

static void s1()
{

	DWORD threadID = 0;
	LPVOID threadParam = (LPVOID)0x1;
	HANDLE aThreadHandle[2] = { 0 };

	aThreadHandle[0] = CreateThread(NULL, 0, ThreadProc_s1, threadParam, 0, &threadID);
	if (!aThreadHandle[0])
	{
		printf("CreateThread 1 error: %d\n", GetLastError());
		return;
	}

	threadParam = (LPVOID)0x2;
	aThreadHandle[1] = CreateThread(NULL, 0, ThreadProc_s1, threadParam, 0, &threadID);
	if (!aThreadHandle[1])
	{
		printf("CreateThread 2 error: %d\n", GetLastError());
		return;
	}

	WaitForMultipleObjects(2, aThreadHandle, TRUE, INFINITE);

	printf("ok\n");

	CloseHandle(aThreadHandle[0]);
	CloseHandle(aThreadHandle[1]);
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					2. 临界区    CRITICAL_SECTION

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


static CRITICAL_SECTION cs_s2;
static int g_tickets_s2 = 1000;

static DWORD WINAPI ThreadProc_s2(LPVOID lpParameter)
{
	DWORD param = (DWORD)(UINT64)lpParameter;

	while (1)
	{
		EnterCriticalSection(&cs_s2);	// 进入临界区,锁定资源.
		if (g_tickets_s2 > 0)		// 所有有威胁的操作都要放到锁后.
		{
			printf("thread:%d 还有%d张票\n", param, g_tickets_s2);
			g_tickets_s2--;
			printf("thread:%d 卖出一张,还有%d张票\n", param, g_tickets_s2);
			LeaveCriticalSection(&cs_s2);  // 退出临界区,解锁资源.
		}
		else
		{
			LeaveCriticalSection(&cs_s2);  // 退出临界区,解锁资源.
			break;
		}
	}
	return 0;
}

static void s2()
{
	InitializeCriticalSection(&cs_s2);

	DWORD threadID = 0;
	LPVOID threadParam = (LPVOID)0x1;
	HANDLE aThreadHandle[2] = { 0 };

	aThreadHandle[0] = CreateThread(NULL, 0, ThreadProc_s2, threadParam, 0, &threadID);
	if (!aThreadHandle[0])
	{
		printf("CreateThread 1 error: %d\n", GetLastError());
		return;
	}

	threadParam = (LPVOID)0x2;
	aThreadHandle[1] = CreateThread(NULL, 0, ThreadProc_s2, threadParam, 0, &threadID);
	if (!aThreadHandle[1])
	{
		printf("CreateThread 2 error: %d\n", GetLastError());
		return;
	}

	WaitForMultipleObjects(2, aThreadHandle, TRUE, INFINITE);

	printf("ok\n");

	CloseHandle(aThreadHandle[0]);
	CloseHandle(aThreadHandle[1]);

	DeleteCriticalSection(&cs_s2);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					3. 互斥体

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static HANDLE mutex_s3;
static int g_tickets_s3 = 1000;

static DWORD WINAPI ThreadProc_s3(LPVOID lpParameter)
{
	DWORD param = (DWORD)(UINT64)lpParameter;

	while (1)
	{
		WaitForSingleObject(mutex_s3, INFINITE);
		if (g_tickets_s3 > 0)
		{
			printf("thread:%d 还有%d张票\n", param, g_tickets_s3);
			g_tickets_s3--;
			printf("thread:%d 卖出一张,还有%d张票\n", param, g_tickets_s3);
			ReleaseMutex(mutex_s3);
		}
		else
		{
			ReleaseMutex(mutex_s3);
			break;
		}
	}
	return 0;
}

static void s3()
{
	mutex_s3 = CreateMutexA(NULL, FALSE/*FALSE创建时有信号,TRUE创建时没信号要自己release*/, "ttt");
	if (!mutex_s3)
	{
		printf("CreateMutexA error: %d\n", GetLastError());
		return;
	}

	DWORD threadID = 0;
	LPVOID threadParam = (LPVOID)0x1;
	HANDLE aThreadHandle[2] = { 0 };

	aThreadHandle[0] = CreateThread(NULL, 0, ThreadProc_s3, threadParam, 0, &threadID);
	if (!aThreadHandle[0])
	{
		printf("CreateThread 1 error: %d\n", GetLastError());
		return;
	}

	threadParam = (LPVOID)0x2;
	aThreadHandle[1] = CreateThread(NULL, 0, ThreadProc_s3, threadParam, 0, &threadID);
	if (!aThreadHandle[1])
	{
		printf("CreateThread 2 error: %d\n", GetLastError());
		return;
	}

	WaitForMultipleObjects(2, aThreadHandle, TRUE, INFINITE);

	printf("ok\n");

	CloseHandle(aThreadHandle[0]);
	CloseHandle(aThreadHandle[1]);

	CloseHandle(mutex_s3);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					4. 事件: 互斥

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static HANDLE event_s4;
static int g_tickets_s4 = 1000;

static DWORD WINAPI ThreadProc_s4(LPVOID lpParameter)
{
	DWORD param = (DWORD)(UINT64)lpParameter;

	while (1)
	{
		WaitForSingleObject(event_s4, INFINITE);
		if (g_tickets_s4 > 0)
		{
			printf("thread:%d 还有%d张票\n", param, g_tickets_s4);
			g_tickets_s4--;
			printf("thread:%d 卖出一张,还有%d张票\n", param, g_tickets_s4);
			SetEvent(event_s4);
		}
		else
		{
			SetEvent(event_s4);
			break;
		}
	}
	return 0;
}

static void s4()
{
	event_s4 = CreateEventA(NULL, FALSE, FALSE, "ttt");
	if (!event_s4)
	{
		printf("CreateEventA 1 error: %d\n", GetLastError());
		return;
	}

	DWORD threadID = 0;
	LPVOID threadParam = (LPVOID)0x1;
	HANDLE aThreadHandle[2] = { 0 };

	aThreadHandle[0] = CreateThread(NULL, 0, ThreadProc_s4, threadParam, 0, &threadID);
	if (!aThreadHandle[0])
	{
		printf("CreateThread 1 error: %d\n", GetLastError());
		return;
	}

	threadParam = (LPVOID)0x2;
	aThreadHandle[1] = CreateThread(NULL, 0, ThreadProc_s4, threadParam, 0, &threadID);
	if (!aThreadHandle[1])
	{
		printf("CreateThread 2 error: %d\n", GetLastError());
		return;
	}

	int i = 0;
	for (i = 0; i < 5; i++)
	{
		Sleep(1000);
		printf("主线程 %d\n", i);
	}
	SetEvent(event_s4);


	WaitForMultipleObjects(2, aThreadHandle, TRUE, INFINITE);

	printf("ok\n");

	CloseHandle(aThreadHandle[0]);
	CloseHandle(aThreadHandle[1]);

	CloseHandle(event_s4);

}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					5. 事件: 通知

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static HANDLE event_s5;

static DWORD WINAPI ThreadProc_s5(LPVOID lpParameter)
{
	DWORD param = (DWORD)(UINT64)lpParameter;

	printf("%d 线程开始等待\n", param);

	WaitForSingleObject(event_s5, INFINITE);

	printf("%d 线程执行\n", param);

	return 0;
}

static void s5()
{
	// 通知类型,N个线程等待这个事件直到其有信号
	event_s5 = CreateEventA(NULL, TRUE, FALSE, "ttt");
	if (!event_s5)
	{
		printf("CreateEventA 1 error: %d\n", GetLastError());
		return;
	}

	DWORD threadID = 0;
	LPVOID threadParam = (LPVOID)0x1;
	HANDLE aThreadHandle[2] = { 0 };

	aThreadHandle[0] = CreateThread(NULL, 0, ThreadProc_s5, threadParam, 0, &threadID);
	if (!aThreadHandle[0])
	{
		printf("CreateThread 1 error: %d\n", GetLastError());
		return;
	}

	threadParam = (LPVOID)0x2;
	aThreadHandle[1] = CreateThread(NULL, 0, ThreadProc_s5, threadParam, 0, &threadID);
	if (!aThreadHandle[1])
	{
		printf("CreateThread 2 error: %d\n", GetLastError());
		return;
	}

	int i = 0;
	for (i = 0; i < 5; i++)
	{
		Sleep(1000);
		printf("主线程 %d\n", i);
	}
	SetEvent(event_s5);


	WaitForMultipleObjects(2, aThreadHandle, TRUE, INFINITE);

	printf("ok\n");

	CloseHandle(aThreadHandle[0]);
	CloseHandle(aThreadHandle[1]);

	CloseHandle(event_s5);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					6. 事件: 有序

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static DWORD g_num_s6 = 0;
static HANDLE g_hEvent_producer_s6, g_hEvent_consumer_s6;

static DWORD WINAPI ThreadProrProducer_s6(LPVOID lpParameter)
{
	for (int i = 0; i < 100; i++)
	{
		WaitForSingleObject(g_hEvent_producer_s6, INFINITE);
		g_num_s6++;
		printf("生产者生产1   num:%d\n", g_num_s6);
		SetEvent(g_hEvent_consumer_s6);
	}
	return 0;
}

static DWORD WINAPI ThreadProrConsumer_s6(LPVOID lpParameter)
{
	for (int i = 0; i < 100; i++)
	{
		WaitForSingleObject(g_hEvent_consumer_s6, INFINITE);
		g_num_s6--;
		printf("消费者消费1   num:%d\n", g_num_s6);
		SetEvent(g_hEvent_producer_s6);
	}
	return 0;
}

static void s6()
{
	g_hEvent_producer_s6 = CreateEventA(NULL, FALSE, FALSE, "producer");
	if (!g_hEvent_producer_s6)
	{
		printf("CreateEventA 1 error: %d\n", GetLastError());
		return;
	}

	g_hEvent_consumer_s6 = CreateEventA(NULL, FALSE, FALSE, "consumer");
	if (!g_hEvent_consumer_s6)
	{
		printf("CreateEventA 1 error: %d\n", GetLastError());
		return;
	}


	DWORD threadID = 0;
	LPVOID threadParam = (LPVOID)0x1;
	HANDLE aThreadHandle[2] = { 0 };

	aThreadHandle[0] = CreateThread(NULL, 0, ThreadProrProducer_s6, threadParam, 0, &threadID);
	if (!aThreadHandle[0])
	{
		printf("CreateThread 1 error: %d\n", GetLastError());
		return;
	}

	threadParam = (LPVOID)0x2;
	aThreadHandle[1] = CreateThread(NULL, 0, ThreadProrConsumer_s6, threadParam, 0, &threadID);
	if (!aThreadHandle[1])
	{
		printf("CreateThread 2 error: %d\n", GetLastError());
		return;
	}

	int i = 0;
	for (i = 0; i < 5; i++)
	{
		Sleep(1000);
		printf("主线程 %d\n", i);
	}

	SetEvent(g_hEvent_producer_s6);


	WaitForMultipleObjects(2, aThreadHandle, TRUE, INFINITE);

	printf("ok\n");

	CloseHandle(aThreadHandle[0]);
	CloseHandle(aThreadHandle[1]);


	CloseHandle(g_hEvent_producer_s6);
	CloseHandle(g_hEvent_consumer_s6);

}



void p000_004()
{
	s6();
}
