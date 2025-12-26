#pragma once

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// PE分析对话框类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

struct INFO_POSITION {
	UINT32 offset;
	UINT32 length;
};


class CDialogPE : public CDialogEx
{
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PE_DLG };
#endif

	DECLARE_DYNAMIC(CDialogPE)
	DECLARE_MESSAGE_MAP()

private:
	PCHAR m_pBuffer;					// 缓冲区指针
	UINT32 m_length;					// 缓冲区长度
	UINT32 m_isMemImage;				// 是否内存镜像
	CString m_path;						// 文件路径
	BOOL m_isDirty;						// 是否修改过

	CMenu m_menu;						// 主菜单
	CStatic m_static_path;				// 路径 静态文本框
	CEdit m_edit_path;					// 路径 编辑框
	CTreeCtrl m_tree_header;			// header 树控件
	CListCtrl m_list_section;			// section 列表控件
	CButton m_button_table[16];			// 表格控件数组
	CEdit m_edit_text;					// 文本信息编辑框

	INFO_POSITION m_infoPosition[100];	// 用来存储信息的位置和长度,方便在hex中高亮显示,0-50 用于header,50-80 用于section,80-100 未使用

	UINT32 m_showFlag;					// 显示标志: 0:CEditText,1:Hex
	#define MAX_SHOW_FLAG 2				// 显示标志最大值
	INT32 m_hexBegin;					// hex显示的起始位置
	INT32 m_hexBeginWithBkColor;		// hex显示的起始位置,带背景色
	INT32 m_hexLengthWithBkColor;		// hex显示的结束位置,带背景色
	UINT32 m_hexColor;					// hex显示的背景色	
	UINT32 m_hexFontSize;				// hex显示的字体大小
	CRect m_hexRect;					// hex显示的矩形区域
	UINT32 m_hexModifyCursor;			// hex显示的当前修改位置
	UINT32 m_defaultBkColor;			// 默认背景色
	CScrollBar m_scrollbar_hex;			// hex显示的滚动条
	


public:
	CDialogPE(PCHAR pBuffer,UINT32 length,UINT32 isMemImage = 0, CString path = TEXT(""), CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDialogPE();


public:
	int mCreateItems();				// 创建控件
	int mInitItems();				// 初始化控件
	int mAnalyzeHeaderInfo();		// 分析header信息
	int mAnalyzeSectionInfo();		// 分析section信息
	int mAnalyzeTableInfo();		// 分析table信息
	int mAnalyzePEFile();			// 分析PE文件信息 : 分析header,section,table信息
	int mRefreshPage(char* pNewBuffer = NULL);  // 刷新页面,可更新缓冲区
	int mSwitchShowFlag(int showFlag);
	int mDrawHex(CPaintDC* pDc);
	int mPixelsToPoints(CDC* pDc, int pixels);
	int mSetScrollBarHex();
	UINT32 mGetHexPageRange();

protected:
	afx_msg virtual BOOL OnInitDialog();
	afx_msg virtual void OnCancel();
	afx_msg virtual void PostNcDestroy();
	afx_msg void OnButtonClickDataDirectory(UINT id);
	afx_msg void OnListCtrlSelectMenu(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDlgPeMenuTest();
	afx_msg void OnSectionModify();
	afx_msg void OnSectionAdd();
	afx_msg void OnTestSave();
	afx_msg void OnSectionMerge();
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
public:
	int mSetHexBegin(int hexBegin);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
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


// HEX 属性定义
#define LEFT_HEX_ADDR_CHAR_NUM 9	// 左边地址显示占用的字符数
#define CSCROLLBAR_HEX_V_ID 1032
#define CSCROLLBAR_HEX_V_WIDTH 15
#define CSCROLLBAR_HEX_H_ID 1033
