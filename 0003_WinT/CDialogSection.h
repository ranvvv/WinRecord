#pragma once

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 节操作对话框类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class CDialogSection : public CDialogEx
{
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SECTION_DLG };
#endif

	DECLARE_DYNAMIC(CDialogSection)
	DECLARE_MESSAGE_MAP()

private:
	CString& m_name;		// 节名称
	CString& m_size;		// 节大小
	CString& m_charac;		// 节属性
	BOOL m_isModify;		// 1:修改 0:新建

public:
	CDialogSection(BOOL isModify,CString &name,CString& size,CString& charac,CWnd* pParent = nullptr);   
	virtual ~CDialogSection();

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSectionDlgOk();
};
