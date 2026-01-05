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




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					7. 信号量

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static HANDLE g_hSemaphore_s7;
static CRITICAL_SECTION g_cs_s7;
static int g_tickets_s7 = 1000;

static DWORD WINAPI ThreadProrSemaphore_s7(LPVOID lpParameter)
{
	DWORD param = (DWORD)(UINT64)lpParameter;

	while (1)
	{
		WaitForSingleObject(g_hSemaphore_s7, INFINITE);
		EnterCriticalSection(&g_cs_s7);		// 由于有2个线程同时访问,所以还是要加锁才行.
		if (g_tickets_s7 > 0)
		{
			// 即便是单线程,也是有线程安全问题的,可能在这里发生线程切换,但是此时g_tickets已经减没了.
			printf("thread:%d 还有%d张票\n", param, g_tickets_s7);
			g_tickets_s7--;
			printf("thread:%d 卖出一张,还有%d张票\n", param, g_tickets_s7);
		}
		else
		{
			LeaveCriticalSection(&g_cs_s7);
			ReleaseSemaphore(g_hSemaphore_s7, 1, NULL);
			break;
		}
		LeaveCriticalSection(&g_cs_s7);
		ReleaseSemaphore(g_hSemaphore_s7, 1, NULL);
	}

	return 0;
}

static void s7()
{
	DWORD threadID = 0;
	DWORD threadParam = 0x1;
	HANDLE aThreadHandle[4] = { 0 };

	InitializeCriticalSection(&g_cs_s7);
	g_hSemaphore_s7 = CreateSemaphore(NULL, 0, 2, NULL);

	aThreadHandle[0] = CreateThread(NULL, 0, ThreadProrSemaphore_s7, (LPVOID)0/*NULL*/, 0/*CREATE_SUSPENDED*/, &threadID);
	aThreadHandle[1] = CreateThread(NULL, 0, ThreadProrSemaphore_s7, (LPVOID)1/*NULL*/, 0/*CREATE_SUSPENDED*/, &threadID);
	aThreadHandle[2] = CreateThread(NULL, 0, ThreadProrSemaphore_s7, (LPVOID)2/*NULL*/, 0/*CREATE_SUSPENDED*/, &threadID);
	aThreadHandle[3] = CreateThread(NULL, 0, ThreadProrSemaphore_s7, (LPVOID)3/*NULL*/, 0/*CREATE_SUSPENDED*/, &threadID);


	ReleaseSemaphore(g_hSemaphore_s7, 2, NULL);

	WaitForMultipleObjects(4, aThreadHandle, TRUE, INFINITE);

	printf("ok\n");

	CloseHandle(aThreadHandle[0]);
	CloseHandle(aThreadHandle[1]);
	CloseHandle(aThreadHandle[2]);
	CloseHandle(aThreadHandle[3]);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					8. 生产消费实例

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static HWND hDlg_s8;	// 对话框句柄
static HANDLE g_hSemaphore_Productor_s8;
static HANDLE g_hSemaphore_Consumer_s8;
static CRITICAL_SECTION cs_s8;

static DWORD WINAPI ThreadProc_s8_productor(LPVOID lpParameter)
{
	// 读取资源
	TCHAR buf[1024];
	TCHAR c[2] = { 0 };
	UINT n = 0;
	while (1)
	{
		WaitForSingleObject(g_hSemaphore_Productor_s8, INFINITE);

		n = GetDlgItemText(hDlg_s8, IDC_EDIT_SAFE_RESOURCE, buf, sizeof(buf)/sizeof(TCHAR));
		if (n == 0)
			break; // 没有资源了,退出

		c[0] = buf[0];
		SetDlgItemText(hDlg_s8, IDC_EDIT_SAFE_RESOURCE, buf + 1);

		EnterCriticalSection(&cs_s8);
		// 测试A
		n = GetDlgItemText(hDlg_s8, IDC_EDIT_SAFE_A , buf, sizeof(buf)/sizeof(TCHAR));
		if (n==0)
		{
			SetDlgItemText(hDlg_s8, IDC_EDIT_SAFE_A , c);
		}
		else
		{
			// 测试B
			n = GetDlgItemText(hDlg_s8, IDC_EDIT_SAFE_B, buf, sizeof(buf) / sizeof(TCHAR));
			if (n == 0)
				SetDlgItemText(hDlg_s8, IDC_EDIT_SAFE_B, c);
		}
		LeaveCriticalSection(&cs_s8);
		Sleep(300);
		ReleaseSemaphore(g_hSemaphore_Consumer_s8, 1, NULL);
	}

	return 0;
}

static DWORD WINAPI ThreadProc_s8_consumer(LPVOID lpParameter)
{
	TCHAR buf[1024];
	UINT n = 0;
	TCHAR c[2] = { 0 };
	int eid = 0;
	switch ((UINT)lpParameter)
	{
		case 0:
			eid = IDC_EDIT_SAFE_C1;
		break;
		case 1:
			eid = IDC_EDIT_SAFE_C2;
			break;
		case 2:
			eid = IDC_EDIT_SAFE_C3;
			break;
		case 3:
			eid = IDC_EDIT_SAFE_C4;
			break;
		default:
			return 0;
	}
	

	while (1)
	{
		Sleep(3000);
		WaitForSingleObject(g_hSemaphore_Consumer_s8, INFINITE);
		EnterCriticalSection(&cs_s8);
		memset(c, 0, sizeof(c));
		n = GetDlgItemText(hDlg_s8, IDC_EDIT_SAFE_A , buf, sizeof(buf)/sizeof(TCHAR));
		if (n != 0)
		{
			c[0] = buf[0];
			SetDlgItemText(hDlg_s8, IDC_EDIT_SAFE_A, TEXT(""));
		}
		else
		{
			n = GetDlgItemText(hDlg_s8, IDC_EDIT_SAFE_B , buf, sizeof(buf)/sizeof(TCHAR));
			c[0] = buf[0];
			SetDlgItemText(hDlg_s8, IDC_EDIT_SAFE_B , TEXT(""));
		}

		GetDlgItemText(hDlg_s8, eid , buf, sizeof(buf)/sizeof(TCHAR));
		lstrcat(buf, c);
		SetDlgItemText(hDlg_s8, eid , buf);
		LeaveCriticalSection(&cs_s8);
		ReleaseSemaphore(g_hSemaphore_Productor_s8, 1, NULL);
	}

	return 0;
}

static DWORD WINAPI ThreadProc_s8_begin(LPVOID lpParameter)
{
	//DWORD result;
	DWORD threadID = 0;
	g_hSemaphore_Productor_s8 = CreateSemaphore(NULL, 2, 2, NULL);
	g_hSemaphore_Consumer_s8 = CreateSemaphore(NULL, 0, 2, NULL);
	InitializeCriticalSection(&cs_s8);

	HANDLE hThread[5];

	// 生产者线程
	hThread[0] = CreateThread(NULL, 0, ThreadProc_s8_productor, (LPVOID)0 , 0, &threadID);
	hThread[1] = CreateThread(NULL, 0, ThreadProc_s8_consumer, (LPVOID)0 , 0, &threadID);
	hThread[2] = CreateThread(NULL, 0, ThreadProc_s8_consumer, (LPVOID)1 , 0, &threadID);
	hThread[3] = CreateThread(NULL, 0, ThreadProc_s8_consumer, (LPVOID)2 , 0, &threadID);
	hThread[4] = CreateThread(NULL, 0, ThreadProc_s8_consumer, (LPVOID)3 , 0, &threadID);

	WaitForMultipleObjects(5, hThread, TRUE, INFINITE);

	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);
	CloseHandle(hThread[3]);
	CloseHandle(hThread[4]);

	CloseHandle(g_hSemaphore_Productor_s8);
	CloseHandle(g_hSemaphore_Consumer_s8);
	DeleteCriticalSection(&cs_s8);

	return 0;
}

static LRESULT CALLBACK mdlgProc_s8(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// 消息处理
	switch (msg)
	{
	case WM_INITDIALOG:	
	{
		hDlg_s8 = hwnd;
		SetDlgItemText(hwnd, IDC_EDIT_SAFE_RESOURCE, TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
		break;
	}
	case WM_COMMAND:		
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_SAFE_BEGIN:
		{
			// 生产者线程
			HANDLE hThread = CreateThread(NULL, 0, ThreadProc_s8_begin, (LPVOID)0, 0, NULL);
			CloseHandle(hThread);
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_CLOSE:						// 点右上角关闭按钮时发送此消息.
		EndDialog(hwnd, 0x1111);		// 退出消息循环,使得DialogBox得以返回.
		break;
	default:
		return FALSE;			// 没处理交给默认,返回false
	}
	return TRUE;					// 处理了返回true
}

static void s8()
{
	DialogBoxParam(GetModuleHandle(0), (LPCTSTR)MAKEINTRESOURCEA(IDD_DIALOG_THREAD_SAFE), 0, (DLGPROC)mdlgProc_s8, 0x12345678);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					8. 定时器的手工重置和自动重置

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// Timer的APC是发给创建timer的线程的
VOID CALLBACK TimerAPCProc(PVOID pvArgToCompletionRoutine, DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
	printf("定时器触发了! %016I64X  currentThreadid: %d\n", (UINT64)pvArgToCompletionRoutine, GetCurrentThreadId());
}

static HANDLE hTimer = NULL;

static DWORD WINAPI ThreadProc_s9(LPVOID lpParameter)
{
	DWORD param = (DWORD)(UINT64)lpParameter;
	printf("new thread begin %x \n", GetCurrentThreadId());
	for (int i = 0; i < 3; i++)
	{
		// 1. timer apc 是发送给创建timer的线程的. 所以这个不会返回
		// SleepEx(INFINITE, TRUE);
		// printf("thead : %d  ; APC 唤醒 : %d\n", param,i);

		// 2. timer有信号时可继续执行, 
		WaitForSingleObjectEx(hTimer, INFINITE, TRUE);
		printf("timer signaled %d \n", GetCurrentThreadId());
	}
	return 0;
}

static void s9()
{
	// 创建一个可等待计时器
	hTimer = CreateWaitableTimer(NULL, TRUE/*是否手工重置*/, NULL);
	// 手工重置: 计时器有信号后,等待这个计时器的所有线程都会变为有信号状态.  要让计时器没信号要用SetEvent()自己实现.
	// 自动重置: 计时器有信号后,只有一个等待此计时器的线程可运行,之后计时器会自动重置为没信号状态.
	if (hTimer == NULL)
	{
		printf("CreateWaitableTimer failed (%d)\n", GetLastError());
		return;
	}

	DWORD arg = 0x11111111;

#define ONE_SECOND 10000000LL

	INT64 qwDueTime = -2LL * ONE_SECOND;
	LARGE_INTEGER liDueTime;
	liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
	liDueTime.HighPart = (DWORD)(qwDueTime >> 32);
	// 设置回调函数
	if (!SetWaitableTimer(hTimer, &liDueTime/*延迟时间*/, 2000/*每隔2秒执行一次,为0就只触发一次*/, TimerAPCProc/*异步APC函数*/, &arg, FALSE))
	{
		printf("SetWaitableTimer failed (%d)\n", GetLastError());
		CloseHandle(hTimer);
		return;
	}

	DWORD threadID = 0;
	LPVOID threadParam = (LPVOID)0x111;
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc_s9, threadParam, 0, &threadID);

	for (int i = 0; i < 10; i++)
	{
		// 1. timer有信号时可继续执行,但是不会执行APC,因为普通的WaitForSingleObject()不会唤醒APC.
		// WaitForSingleObject(hTimer, INFINITE);
		// printf("timer signaled \n");


		// 2. timer有信号时可继续执行,会执行APC,  Ex版本会设置UserApcPending.
		//WaitForSingleObjectEx(hTimer, INFINITE, TRUE);
		//printf("timer signaled %d \n",GetCurrentThreadId());


		// 3. 不去管Timer的信号,只等待APC唤醒.
		// SleepEx(INFINITE, TRUE);
		// printf("APC 唤醒 : %d\n", i);
	}

	// 等待定时器触发
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hTimer);
	return;
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void p000_004()
{
	s8();
}
