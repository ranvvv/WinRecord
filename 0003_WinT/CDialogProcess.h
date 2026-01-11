#pragma once

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 进程管理对话框

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class CDialogProcess : public CDialogEx
{
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESS_DLG };
#endif

	DECLARE_DYNAMIC(CDialogProcess)
	DECLARE_MESSAGE_MAP()

	CStatic m_static_process;			// 进程名称控件
	CListCtrl m_list_process;			// 进程列表控件
	CStatic m_static_module;			// 模块名称控件
	CListCtrl m_list_module;			// 模块列表控件

public:
	CDialogProcess(CWnd* pParent = nullptr);   
	virtual ~CDialogProcess();

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkListModule(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListCtrlProcessSelectMenu(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListCtrlProcessSelectChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDlgMenuInject();

private:
	int mCreateItems();
	int mInitItems();
	void mListProcess();
	void mListModule(DWORD pid);
	int mRemoteThreadInject(PCHAR path, DWORD pid);
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 宏定义

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#define CSTATIC_PROCESS_ID 10001
#define CLIST_PROCESS_ID 10002
#define CSTATIC_MODULE_ID 10003
#define CLIST_MODULE_ID 10004

#define V_MARGIN 10
#define NAME_HEIGHT 20
#define NAME_WIDTH 100
#define LIST_HEIGHT 200
#define LIST_WIDTH 900
