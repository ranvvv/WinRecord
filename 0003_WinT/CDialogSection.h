#pragma once


// CDialogSection 对话框

class CDialogSection : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogSection)
	CString& m_name;
	CString& m_size;
	CString& m_charac;
	BOOL m_isModify;

public:
	CDialogSection(BOOL isModify,CString &name,CString& size,CString& charac,CWnd* pParent = nullptr);   
	virtual ~CDialogSection();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SECTION_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSectionDlgOk();
};
