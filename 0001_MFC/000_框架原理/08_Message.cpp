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


class CMyWnd : public CWnd // 示例代码
{
public:
	CMyWnd();
	void OnCreate() { /* 响应WM_CREAT 消息的代码*/ }
	void OnPaint() { /* 响应WM_PAINT 消息的代码*/ }
	void OnDestory() { /* 响应WM_DESTROY 消息的代码*/ }
	// 定义消息映射的代码
private:
	static const AFX_MSGMAP_ENTRY _messageEntries[];
protected:
	static const AFX_MSGMAP messageMap;
	virtual const AFX_MSGMAP* GetMessageMap() const;
};

// 实现消息映射的代码
const AFX_MSGMAP* CMyWnd::GetMessageMap() const
{
	return &CMyWnd::messageMap;
}

const AFX_MSGMAP CMyWnd::messageMap = { NULL/*&CCmdTarget::messageMap// 这里记录基类消息映射表表,*/, &CMyWnd::_messageEntries[0] };

const AFX_MSGMAP_ENTRY CMyWnd::_messageEntries[] =
{
	{WM_CREATE, 0, 0, 0, 0, (AFX_PMSG)&CMyWnd::OnCreate},
	{WM_PAINT, 0, 0, 0, 0, (AFX_PMSG)&CMyWnd::OnPaint},
	{WM_DESTROY, 0, 0, 0, 0, (AFX_PMSG)&CMyWnd::OnDestory}
	// 在这里添加你要处理的消息

};

CMyWnd::CMyWnd()
{
	LPCTSTR lpszClassName = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, ::LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_3DFACE + 1), ::LoadIcon(NULL, IDI_APPLICATION));
	CreateEx(WS_EX_CLIENTEDGE, lpszClassName, TEXT("框架程序创建的窗口"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL);
}

BOOL CMyApp::InitInstance()
{
	m_pMainWnd = new CMyWnd;
	::ShowWindow(*m_pMainWnd, this->m_nCmdShow);
	::UpdateWindow(*m_pMainWnd);
	return TRUE; // 返回TRUE 进入消息循环
}
#endif
