#if 01
// #define _WIN32_WINNT_MAXVER
#include <afxwin.h>     // MFC核心组件

// ***********************    程序主窗口    ***************************** 

// 程序主窗口对象  
class CMyFrame :public CFrameWnd                // 框架窗口继承自CFrameWnd
{
public:
    CMyFrame();
};

CMyFrame::CMyFrame()
{
    Create(NULL, TEXT("abc"));                  // 构造函数中 创建窗口
}

// ***********************    应用程序实例    ***************************** 

class CMyApp : public CWinApp      // 应用程序实例,必须继承自CWinApp,程序的初始化工作是在CWinApp的构造中进行的.
{
private:
    virtual BOOL InitInstance();
public:
    CMyApp();
};

CMyApp::CMyApp()                   // 构造
{
}

BOOL CMyApp::InitInstance()		// 由WinMain调用InitInstance()进行程序的初始化工作.
{
    m_pMainWnd = new CMyFrame;                  // 设置主窗口,这里使用构造里Create的方式自己创建了主窗口.
                                                // 实际MFC框架通过LoadFrame创建主框架窗口.
    m_pMainWnd->ShowWindow(m_nCmdShow);
    m_pMainWnd->UpdateWindow();
    return TRUE;                            	// 进入消息循环
}

CMyApp theApp;                              	// 应用程序实例,一个进程只能有一个实例
                                                // 全局变量优先于WinMain执行.所以WinMain执行时theApp已经实例化,可以使用了.
#endif