#if 0
#include"05_State.h"
#include"08_CWinApp.h"
#include"09_CWindows.h"

class CMyApp : public CWinApp
{

public:
	virtual BOOL InitInstance();
};

// ★★★★★★  由于文件顺序的关系,这个测试无法成功,把这段代码放到_afxwinmain.cpp里测试就可以成功
				// 这个测试需要用#include"7_window.bak" 把本文件放到链接靠后的cpp中
CMyApp theApp;	// AFX_MODULE_STATE _afxBaseModuleState;	在这个之后初始化,就会造成 构造函数中设置的CWinThread指针被覆盖成0  异常

// 实现窗口类
class CMyWnd : public CWnd
{
public:
	CMyWnd();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


BOOL CMyApp::InitInstance()
{
	m_pMainWnd = new CMyWnd;
	::ShowWindow(*m_pMainWnd, this->m_nCmdShow);
	::UpdateWindow(*m_pMainWnd);
	return TRUE; // 返回TRUE 进入消息循环
}


CMyWnd::CMyWnd()
{
	LPCTSTR lpszClassName = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, ::LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_3DFACE + 1), ::LoadIcon(NULL, IDI_APPLICATION));
	CreateEx(WS_EX_CLIENTEDGE, lpszClassName, TEXT("框架程序创建的窗口"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL);
}

LRESULT CMyWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCDESTROY)
	{
		::PostQuitMessage(0);
		delete this;
		return 0; // CMyWnd 对象已经不存在了，必须在这里返回，不能再访问任何非静态成员了
	}
	return Default();
}

#endif
