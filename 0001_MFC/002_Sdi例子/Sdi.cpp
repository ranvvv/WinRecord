#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Sdi.h"
#include "MainFrm.h"

#include "SdiDoc.h"
#include "SdiView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSdiApp theApp;		// 应用程序实例

BEGIN_MESSAGE_MAP(CSdiApp, CWinApp)
	//ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, &CSdiApp::OnFileNew)
END_MESSAGE_MAP()

// 1. 最先执行, 之后WinMain才会执行
CSdiApp::CSdiApp() noexcept
{

}

// 2. WinMain>>AfxWinMain>>CSdiApp::InitInstance     进行初始化
BOOL CSdiApp::InitInstance()
{
	CWinApp::InitInstance();

	// 单文档模板
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSdiDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CSdiView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);		// 将模板添加到模板管理器中  theApp.m_pDocManager 就是模板管理器

	// 命令行参数分析
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 根据命令行处理, 有文件路径就调用OpenDocumentFile打开文档,
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}


// 接管文件新建命令
void CSdiApp::OnFileNew()
{
	// AfxMessageBox(_T("OnFileNew"));
	CWinApp::OnFileNew();
}
