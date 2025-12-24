
// TestView.cpp: CTestView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Test.h"
#endif

#include "TestDoc.h"
#include "TestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestView

IMPLEMENT_DYNCREATE(CTestView, CView)

BEGIN_MESSAGE_MAP(CTestView, CView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CTestView 构造/析构

CTestView::CTestView() noexcept
{
	// TODO: 在此处添加构造代码

}

CTestView::~CTestView()
{
}

BOOL CTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CTestView 绘图

void CTestView::OnDraw(CDC* /*pDC*/)
{
	CTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CTestView 诊断

#ifdef _DEBUG
void CTestView::AssertValid() const
{
	CView::AssertValid();
}

void CTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTestDoc* CTestView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestDoc)));
	return (CTestDoc*)m_pDocument;
}
#endif //_DEBUG


// CTestView 消息处理程序


void CTestView::OnPaint()
{
	// win32 API 实现绘图
#if 0
	PAINTSTRUCT ps;
	HDC hdc = ::GetDC(m_hWnd);
	::BeginPaint(m_hWnd, &ps);
	::DrawText(hdc, _T("Hello world!"), -1, CRect(10, 10, 200, 50), DT_SINGLELINE);
	::EndPaint(m_hWnd, &ps);
#endif

	// MFC 实现绘图
	CPaintDC dc(this); 
	dc.SetBkColor(RGB(255, 0, 0));	// 设置dc的背景色
	dc.DrawText(_T("Hello world!"),CRect(100, 100, 200, 200), DT_SINGLELINE);
}


void CTestView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CView::OnLButtonDown(nFlags, point);

	// win32 API 实现绘图
#if 0
	HDC hdc = ::GetDC(m_hWnd);
	::Ellipse(hdc, point.x - 10, point.y - 10, point.x + 10, point.y + 10);
	::ReleaseDC(m_hWnd, hdc);
#endif

	//CClientDC dc(this);	// 当前窗口的DC
	CWindowDC dc(NULL);	// 可以画在任意窗口上,包括桌面
	dc.Ellipse(point.x - 10, point.y - 10, point.x + 10, point.y + 10);


}
