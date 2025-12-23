#if 0
#include<afxwin.h>
#include"resource.h"
// 必须有一个窗口标题的字符串资源   AFX_IDS_UNTITLED    否则异常.

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
    pDC->TextOut(100, 100, "视图窗口");
}

class CMyFrame : public CFrameWnd
{
    DECLARE_DYNCREATE(CMyFrame)
};
IMPLEMENT_DYNCREATE(CMyFrame, CFrameWnd)

class CMyWinApp :public CWinApp
{
public:
    virtual BOOL InitInstance();
};

BOOL CMyWinApp::InitInstance()
{
    // SetThreadLocale(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));

    // 1. 先弄个单文档模板类
    CSingleDocTemplate* pTemplate = new CSingleDocTemplate(IDS_TITLE, RUNTIME_CLASS(CMyDoc), RUNTIME_CLASS(CMyFrame), RUNTIME_CLASS(CMyView));
    // 对于IDS_TITLE 资源,要有对应menu和string资源,否则异常.
    // 2. 将文档模板添加到 文档管理器的链表里.
    AddDocTemplate(pTemplate);

    // 3 : 创建框架实例

    // 3.  实现方式1 : 创建文档实例,框架实例,然后调用LoadFrame() 实现整体的创建和连接
    // OnFileNew();

    // 3.  实现方式2 : AfxGetApp()->OnCmdMsg(ID_FILE_NEW, 0, NULL, NULL)
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    return TRUE;

}

CMyWinApp theApp;
#endif


/*
// 从动态库MFC 转 静态MFC的注意事项:
    1. 在工程属性->配置属性->C/C++->代码生成->运行库: MTD
    2. 解决: new  / delete 冲定义在LIBCMTD.LIB 的问题
        连接器>输入->附加依赖项: uafxcwd.lib;libcmtd.lib
        连接器>输入->忽略特定库: uafxcwd.lib;libcmtd.lib
    3. MFC预定义字符串在资源文件中的值要和他定义的一致,否则LoadString() 搞不到就报错.
*/

