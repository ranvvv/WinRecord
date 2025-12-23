#pragma once

#include"04_afx.h"
#include"05_State.h"
#include"08_CWinApp.h"


IMPLEMENT_DYNCREATE(CWinApp, CWinThread)

CWinApp::CWinApp()
{
	// 初始化CWinThread 状态
	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
	AFX_MODULE_THREAD_STATE* pThreadState = pModuleState->m_thread;
	ASSERT(AfxGetThread() == NULL);
	pThreadState->m_pCurrentWinThread = this;
	ASSERT(AfxGetThread() == this);
	m_hThread = ::GetCurrentThread();
	m_nThreadID = ::GetCurrentThreadId();

	// 初始化CWinApp 状态
	ASSERT(pModuleState->m_pCurrentWinApp == NULL);
	pModuleState->m_pCurrentWinApp = this;
	ASSERT(AfxGetApp() == this);

	// 直到进入WinMain 函数之后再设置为运行状态
	m_hInstance = NULL;
}

CWinApp::~CWinApp()
{
	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
	if (pModuleState->m_pCurrentWinApp == this)
		pModuleState->m_pCurrentWinApp = NULL;
}

BOOL CWinApp::InitApplication()
{
	return TRUE;
}

BOOL CWinApp::InitInstance()
{
	return TRUE;
}

int CWinApp::Run()
{
	return CWinThread::Run();
}

int CWinApp::ExitInstance()
{
	return (int)m_msgCur.wParam;
}

