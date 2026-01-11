#pragma once

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 输入对话框

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


class CDialogInput : public CDialogEx
{
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUT_DLG };
#endif

	DECLARE_DYNAMIC(CDialogInput)
	DECLARE_MESSAGE_MAP()


private:
	CString m_title;		// 标题
	CString& m_str;			// 输入内容饮用

public:
	CDialogInput(CString title,CString& str,CWnd* pParent = nullptr);  
	virtual ~CDialogInput();

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedInputDlgOk();
};
