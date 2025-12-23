#pragma once

#include"04_Afx.h"


typedef UINT(__cdecl* AFX_THREADPROC)(LPVOID);

class CWinThread;
class CWnd;

struct _AFX_THREAD_STARTUP
{
	CWinThread* pThread;					// 控制新线程的CWinThread 对象的指针
	HANDLE hEvent;							// 此事件在线程初始化完毕后将被触发
	HANDLE hEvent2;							// 此事件在线程从CreateThread 函数返回时将被触发
	BOOL bError;							// 指示线程是否初始化成功
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CWinThread  : MFC 线程类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class CWinThread : public CObject
{
public:
	HANDLE m_hThread;						// 线程句柄
	DWORD m_nThreadID;						// 线程ID
	AFX_THREADPROC m_pfnThreadProc;			// 线程函数的地址
	LPVOID m_pThreadParams;					// 用户传递给新线程的参数
	BOOL m_bAutoDelete;						// delete self
	CWnd* m_pMainWnd;						// 线程主窗口
	MSG m_msgCur;							// 当前正在处理的消息

	DECLARE_DYNCREATE(CWinThread)

public:
	CWinThread();
	CWinThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam);
	void CommonConstruct();
	virtual ~CWinThread();

	BOOL CreateThread(DWORD dwCreateFlags = 0, UINT nStackSize = 0, LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);
	operator HANDLE() const;
	int GetThreadPriority();
	BOOL SetThreadPriority(int nPriority);
	DWORD SuspendThread();
	DWORD ResumeThread();
public:
	virtual void Delete();

	// 允许重载的函数（Overridables）
	// 执行线程实例初始化
	virtual BOOL InitInstance();

	// 开始处理消息
	virtual int Run();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PumpMessage();
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL IsIdleMessage(MSG* pMsg);

	// 线程终止时执行清除
	virtual int ExitInstance();

};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					线程 用到的 全局函数

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

UINT __stdcall _AfxThreadEntry(void* pParam);

// 创建线程
CWinThread* AfxBeginThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam,
	int nPriority = THREAD_PRIORITY_NORMAL, UINT nStackSize = 0,
	DWORD dwCreateFlags = 0, LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);

// 获取当前线程
CWinThread* AfxGetThread();

// 结束线程
void AfxEndThread(UINT nExitCode, BOOL bDelete = TRUE);

