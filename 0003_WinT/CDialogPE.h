#pragma once

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// PE分析对话框类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$



class CDialogPE : public CDialogEx
{
	// 用来记录高亮显示的位置和长度,方便在hex中高亮显示
	struct INFO_POSITION {
		UINT32 offset;
		UINT32 length;
	};

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PE_DLG };
#endif

	DECLARE_DYNAMIC(CDialogPE)
	DECLARE_MESSAGE_MAP()

private:
	// 文件数据信息
	PCHAR m_pBuffer;					// 缓冲区指针
	int m_length;						// 缓冲区长度
	UINT32 m_isMemImage;				// 是否内存镜像
	CString m_path;						// 文件路径
	BOOL m_isDirty;						// 是否修改过
	UINT32 m_validPEFlags;				// 是否正确的PE文件: 按位标记  0:文件大小或指纹,1:DOS头,2:NT头,3:节表,4:数据目录

	// 界面控件
	CMenu m_menu;						// menu 主菜单
	HACCEL m_hAccelTable;				// 加速键
	CStatic m_static_path;				// edit 路径 静态文本框
	CEdit m_edit_path;					// edit 路径 编辑框
	CTreeCtrl m_tree_header;			// header 树控件
	CListCtrl m_list_section;			// section 列表控件
	CButton m_button_table[16];			// button 表格控件数组
	CEdit m_edit_text;					// edit 文本信息编辑框
	CScrollBar m_scrollbar_hex;			// scrollbar 显示的滚动条
	INFO_POSITION m_infoPosition[100];	// 用来存储信息的位置和长度,方便在hex中高亮显示,0-50 用于header,50-80 用于section,80-100 未使用
	CRect m_hexRect;					// hex显示的矩形区域
	CEdit m_edit_bottomInfo;			// 底部信息编辑框

	// hex显示信息配置
	#define MAX_SHOW_FLAG 2				// 显示标志最大值
	UINT32 m_showFlag;					// 显示标志: 0:CEditText,1:Hex
	INT32 m_hexBegin;					// hex显示的起始位置
	INT32 m_hexBeginWithBkColor;		// hex显示的起始位置,带背景色
	INT32 m_hexLengthWithBkColor;		// hex显示的结束位置,带背景色
	UINT32 m_hexColor;					// hex显示的背景色	
	int m_hexFontSize;					// hex显示的字体大小
	UINT32 m_hexModifyCursor;			// hex显示的当前修改位置
	UINT32 m_defaultBkColor;			// hex默认背景色
	int m_flagEraseBk;					// 背景擦除标志


public:
	CDialogPE(PCHAR pBuffer,UINT32 length,UINT32 isMemImage = 0, CString path = NULL, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDialogPE();

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();
	virtual void PostNcDestroy();

private:
	afx_msg void OnDlgPeMenuTest();
	afx_msg void OnDlgPeMenuSwitch();
	afx_msg void OnDlgPeMenuSave();

	afx_msg void OnPaint();
	afx_msg void OnTreeCtrlSelectChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListCtrlSelectMenu(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListCtrlSelectChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonClickDataDirectory(UINT id);
	afx_msg void OnSectionModify();
	afx_msg void OnSectionAdd();
	afx_msg void OnSectionMerge();

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnHexGoto();

private:
	int mCreateItems();									// 创建控件
	int mInitItems();									// 初始化控件
	int mAnalyzeHeaderInfo();							// 分析header信息
	int mAnalyzeSectionInfo();							// 分析section信息
	int mAnalyzeTableInfo();							// 分析table信息
	int mAnalyzePEFile();								// 分析PE文件信息 : 分析header,section,table信息
	int mRefreshPage(char* pNewBuffer=NULL,int len=0);	// 刷新页面,可更新缓冲区
	int mSwitchShowFlag(int showFlag);					// 切换显示标志
	int mDrawHex(CPaintDC* pDc);						// 绘制hex
	int mSetScrollBarHex();								// 设置滚动条hex
	int mSetHexBegin(int hexBegin);						// 设置hex显示的起始位置
	int mGetPageRange();								// 获取页面范围
	int mSetHexBeginColored(int begin, int length);		// 设置hex显示的起始位置,带背景色
	int mSetEditText(CString text);						// 设置编辑文本信息
	void mGetPEBaseInfo();								// 获取PE基本信息
	void mGetPEexportTableInfo();						// 获取导出表信息
	void mGetPEImportTableInfo();						// 获取导入表信息
	void mEnumResourceTree(int deep, CStringW& str, IMAGE_RESOURCE_DIRECTORY* root, PCHAR baseAddr);	// 递归枚举资源树
	void mGetPEResourceTableInfo();						// 获取资源表信息
	void mGetPESEHTableInfo();							// 获取SEH表信息
	void mGetPECertificateTableInfo();					// 获取证书表信息
	void mGetPEBaseRelocationTableInfo();				// 获取基址重定位表信息
	void mGetPEDebugTableInfo();						// 获取调试表信息
	void mGetPEArchitectureTableInfo();					// 获取架构表信息
	void mGetPEGlobalPTRTableInfo();					// 获取全局PTR表信息
	void mGetTLSTableInfo();							// 获取TLS表信息
	void mGetPELoadConfigTableInfo();					// 获取加载配置表信息
	void mGetPEBoundImportTableInfo();					// 获取边界导入表信息
	void mGetPEIATTableInfo();							// 获取IAT表信息
	void mGetPEDelayLoadImportTableInfo();				// 获取延迟加载导入表信息
	void mGetPEComTableInfo();							// 获取COM表信息
public:




	afx_msg void OnDlgPeMenuToFile();
	afx_msg void OnDlgPeMenuToImage();
	afx_msg void OnDlgPeMenuImportInject();
	afx_msg void OnDlgPeMenuFakeShellExe();
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 宏定义

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 通用控件尺寸
#define MARGIN_COMMON 10		// 通用边距
#define ITEM_HEIGHT_COMMON 20	// 控件通用高度定义
#define HALF_WIDTH 630			// 半个长度定义

// CStatic  path 控件定义
#define CSTATIC_PATH_ID 1000
#define CSTATIC_PATH_WIDTH 50

// CEdit path 控件定义
#define CEDIT_PATH_ID 1001
#define CEDIT_PATH_WIDTH 600

// CTree header 控件定义
#define CTREE_HEADER_ID 1002
#define CTREE_HEADER_HEIGHT 200

// CList section 控件定义
#define CLIST_SECTION_ID 1003
#define CLIST_SECTION_HEIGHT 200

// CButton table 控件定义
#define CBUTTON_TABLE_ID_BEGIN 1004
#define CBUTTON_TABLE_ID_END 1030
#define CBUTTON_TABLE_WIDTH 150
#define CBUTTON_TABLE_HEIGHT 30

// CEdit text 控件定义
#define CEDIT_TEXT_ID 1031

// CScrollBar HEX 属性定义
#define CSCROLLBAR_HEX_V_ID 1032
#define CSCROLLBAR_HEX_V_WIDTH 15
#define CSCROLLBAR_HEX_H_ID 1033

// CEdit  bottom info 控件定义
#define CEDIT_BOTTOM_INFO_ID 1034
#define CEDIT_BOTTOM_INFO_HEIGHT 20

// hex
#define LEFT_HEX_ADDR_CHAR_NUM 9	// hex 左边地址显示占用的字符数

// 字体定义
#define FONT_TYPE TEXT("Courier New")
