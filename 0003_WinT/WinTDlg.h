#pragma once

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 主界面

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class CWinTDlg : public CDialogEx
{
	DECLARE_MESSAGE_MAP()

public:
	HICON m_hIcon;
public:
	CWinTDlg(CWnd* pParent = nullptr);	// 标准构造函数

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINT_DIALOG };
#endif

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonPe();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
