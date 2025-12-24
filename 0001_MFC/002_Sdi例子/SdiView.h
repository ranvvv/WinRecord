#pragma once


class CSdiView : public CView
{
	DECLARE_DYNCREATE(CSdiView)
	DECLARE_MESSAGE_MAP()

protected: // 仅从序列化创建
	CSdiView() noexcept;

// 特性
public:
	virtual ~CSdiView();

	CSdiDoc* GetDocument() const;
// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

};


