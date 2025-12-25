#pragma once

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// PE 分析界面

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

struct INFO_POSITION {
	UINT32 offset;
	UINT32 length;
};


class CDialogPE : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogPE)
	DECLARE_MESSAGE_MAP()

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PE_DLG };
#endif


private:
	PCHAR m_pBuffer;					// 缓冲区指针
	UINT32 m_length;					// 缓冲区长度
	UINT32 m_isMemImage;				// 是否内存镜像
	CString m_path;						// 文件路径

	CMenu m_menu;						// 主菜单
	CStatic m_static_path;				// 路径 静态文本框
	CEdit m_edit_path;					// 路径 编辑框
	CTreeCtrl m_tree_header;			// header 树控件
	CListCtrl m_list_section;			// section 列表控件
	CButton m_button_table[16];			// 表格控件数组
	CEdit m_edit_text;					// 文本信息编辑框

	INFO_POSITION m_infoPosition[100];	// 用来存储信息的位置和长度,方便在hex中高亮显示,0-50 用于header,50-80 用于section,80-100 未使用


public:
	CDialogPE(PCHAR pBuffer,UINT32 length,UINT32 isMemImage = 0, CString path = TEXT(""), CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDialogPE();

public:
	int mCreateItems();
	int mInitItems();
	int mAnalyzePEFile();
	int mAnalyzeHeaderInfo();
	int mAnalyzeSectionInfo();
	int mAnalyzeTableInfo();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	void OnButtonClickDataDirectory(UINT id);
	void OnListCtrlSelectMenu(NMHDR* pNMHDR, LRESULT* pResult);
public:
	afx_msg void OnDlgPeMenuTest();
	afx_msg void OnSectionModify();
	int mRefreshPage(char* pNewBuffer = NULL);
	afx_msg void OnSectionAdd();
	afx_msg void OnTestSave();
	afx_msg void OnSectionMerge();
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 宏定义

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#define MARGIN_COMMON 10		// 通用边距
#define ITEM_HEIGHT_COMMON 20	// 控件通用高度定义
#define HALF_WIDTH 630			// 半个长度定义


// 静态 path 控件定义
#define CSTATIC_PATH_ID 1000
#define CSTATIC_PATH_WIDTH 50

// 编辑 path 控件定义
#define CEDIT_PATH_ID 1001
#define CEDIT_PATH_WIDTH 600

// 树 header 控件定义
#define CTREE_HEADER_ID 1002
#define CTREE_HEADER_HEIGHT 200

// 列表 section 控件定义
#define CLIST_SECTION_ID 1003
#define CLIST_SECTION_HEIGHT 200

// 按钮 table 控件定义
#define CBUTTON_TABLE_ID_BEGIN 1004
#define CBUTTON_TABLE_ID_END 1030
#define CBUTTON_TABLE_WIDTH 150
#define CBUTTON_TABLE_HEIGHT 30

// 编辑 text 控件定义
#define CEDIT_TEXT_ID 1031


