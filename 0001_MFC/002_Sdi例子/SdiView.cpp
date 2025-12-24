#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Sdi.h"
#endif

#include "SdiDoc.h"
#include "SdiView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



IMPLEMENT_DYNCREATE(CSdiView, CView)

BEGIN_MESSAGE_MAP(CSdiView, CView)
END_MESSAGE_MAP()


CSdiView::CSdiView() noexcept
{

}

CSdiView::~CSdiView()
{
}

BOOL CSdiView::PreCreateWindow(CREATESTRUCT& cs)
{

	return CView::PreCreateWindow(cs);
}


void CSdiView::OnDraw(CDC* /*pDC*/)
{
	CSdiDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}


// CSdiView 诊断

#ifdef _DEBUG

CSdiDoc* CSdiView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSdiDoc)));
	return (CSdiDoc*)m_pDocument;
}
#endif //_DEBUG

