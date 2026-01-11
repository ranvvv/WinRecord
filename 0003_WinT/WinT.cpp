#include "pch.h"
#include "framework.h"
#include "WinT.h"
#include "WinTDlg.h"

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 应用程序类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CWinTApp theApp;		// 唯一的应用程序对象


BEGIN_MESSAGE_MAP(CWinTApp, CWinApp)

END_MESSAGE_MAP()


CWinTApp::CWinTApp()
{
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 事件处理

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

BOOL CWinTApp::InitInstance()
{
	CWinApp::InitInstance();

	// 创建 shell 管理器，以防对话框包含 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题, 让MFC窗口外观与当前系统主题一致
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 主界面Dialog
	CWinTDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();		// 阻塞: 消息循环.


	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
		delete pShellManager;

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	return FALSE;	// 返回FALSE,防止进入主消息循环,退出进程.
}

