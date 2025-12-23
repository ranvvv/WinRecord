#include<tchar.h>

#include"08_CWinApp.h"
#include"09_CWindows.h"

// #include"7_window.cpp"

/*
	MFC程序的的执行流程:

	AFX_MODULE_STATE _afxBaseModuleState;	实例化
	CMyApp theApp;							实例化
			_afxBaseModuleState->m_pCurrentWinApp = &theApp;
			_afxBaseModuleState->m_thread->m_pCurrentWinThread = &theApp;
	WinMain:
	AfxWinInit:
	theApp->InitApplication()
	theApp->InitInstance()
	theApp->Run(); // 消息循环
	程序结束
*/

int __stdcall _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	ASSERT(hPrevInstance == NULL);
	int nReturnCode = -1;
	CWinThread* pThread = AfxGetThread();
	ASSERT(pThread);
	CWinApp* pApp = AfxGetApp();
	// 类库框架内部的初始化
	if (!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
		goto InitFailure;
	// 应用程序的全局初始化
	if (pApp != NULL && !pApp->InitApplication())
		goto InitFailure;
	// 主线程的初始化
	if (!pThread->InitInstance())
	{
		nReturnCode = pThread->ExitInstance();
		goto InitFailure;
	}
	// 开始与用户交互
	nReturnCode = pThread->Run();

InitFailure:
	return nReturnCode;
}


BOOL AfxWinInit(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	ASSERT(hPrevInstance == NULL);

	// 设置实例句柄
	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
	pModuleState->m_hCurrentInstanceHandle = hInstance;
	pModuleState->m_hCurrentResourceHandle = hInstance;

	// 为这个应用程序填写初始化状态
	CWinApp* pApp = AfxGetApp();
	if (pApp != NULL)
	{
		pApp->m_hInstance = hInstance;
		pApp->m_hPrevInstance = hInstance;
		pApp->m_lpCmdLine = lpCmdLine;
		pApp->m_nCmdShow = nCmdShow;
	}
	return TRUE;
}



















