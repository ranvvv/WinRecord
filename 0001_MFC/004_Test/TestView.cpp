
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
	ON_WM_MOUSEMOVE()
	ON_WM_NCLBUTTONDOWN()
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
static int b_inbutton = 0;

void CTestView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// 设置线条颜色和宽度
	CPen pen(PS_SOLID, 2, RGB(0, 255, 0));
	CPen* oldPen;
	oldPen = dc.SelectObject(&pen);


	// 画线
	dc.MoveTo(10, 10);
	dc.LineTo(10, 30);
	dc.LineTo(30, 50);

	// 点连接
	POINT pt[5] =  {
		{40,40},
		{100,40},
		 {100,100},
		 {40,100},
		{40,40}
	};
	dc.Polygon(pt, 5);

	// 椭圆
	dc.Ellipse(100, 100, 200, 200);

	// 画矩形
	dc.Rectangle(200, 100, 300, 200);

	// 填充矩形
	dc.FillRect(&CRect(300, 100, 400, 200), &CBrush(RGB(255, 0, 0)));

	// 画3D矩形边框
	CRect rect(500, 100, 600, 150);
	if (b_inbutton)
		dc.FillRect(&rect, &CBrush(RGB(0, 255, 0)));
	else
		dc.FillRect(&rect, &CBrush(RGB(255, 0, 0)));
	dc.Draw3dRect(&rect, RGB(255, 255, 255), RGB(128, 128, 128)); // 左上颜色, 右下颜色  分开设置 ,形成类似3D效果
	dc.SetBkMode(TRANSPARENT);	// 透明背景
	dc.DrawText(L"Hello, MFC!", &rect, DT_CENTER | DT_VCENTER);// 给按钮画文字
	dc.TextOut(500, 120, L"Hello, MFC!");	// 画文字

	
	dc.SelectObject(oldPen);	// 恢复原来的画笔
}


void CTestView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (nFlags & MK_SHIFT)
	{
		MessageBox(L"Shift键被按下！");
	}

	if(CRect(500, 100, 600, 200).PtInRect(point))
	{
		MessageBox(L"恭喜你，按到了按钮！");
	}

}

// 鼠标进入按钮切换颜色的实现.
void CTestView::OnMouseMove(UINT nFlags, CPoint point)
{
	int b_inbutton_pre = b_inbutton;
	if (CRect(500, 100, 600, 150).PtInRect(point))
		b_inbutton = 1;
	else
		b_inbutton = 0;

	if (b_inbutton_pre != b_inbutton)
		Invalidate();

	CView::OnMouseMove(nFlags, point);
}


void CTestView::OnNcLButtonDown(UINT nHitTest, CPoint point)
{

	CView::OnNcLButtonDown(nHitTest, point);
}
