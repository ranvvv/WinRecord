#if 0
#include<windows.h>
#include<stdio.h>
#include<process.h>
#include"03_MFCTLS.h"


struct CMyThreadData : public CNoTrackObject
{
	int nSomeData;
};

// 定义一个线程局部存储的变量，类型为CMyThreadData,每个线程都有自己的拷贝。
THREAD_LOCAL(CMyThreadData, g_myThreadData)

void ShowData()
{
	int nData = g_myThreadData->nSomeData;
	printf(" Thread ID: %-5d, nSomeData = %d \n", ::GetCurrentThreadId(), nData);
	// g_myThreadData->nSomeData;

}

UINT __stdcall ThreadFunc(LPVOID lpParam)
{
	g_myThreadData->nSomeData = (int)lpParam;

	ShowData();

	return 0;
}

int main()
{
	HANDLE h[10];
	UINT uID;
	// 启动十个线程，将i 做为线程函数的参数传过去
	for (int i = 0; i < 10; i++)
		h[i] = (HANDLE) ::_beginthreadex(NULL, 0, ThreadFunc, (void*)i, 0, &uID);
	::WaitForMultipleObjects(10, h, TRUE, INFINITE);
	for (int i = 0; i < 10; i++)
		::CloseHandle(h[i]);
}

#endif 
