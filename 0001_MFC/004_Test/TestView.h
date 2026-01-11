#pragma once


class CTestView : public CView
{
	CStatic m_static;
	CEdit m_edit;
	CTreeCtrl m_tree;
	CListCtrl m_list;
	CButton m_button;
	CScrollBar m_scrollbar;

protected: // 仅从序列化创建
	DECLARE_DYNCREATE(CTestView)
	DECLARE_MESSAGE_MAP()

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CTestView() noexcept;
	virtual ~CTestView();

	CTestDoc* GetDocument() const;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void mDoubleBuffering();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // TestView.cpp 中的调试版本
inline CTestDoc* CTestView::GetDocument() const
   { return reinterpret_cast<CTestDoc*>(m_pDocument); }
#endif


#define ID_STATIC 1000
#define ID_EDIT 1001
#define ID_TREE 1002
#define ID_LIST 1003
#define ID_BUTTON 1004
#define ID_SCROLLBAR 1005
