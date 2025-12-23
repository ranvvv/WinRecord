#include"04_afx.h"
#include"05_State.h"
#include"06_Thread.h"
#include<process.h>


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CWinThread  : MFC 线程类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
IMPLEMENT_DYNCREATE(CWinThread, CObject)

void CWinThread::CommonConstruct()
{
	m_hThread = NULL;
	m_nThreadID = 0;
	m_bAutoDelete = TRUE;
}

CWinThread::CWinThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam)
{
	m_pfnThreadProc = pfnThreadProc;
	m_pThreadParams = pParam;
	CommonConstruct();
}

CWinThread::CWinThread()
{
	m_pThreadParams = NULL;
	m_pfnThreadProc = NULL;
	CommonConstruct();
}

CWinThread::~CWinThread()
{
	// 释放线程内核对象句柄
	if (m_hThread != NULL)
		CloseHandle(m_hThread);
	// 清除线程的状态
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();
	if (pState->m_pCurrentWinThread == this)
		pState->m_pCurrentWinThread = NULL;
}

void CWinThread::Delete()
{
	// 如果指定了自动清除的话，删除this 指针
	if (m_bAutoDelete)
		delete this;
}

CWinThread::operator HANDLE() const
{
	return this == NULL ? NULL : m_hThread;
}

int CWinThread::GetThreadPriority()
{
	return ::GetThreadPriority(m_hThread);
}

BOOL CWinThread::SetThreadPriority(int nPriority)
{
	return ::SetThreadPriority(m_hThread, nPriority);
}

DWORD CWinThread::SuspendThread()
{
	return ::SuspendThread(m_hThread);
}

DWORD CWinThread::ResumeThread()
{
	return ::ResumeThread(m_hThread);
}

BOOL CWinThread::CreateThread(DWORD dwCreateFlags, UINT nStackSize, LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
	// 为线程的初始化定义变量startup 
	_AFX_THREAD_STARTUP startup;
	memset(&startup, 0, sizeof(startup));
	startup.pThread = this;
	startup.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	startup.hEvent2 = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	// 创建一个初始状态为不可调度的线程（挂起）,  MFC线程的入口函数都是_AfxThreadEntry
	m_hThread = (HANDLE)_beginthreadex(lpSecurityAttrs, nStackSize, &_AfxThreadEntry, &startup, dwCreateFlags | CREATE_SUSPENDED, (UINT*)&m_nThreadID);
	if (m_hThread == NULL)
		return FALSE;
	// 恢复线程的执行
	ResumeThread();
	// 等待线程初始化完毕
	::WaitForSingleObject(startup.hEvent, INFINITE);
	::CloseHandle(startup.hEvent);

	// 如果用户创建的是一个挂起的线程，我们就暂停线程的运行
	if (dwCreateFlags & CREATE_SUSPENDED)
		::SuspendThread(m_hThread);

	// 如果线程在初始化时出错，释放所有资源
	if (startup.bError)
	{
		::WaitForSingleObject(m_hThread, INFINITE);
		::CloseHandle(m_hThread);
		m_hThread = NULL;
		::CloseHandle(startup.hEvent2);
		return FALSE;
	}

	// 通知线程继续运行
	::SetEvent(startup.hEvent2);
	return TRUE;
}


int CWinThread::Run()
{
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;
	for (;;)
	{
		while (bIdle && !::PeekMessage(&m_msgCur, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!OnIdle(lIdleCount++))
				bIdle = FALSE;
		}
		do
		{
			if (!PumpMessage())
				return ExitInstance();
			if (IsIdleMessage(&m_msgCur))
			{
				bIdle = TRUE;
				lIdleCount = 0;
			}
		} while (::PeekMessage(&m_msgCur, NULL, 0, 0, PM_NOREMOVE));
	}
	ASSERT(FALSE);
}

// 在消息送给Windows 的 TranslateMessage 和 DispatchMessage 之前进行消息过滤
BOOL CWinThread::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}

BOOL CWinThread::PumpMessage()
{
	if (!::GetMessage(&m_msgCur, NULL, NULL, NULL))
		return FALSE;
	if (!PreTranslateMessage(&m_msgCur)) // 没有完成翻译
	{
		::TranslateMessage(&m_msgCur);
		::DispatchMessage(&m_msgCur);
	}
	return TRUE;
}

BOOL CWinThread::OnIdle(LONG lCount)
{
	return lCount < 0;
}

BOOL CWinThread::IsIdleMessage(MSG* pMsg)
{
	return TRUE;
}

BOOL CWinThread::InitInstance()
{
	return FALSE;
}

int CWinThread::ExitInstance()
{
	int nResult = (int)m_msgCur.lParam;
	return nResult;
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					创建线程 用到的 全局函数

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// 线程创建接口函数
CWinThread* AfxBeginThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam, int nPriority, UINT nStackSize, DWORD dwCreateFlags, LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
	// 为新线程创建一个CWinThead 类的对象
	CWinThread* pThread = new CWinThread(pfnThreadProc, pParam);

	// 创建线程，并挂起
	if (!pThread->CreateThread(dwCreateFlags | CREATE_SUSPENDED, nStackSize, lpSecurityAttrs))
	{
		pThread->Delete();
		return NULL;
	}
	// 设置新线程的优先级
	pThread->SetThreadPriority(nPriority);

	// 如果没有指定CREATE_SUSPENDED 标记，则唤醒线程
	if (!(dwCreateFlags & CREATE_SUSPENDED))
		pThread->ResumeThread();

	return pThread;
}

// 封装线程入口
UINT __stdcall _AfxThreadEntry(void* pParam)
{
	_AFX_THREAD_STARTUP* pStartup = (_AFX_THREAD_STARTUP*)pParam;
	CWinThread* pThread = pStartup->pThread;
	try
	{ // 设置新线程的状态
		AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();	// 这里保存当前线程指针
		pState->m_pCurrentWinThread = pThread;							// 这里赋值了线程指针
	}
	catch (...)
	{
		// 如果try 块有异常抛出，此处的代码将被执行
		pStartup->bError = TRUE;
		::SetEvent(pStartup->hEvent);
		AfxEndThread((UINT)-1, FALSE);
	}
	// 调用下面的SetEvent 函数后，pStartup 所指向的内存空间就有可能被父线程销毁，
	// 所以要保存hEvent2 的值
	HANDLE hEvent2 = pStartup->hEvent2;
	// 允许父线程从CWinThread::CreateThread 函数返回
	::SetEvent(pStartup->hEvent);
	// 等待父线程中CWinThread::CreateThread 函数的代码执行完毕
	::WaitForSingleObject(hEvent2, INFINITE);
	::CloseHandle(hEvent2);

	// 调用用户指定的线程函数	: ★★★★★
	DWORD nResult = (*pThread->m_pfnThreadProc)(pThread->m_pThreadParams);

	// 结束线程
	AfxEndThread(nResult);
	return 0;
}

CWinThread* AfxGetThread() // 返回当前线程CWinThread 对象的指针
{
	// 取得模块线程状态指针
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();
	return pState->m_pCurrentWinThread;
}

// 封装线程结束
void AfxEndThread(UINT nExitCode, BOOL bDelete) // 结束当前线程
{
	// 释放当前CWinThread 对象占用的内存
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();
	CWinThread* pThread = pState->m_pCurrentWinThread;
	if (pThread != NULL)
	{
		if (bDelete)
			pThread->Delete();
		pState->m_pCurrentWinThread = NULL;
	}
	// 释放线程局部存储占用的内存
	if (_afxThreadData != NULL)
		_afxThreadData->DeleteValues(NULL, FALSE);
	// 结束此线程的运行
	_endthreadex(nExitCode);
}