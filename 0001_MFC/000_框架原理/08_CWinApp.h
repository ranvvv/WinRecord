#pragma once
#include"04_afx.h"
#include"05_State.h"
#include"06_Thread.h"


class CWinApp : public CWinThread 
{
	DECLARE_DYNCREATE(CWinApp)
public:
	CWinApp();
	virtual ~CWinApp();
	// 属性
	 // WinMain 函数的四个参数
	HINSTANCE m_hInstance;
	HINSTANCE m_hPrevInstance;
	LPTSTR m_lpCmdLine;
	int m_nCmdShow;
	CWnd* m_pMainWnd;
	// 帮助操作，通常在InitInstance 函数中进行
public:
	HCURSOR LoadCursor(UINT nIDResource) const;
	HICON LoadIcon(UINT nIDResource) const;
	// 虚函数
public:
	virtual BOOL InitApplication();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
};

__inline HCURSOR CWinApp::LoadCursor(UINT nIDResource) const
{
	return::LoadCursor(AfxGetModuleState()->m_hCurrentResourceHandle, (LPCTSTR)nIDResource);
}

__inline HICON CWinApp::LoadIcon(UINT nIDResource) const
{
	return::LoadIcon(AfxGetModuleState()->m_hCurrentResourceHandle, (LPCTSTR)nIDResource);
}

CWinApp* AfxGetApp();

__inline CWinApp* AfxGetApp()
{
	return AfxGetModuleState()->m_pCurrentWinApp;
}

// 待会儿再讲这个函数，先将它声明在这里。它负责类库框架的内部初始化
BOOL AfxWinInit(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);

