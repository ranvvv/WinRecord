#include "pch.h"
#include "framework.h"
#include "Test.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame() noexcept
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序



void CMainFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	//if (nHitTest == HTCAPTION )
	//{
	//	MessageBox(_T("你点击了标题栏"), _T("提示"), MB_OK);
	//}

	//if (nHitTest == HTCLOSE)
	//{
	//	MessageBox(_T("你点击了关闭按钮"), _T("提示"), MB_OK);
	//}

	CFrameWnd::OnNcLButtonDown(nHitTest, point);
}
