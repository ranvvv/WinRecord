#pragma once

class CMainFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CMainFrame)
	DECLARE_MESSAGE_MAP()

public:
	CStatusBar        m_wndStatusBar;
	
protected: // 仅从序列化创建
	CMainFrame() noexcept;
	virtual ~CMainFrame();

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

};


