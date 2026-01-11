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

// CTestView 消息处理程序
static int b_inbutton = 0;


IMPLEMENT_DYNCREATE(CTestView, CView)

BEGIN_MESSAGE_MAP(CTestView, CView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

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


CTestView::CTestView() noexcept
{

}

CTestView::~CTestView()
{

}


BOOL CTestView::PreCreateWindow(CREATESTRUCT& cs)
{

	return CView::PreCreateWindow(cs);
}




int CTestView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;

	BOOL bRet = FALSE;
	DWORD dwStyle = 0;
	LONG lStyle = 0;
	int left = 0, top = 0, right = 0, bottom = 0;

	// 创建 CStatic 控件 
	top = 10;
	left = 10;
	right = left + 100;
	bottom = top + 20;
	dwStyle = WS_CHILD | WS_VISIBLE;
	bRet = m_static.Create(_T("Static"), dwStyle, CRect(left, top, right, bottom), this, ID_STATIC);
	if (!bRet)
		return -1;
	m_static.SetWindowText(_T("Static"));

	// 创建 CEdit 控件
	top = 30;
	left = 10;
	right = left + 100;
	bottom = top + 20;
	dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY;
	bRet = m_edit.Create(dwStyle, CRect(left, top, right, bottom), this, ID_EDIT);
	if (!bRet)
		return -1;
	m_edit.SetWindowText(_T("Edit"));

	// 创建 CTreeCtrl 控件 
	top = 60;
	left = 10 ;
	right = left + 200;
	bottom = top + 200;
	dwStyle = WS_VISIBLE | WS_TABSTOP | WS_CHILD | WS_BORDER | WS_VSCROLL | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES | TVS_DISABLEDRAGDROP | TVS_NOTOOLTIPS;
	bRet = m_tree.Create(dwStyle, CRect(left, top, right, bottom), this, ID_TREE);
	if (!bRet)
		return -1;
	HTREEITEM hRoot = NULL;                 // 根节点
	HTREEITEM hChild = NULL;
	hRoot = m_tree.InsertItem(_T("Root"));
	hChild = m_tree.InsertItem(TEXT("child1"), hRoot);
	m_tree.SetItemData(hChild, 1);
	hChild = m_tree.InsertItem(TEXT("child2"), hRoot);
	m_tree.SetItemData(hChild, 2);
	// DWORD_PTR data = m_tree.GetItemData(hChild);
	// m_tree.DeleteAllItems();


	// 创建 CListCtrl 控件 section
	top = bottom + 10;
	left = 10;
	right = left + 200;
	bottom = top + 200;
	dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_VSCROLL;
	bRet = m_list.Create(dwStyle, CRect(left, top, right, bottom), this, ID_LIST );
	if (!bRet)
		return -1;
	// 扩展样式
	lStyle = GetWindowLong(m_list.m_hWnd, GWL_STYLE);		// 获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; 										// 清除显示方式位
	lStyle |= LVS_REPORT; 											// 设置style
	lStyle |= LVS_SINGLESEL;										// 单选模式
	SetWindowLong(m_list.m_hWnd, GWL_STYLE, lStyle);		// 设置style
	// 扩展样式
	dwStyle = m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	//dwStyle |= LVS_EX_CHECKBOXES;
	m_list.SetExtendedStyle(dwStyle);
	// 设置列头
	m_list.InsertColumn(0, _T("索引"), LVCFMT_LEFT, 40);	// 列1
	m_list.InsertColumn(1, _T("名称"), LVCFMT_LEFT, 58);	// 列2
	int n;
	// 行1
	n = m_list.InsertItem(0,TEXT("List0"));
	m_list.SetItemText(n, 1, TEXT("List0"));
	// 行2
	n = m_list.InsertItem(1,TEXT("List1"));
	m_list.SetItemText(n, 1, TEXT("List1"));

	// 按钮
	top = bottom + 10;
	left = 10;
	right = left + 200;
	bottom = top + 20;
	dwStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;
	bRet = m_button.Create(_T("Button"), dwStyle, CRect(left, top, right, bottom), this, ID_BUTTON);
	if (!bRet)
		return -1;
	m_button.SetWindowText(_T("Button"));

	// 滚动条
	top = 10;
	left = 230;
	right = left + 20;
	bottom = top + 200;
	dwStyle = WS_CHILD | WS_VISIBLE | SBS_VERT;
	bRet = m_scrollbar.Create(dwStyle, CRect(left, top, right, bottom), this, ID_SCROLLBAR);
	if (!bRet)
		return -1;
	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(si);
	si.nMin = 0;
	si.nMax = 100;
	si.nPage = 10;
	si.nPos = 30;
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
	m_scrollbar.SetScrollInfo(&si, TRUE);

	return 0;
}

void CTestView::OnDraw(CDC* pDC)
{
	CTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}





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
	mDoubleBuffering();

	return;

	if (nFlags & MK_SHIFT)
	{
		MessageBox(L"Shift键被按下！");
	}

	if(CRect(500, 100, 600, 200).PtInRect(point))
	{
		MessageBox(L"恭喜你，按到了按钮！");
	}

}

void CTestView::OnMouseMove(UINT nFlags, CPoint point)
{
	return;
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

// 滚动条事件
void CTestView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.fMask = SIF_ALL;
	pScrollBar->GetScrollInfo(&si);

	int id = pScrollBar->GetDlgCtrlID();

	int pos = 0;

	if (id == ID_SCROLLBAR)
	{
		switch (nSBCode)
		{
		case SB_TOP:
			pos = 0;
			break;
		case SB_BOTTOM:
			pos = 100;
			break;
		case SB_LINEUP:
			pos = si.nPos - 1;
			break;
		case SB_LINEDOWN:
			pos = si.nPos + 1;
			break;
		case SB_PAGEUP:
			pos = si.nPos - si.nPage;
			break;
		case SB_PAGEDOWN:
			pos = si.nPos + si.nPage;
			break;
		case SB_THUMBPOSITION:
			pos = si.nTrackPos;
			break;
		case SB_THUMBTRACK:
			pos = si.nTrackPos;
			break;
		default:
			return;
		}
	}
	
	m_scrollbar.SetScrollPos(pos);

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

// 双缓冲绘图
void CTestView::mDoubleBuffering()
{
	// ===== 双缓冲核心代码开始 =====
	CClientDC dc(this); // 前台DC（关联窗口）

	// 1. 获取窗口客户区大小，用于创建兼容位图
	CRect clientRect;
	GetClientRect(&clientRect);

	// 2. 创建内存DC（后台DC）
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);// 创建与前台DC兼容的内存DC

	// 3. 创建与前台DC兼容的位图（内存画布），大小为客户区大小
	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc, clientRect.Width(), clientRect.Height());

	// 4. 将位图选入内存DC（给内存DC绑定画布）
	CBitmap* pOldBmp = memDC.SelectObject(&memBmp);

	// 5. 清空内存DC背景（可选，避免脏数据）
	memDC.FillSolidRect(clientRect, RGB(0, 255, 255)); // 白色背景

	// 6. 在内存DC上执行所有绘图操作（这一步不会闪烁）
	memDC.SetTextColor(RGB(255, 0, 0)); // 红色文字
	memDC.TextOut(50, 50, _T("双缓冲绘制的文字，无闪烁"));
	memDC.Rectangle(50, 80, 250, 180); // 绘制矩形
	memDC.Ellipse(300, 80, 400, 180);  // 绘制椭圆

	// 7. 将内存DC的内容一次性复制到前台DC（关键：仅这一步刷新屏幕）
	dc.BitBlt(0, 0, clientRect.Width(), clientRect.Height(),&memDC, 0, 0, SRCCOPY);

	// 8. 释放资源（必须还原DC的原始位图，否则会内存泄漏）
	memDC.SelectObject(pOldBmp);
	memBmp.DeleteObject();
	memDC.DeleteDC();
	// ===== 双缓冲核心代码结束 =====
}

// 重绘背景事件
BOOL CTestView::OnEraseBkgnd(CDC* pDC)
{

	return TRUE; // 不重绘背景，否则会闪烁

	return CView::OnEraseBkgnd(pDC); // 重绘
}
