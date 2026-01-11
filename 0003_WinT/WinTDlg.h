#pragma once

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 主界面对话框

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class CWinTDlg : public CDialogEx
{
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINT_DIALOG };
#endif

	DECLARE_MESSAGE_MAP()

public:
	HICON m_hIcon;
public:
	CWinTDlg(CWnd* pParent = nullptr);	

protected:
	afx_msg virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonPe();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonWintDlgProcess();
};
