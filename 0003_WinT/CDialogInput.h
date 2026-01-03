#pragma once


class CDialogInput : public CDialogEx
{
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUT_DLG };
#endif

	DECLARE_DYNAMIC(CDialogInput)
	DECLARE_MESSAGE_MAP()


private:
	CString m_title;
	CString& m_str;

public:
	CDialogInput(CString title,CString& str,CWnd* pParent = nullptr);  
	virtual ~CDialogInput();

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedInputDlgOk();
};
