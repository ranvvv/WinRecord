#if 0
#include<afxwin.h>
#include<afxext.h>
#include"resource.h"

class CMyDoc : public CDocument
{
    DECLARE_DYNCREATE(CMyDoc)
};
IMPLEMENT_DYNCREATE(CMyDoc, CDocument)


class CMyView : public CView
{
    DECLARE_DYNCREATE(CMyView)
public:
    virtual void OnDraw(CDC* pDC);
};
IMPLEMENT_DYNCREATE(CMyView, CView)

void CMyView::OnDraw(CDC* pDC)
{
    pDC->TextOutW(100, 100, L"视图窗口");
}

// 子框架
class CMyChild : public CMDIChildWnd
{
    DECLARE_DYNCREATE(CMyChild)
};
IMPLEMENT_DYNCREATE(CMyChild, CMDIChildWnd)

// 主框架,这个不需要动态创建
class CMyFrame : public CMDIFrameWnd
{
};


class CMyWinApp :public CWinApp
{
public:
    virtual BOOL InitInstance();
};

BOOL CMyWinApp::InitInstance()
{
    // 1. 创建主框架窗口
    CMyFrame* pFrame = new CMyFrame;
    pFrame->LoadFrame(IDR_MENU1);

    // 2. 显示主框架窗口
    m_pMainWnd = pFrame;
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    // 3. 注册模板
    CMultiDocTemplate* pTemplate = new CMultiDocTemplate(IDR_MENU2, RUNTIME_CLASS(CMyDoc), RUNTIME_CLASS(CMyChild), RUNTIME_CLASS(CMyView));
    AddDocTemplate(pTemplate);

    // 4. 创建子框架
    OnFileNew(); // 每调一个就创建一个子框架窗口
    OnFileNew();
    OnFileNew();
    return TRUE;
}
CMyWinApp theApp;

#endif

/*
    多文档注意事项:
    Menu必须有子项,有2个
*/