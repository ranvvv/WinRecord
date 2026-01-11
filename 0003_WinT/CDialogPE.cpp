#include<windows.h>
#include "pch.h"
#include "WinT.h"
#include "CDialogPE.h"
#include "afxdialogex.h"
#include "PE.h"
#include "CDialogSection.h"
#include "CDialogInput.h"
#include "Common.h"



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// PE分析对话框

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

IMPLEMENT_DYNAMIC(CDialogPE, CDialogEx)

BEGIN_MESSAGE_MAP(CDialogPE, CDialogEx)
	// 快捷键: 跳转到
	ON_COMMAND(ID_HEX_GOTO, OnHexGoto)
	// 菜单项: 切换界面显示项
	ON_COMMAND(ID_DLG_PE_MENU_SWITCH, OnMenuSwitchShowFlag)
	// 菜单项: 修改节
	ON_COMMAND(ID_SECTION_MODIFY, OnMenuModifySection)
	// 菜单项: 添加节
	ON_COMMAND(ID_SECTION_ADD, OnMenuAddSection)
	// 菜单项: 合并节
	ON_COMMAND(ID_SECTION_MERGE, OnMenuMergeSection)
	// 菜单项: 测试按钮
	ON_COMMAND(ID_DLG_PE_MENU_TEST, OnMenuTest)
	// 菜单项: 保存文件
	ON_COMMAND(ID_DLG_PE_MENU_SAVE, OnMenuSave)
	// 菜单项: Image 转 file
	ON_COMMAND(ID_DLG_PE_MENU_TO_FILE, OnMenuImageToFile)
	// 菜单项: file 转 Image
	ON_COMMAND(ID_DLG_PE_MENU_TO_IMAGE, OnMenuFileToImage)
	// 菜单项: 注入导入表
	ON_COMMAND(ID_DLG_PE_MENU_IMPORT_INJECT, OnMenuImportInject)
	// 菜单项: 借壳执行
	ON_COMMAND(ID_DLG_PE_MENU_FAKE_SHELL_EXE, OnMenuFakeShellExe)
	// 菜单项: 加壳
	ON_COMMAND(ID_DLG_PE_MENU_ADD_SHELL, OnMenuAddShell)
	// CButton控件 : 点击事件
	ON_COMMAND_RANGE(CBUTTON_TABLE_ID_BEGIN, CBUTTON_TABLE_ID_BEGIN + 15, OnButtonDataDirectoryClick)
	// CListCtrl Section控件 : 右键点击
	ON_NOTIFY(NM_RCLICK, CLIST_SECTION_ID, OnListCtrlSectionSelectMenu)
	// CTreeCtrl Header : 选中改变事件
	ON_NOTIFY(TVN_SELCHANGED, CTREE_HEADER_ID, OnTreeCtrlHeaderInfoSelectChange)
	// CListCtrl Section: 选中改变事件
	ON_NOTIFY(NM_CLICK, CLIST_SECTION_ID, OnListCtrlSectionSelectChange)
	// 绘图
	ON_WM_PAINT()
	// 鼠标滚轮
	ON_WM_MOUSEHWHEEL()
	// 滚动条
	ON_WM_VSCROLL()
	// 鼠标滚轮滚动
	ON_WM_MOUSEWHEEL()
	// 擦除背景
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// 构造函数
CDialogPE::CDialogPE(PCHAR pBuffer, UINT32 bufferSize, UINT32 isImageBuffer, CString path, CWnd* pParent) : CDialogEx(IDD_PE_DLG, pParent)
{
	m_pBuffer = pBuffer;
	m_bufferSize = bufferSize;
	m_isImageBuffer = isImageBuffer;
	m_path = path;
	m_isDirty = FALSE;
	m_validPEFlags = 0;

	m_hexRect.SetRect(0, 0, 0, 0);

	m_showFlag = 1;
	m_hexBegin = 0;
	m_hexBeginWithBkColor = 0;
	m_hexLengthWithBkColor = 0;
	m_hexColor = RGB(0, 123, 0);
	m_hexFontSize = 15;
	m_hexModifyCursor = 0;
	m_defaultBkColor = RGB(255, 255, 255);
	m_flagEraseBk = FALSE;
}

// 析构函数
CDialogPE::~CDialogPE()
{
	// 释放内存区域
	if (m_pBuffer)
		free(m_pBuffer);
}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 功能函数

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 创建控件
int CDialogPE::mCreateItems()
{
	BOOL bRet = FALSE;
	DWORD dwStyle = 0;
	LONG lStyle = 0;
	int left = 0, top = 0, right = 0, bottom = 0;

	// CMenu  主菜单
	m_menu.LoadMenu(MENU_PE_DLG_MAIN);
	SetMenu(&m_menu);

	// 创建 CStatic 控件 path
	left = MARGIN_COMMON;
	top = MARGIN_COMMON;
	right = left + CSTATIC_PATH_WIDTH;
	bottom = top + ITEM_HEIGHT_COMMON;
	dwStyle = WS_CHILD | WS_VISIBLE;
	bRet = m_static_path.Create(_T("路径:"), dwStyle, CRect(left, top, right, bottom), this, CSTATIC_PATH_ID);
	if (!bRet)
		return -1;

	// 创建 CEdit 控件 path
	left = right + MARGIN_COMMON;
	right = MARGIN_COMMON + HALF_WIDTH;
	dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY;
	bRet = m_edit_path.Create(dwStyle, CRect(left, top, right, bottom), this, CEDIT_PATH_ID);
	if (!bRet)
		return -1;

	// 创建 CTreeCtrl 控件 header
	top = bottom + MARGIN_COMMON;
	left = MARGIN_COMMON;
	right = left + HALF_WIDTH;
	bottom = top + CTREE_HEADER_HEIGHT;
	dwStyle = WS_VISIBLE | WS_TABSTOP | WS_CHILD | WS_BORDER | WS_VSCROLL | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES | TVS_DISABLEDRAGDROP | TVS_NOTOOLTIPS;
	bRet = m_tree_header.Create(dwStyle, CRect(left, top, right, bottom), this, CTREE_HEADER_ID);
	if (!bRet)
		return -1;

	// 创建 CListCtrl 控件 section
	top = bottom + MARGIN_COMMON;
	left = MARGIN_COMMON;
	right = left + HALF_WIDTH;
	bottom = top + CLIST_SECTION_HEIGHT;
	dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_VSCROLL;
	bRet = m_list_section.Create(dwStyle, CRect(left, top, right, bottom), this, CLIST_SECTION_ID);
	if (!bRet)
		return -1;
	// 扩展样式
	lStyle = GetWindowLong(m_list_section.m_hWnd, GWL_STYLE);		// 获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; 										// 清除显示方式位
	lStyle |= LVS_REPORT; 											// 设置style
	lStyle |= LVS_SINGLESEL;										// 单选模式
	SetWindowLong(m_list_section.m_hWnd, GWL_STYLE, lStyle);		// 设置style
	// 扩展样式
	dwStyle = m_list_section.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	//dwStyle |= LVS_EX_CHECKBOXES;
	m_list_section.SetExtendedStyle(dwStyle);
	// 设置列头
	m_list_section.InsertColumn(0, _T("索引"), LVCFMT_LEFT, 40);
	m_list_section.InsertColumn(1, _T("名称"), LVCFMT_LEFT, 58);
	m_list_section.InsertColumn(2, _T("内存大小"), LVCFMT_LEFT, 90);
	m_list_section.InsertColumn(3, _T("内存基址"), LVCFMT_LEFT, 90);
	m_list_section.InsertColumn(4, _T("文件大小"), LVCFMT_LEFT, 90);
	m_list_section.InsertColumn(5, _T("文件基址"), LVCFMT_LEFT, 90);
	m_list_section.InsertColumn(6, _T("属性"), LVCFMT_LEFT, 90);

	// 创建 CButton 控件 table
	dwStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;
	for (int i = 0; i < 4; ++i)
	{
		top = bottom + MARGIN_COMMON;
		bottom = top + CBUTTON_TABLE_HEIGHT;
		for (int j = 0; j < 4; ++j)
		{
			left = MARGIN_COMMON + j * CBUTTON_TABLE_WIDTH + j * MARGIN_COMMON;
			right = left + CBUTTON_TABLE_WIDTH;
			bRet = m_button_table[i * 4 + j].Create(_T(""), dwStyle, CRect(left, top, right, bottom), this, CBUTTON_TABLE_ID_BEGIN + i * 4 + j);
			if (!bRet)
				return -1;
		}
	}
	m_button_table[0].SetWindowText(TEXT("导出表"));
	m_button_table[1].SetWindowText(TEXT("导入表"));
	m_button_table[2].SetWindowText(TEXT("资源表"));
	m_button_table[3].SetWindowText(TEXT("异常信息表"));
	m_button_table[4].SetWindowText(TEXT("安全证书表"));
	m_button_table[5].SetWindowText(TEXT("重定位表"));
	m_button_table[6].SetWindowText(TEXT("调试信息表"));
	m_button_table[7].SetWindowText(TEXT("版权所有表"));
	m_button_table[8].SetWindowText(TEXT("全局指针表"));
	m_button_table[9].SetWindowText(TEXT("TLS表"));
	m_button_table[10].SetWindowText(TEXT("加载配置表"));
	m_button_table[11].SetWindowText(TEXT("绑定导入表"));
	m_button_table[12].SetWindowText(TEXT("IAT表"));
	m_button_table[13].SetWindowText(TEXT("延迟导入表"));
	m_button_table[14].SetWindowText(TEXT("COM信息表"));
	m_button_table[15].SetWindowText(TEXT("基本信息"));

	// 创建 CEdit 控件 text 用来显示文本信息
	top = MARGIN_COMMON;
	left = MARGIN_COMMON + HALF_WIDTH + MARGIN_COMMON;
	right = left + HALF_WIDTH;
	m_hexRect.SetRect(left, top, right, bottom);	// 设置hex控件的矩形区域
	dwStyle = WS_HSCROLL | WS_VSCROLL | WS_CHILD | ES_READONLY | ES_MULTILINE | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL;
	bRet = m_edit_text.Create(dwStyle, m_hexRect, this, CEDIT_TEXT_ID);
	if (!bRet)
		return -1;

	// 创建CScrollBar hex 滚动条控件
	left = right;
	right = left + CSCROLLBAR_HEX_V_WIDTH;
	dwStyle = WS_CHILD | WS_VISIBLE | SBS_VERT;
	bRet = m_scrollbar_hex.Create(dwStyle, CRect(left, top, right, bottom), this, CSCROLLBAR_HEX_V_ID);
	if (!bRet)
		return -1;

	// 创建CEdit 底部状态栏控件
	top = bottom + MARGIN_COMMON;
	bottom = top + CEDIT_BOTTOM_INFO_HEIGHT;
	left = 0;
	dwStyle = WS_CHILD | ES_READONLY  | WS_VISIBLE;
	bRet = m_edit_bottomInfo.Create(dwStyle, CRect(left, top, right, bottom), this, CEDIT_BOTTOM_INFO_ID);
	if (!bRet)
		return -1;

	return 0;
}

// 初始化控件
int CDialogPE::mInitItems()
{
	// CEdit 控件 path
	m_edit_path.SetWindowText(TEXT(""));
	m_edit_path.EnableWindow(FALSE);

	// CTreeCtrl 控件 header
	m_tree_header.DeleteAllItems();
	m_tree_header.EnableWindow(FALSE);

	// CListCtrl 控件 section
	m_list_section.DeleteAllItems();
	m_list_section.EnableWindow(FALSE);

	// CButton 控件 table
	for (int i = 0; i < 16; ++i)
		m_button_table[i].EnableWindow(FALSE);

	// CEdit 控件 text
	m_edit_text.SetWindowText(TEXT(""));
	m_edit_text.ShowWindow(m_showFlag == 0);
	m_edit_text.EnableWindow(FALSE);

	// CScrollBar 控件 hex
	m_scrollbar_hex.ShowWindow(m_showFlag == 1);
	mSetScrollBarHexInfo();		// 设置滚动条hex的参数
	m_scrollbar_hex.EnableWindow(FALSE);

	// CEdit 控件 bottomInfo
	m_edit_bottomInfo.SetWindowText(TEXT(""));
	m_edit_bottomInfo.EnableWindow(FALSE);

	return 0;
}

// 分析头部信息
int CDialogPE::mAnalyzeHeaderInfo()
{
	try {
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		CString str;
		UINT32 offset = 0;

		m_tree_header.EnableWindow(TRUE);

		HTREEITEM hRoot = NULL;                 // 根节点
		HTREEITEM hFileHeader = NULL;			// 文件头节点
		HTREEITEM hOptionalHeader = NULL;		// 可选头节点
		HTREEITEM hChild = NULL;

		offset = pDos->e_lfanew;

		// NT_HEADER
		hRoot = m_tree_header.InsertItem(_T("IMAGE_NT_HEADER"));
		m_infoPosition[0].offset = offset;
		m_infoPosition[0].length = sizeof(pNt32->Signature) + sizeof(pNt32->FileHeader) + pNt32->FileHeader.SizeOfOptionalHeader;
		m_tree_header.SetItemData(hRoot, (ULONG_PTR)&m_infoPosition[0]);

		// Signature
		str.Format(TEXT("Signature: 0x%08X"), pNt32->Signature);
		hChild = m_tree_header.InsertItem(str, hRoot);
		m_infoPosition[1].offset = offset;
		m_infoPosition[1].length = sizeof(pNt32->Signature);
		m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[1]);
		offset += sizeof(pNt32->Signature);

		// ------------------------  FILE_HEADER 

		hFileHeader = m_tree_header.InsertItem(_T("IMAGE_FILE_HEADER"), hRoot);
		m_infoPosition[2].offset = offset;
		m_infoPosition[2].length = sizeof(pNt32->FileHeader);
		m_tree_header.SetItemData(hFileHeader, (ULONG_PTR)&m_infoPosition[2]);

		str.Format(TEXT("Machine: 0x%04X"), pNt32->FileHeader.Machine);
		hChild = m_tree_header.InsertItem(str, hFileHeader);
		m_infoPosition[3].offset = offset;
		m_infoPosition[3].length = sizeof(pNt32->FileHeader.Machine);
		m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[3]);
		offset += sizeof(pNt32->FileHeader.Machine);

		str.Format(TEXT("NumberOfSections: %d"), pNt32->FileHeader.NumberOfSections);
		hChild = m_tree_header.InsertItem(str, hFileHeader);
		m_infoPosition[4].offset = offset;
		m_infoPosition[4].length = sizeof(pNt32->FileHeader.NumberOfSections);
		m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[4]);
		offset += sizeof(pNt32->FileHeader.NumberOfSections);

		str.Format(TEXT("TimeDateStamp: 0x%08X"), pNt32->FileHeader.TimeDateStamp);
		hChild = m_tree_header.InsertItem(str, hFileHeader);
		m_infoPosition[5].offset = offset;
		m_infoPosition[5].length = sizeof(pNt32->FileHeader.TimeDateStamp);
		m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[5]);
		offset += sizeof(pNt32->FileHeader.TimeDateStamp);

		str.Format(TEXT("PointerToSymbolTable: 0x%08X"), pNt32->FileHeader.PointerToSymbolTable);
		hChild = m_tree_header.InsertItem(str, hFileHeader);
		m_infoPosition[6].offset = offset;
		m_infoPosition[6].length = sizeof(pNt32->FileHeader.PointerToSymbolTable);
		m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[6]);
		offset += sizeof(pNt32->FileHeader.PointerToSymbolTable);

		str.Format(TEXT("NumberOfSymbols: %d"), pNt32->FileHeader.NumberOfSymbols);
		hChild = m_tree_header.InsertItem(str, hFileHeader);
		m_infoPosition[7].offset = offset;
		m_infoPosition[7].length = sizeof(pNt32->FileHeader.NumberOfSymbols);
		m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[7]);
		offset += sizeof(pNt32->FileHeader.NumberOfSymbols);

		str.Format(TEXT("SizeOfOptionalHeader: %d"), pNt32->FileHeader.SizeOfOptionalHeader);
		hChild = m_tree_header.InsertItem(str, hFileHeader);
		m_infoPosition[8].offset = offset;
		m_infoPosition[8].length = sizeof(pNt32->FileHeader.SizeOfOptionalHeader);
		m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[8]);
		offset += sizeof(pNt32->FileHeader.SizeOfOptionalHeader);

		str.Format(TEXT("Characteristics: 0x%04X"), pNt32->FileHeader.Characteristics);
		hChild = m_tree_header.InsertItem(str, hFileHeader);
		m_infoPosition[9].offset = offset;
		m_infoPosition[9].length = sizeof(pNt32->FileHeader.Characteristics);
		m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[9]);
		offset += sizeof(pNt32->FileHeader.Characteristics);

		// OPTIONAL_HEADER
		if (isX64(p))
		{
			hOptionalHeader = m_tree_header.InsertItem(_T("IMAGE_OPTIONAL_HEADER64"), hRoot);
			m_infoPosition[10].offset = offset;
			m_infoPosition[10].length = pNt64->FileHeader.SizeOfOptionalHeader;
			m_tree_header.SetItemData(hOptionalHeader, (ULONG_PTR)&m_infoPosition[10]);

			str.Format(TEXT("Magic: 0x%04X"), pNt64->OptionalHeader.Magic);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[11].offset = offset;
			m_infoPosition[11].length = sizeof(pNt64->OptionalHeader.Magic);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[11]);
			offset += sizeof(pNt64->OptionalHeader.Magic);

			str.Format(TEXT("MajorLinkerVersion: %d"), pNt64->OptionalHeader.MajorLinkerVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[12].offset = offset;
			m_infoPosition[12].length = sizeof(pNt64->OptionalHeader.MajorLinkerVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[12]);
			offset += sizeof(pNt64->OptionalHeader.MajorLinkerVersion);

			str.Format(TEXT("MinorLinkerVersion: %d"), pNt64->OptionalHeader.MinorLinkerVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[13].offset = offset;
			m_infoPosition[13].length = sizeof(pNt64->OptionalHeader.MinorLinkerVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[13]);
			offset += sizeof(pNt64->OptionalHeader.MinorLinkerVersion);

			str.Format(TEXT("SizeOfCode: %d"), pNt64->OptionalHeader.SizeOfCode);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[14].offset = offset;
			m_infoPosition[14].length = sizeof(pNt64->OptionalHeader.SizeOfCode);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[14]);
			offset += sizeof(pNt64->OptionalHeader.SizeOfCode);

			str.Format(TEXT("SizeOfInitializedData: %d"), pNt64->OptionalHeader.SizeOfInitializedData);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[15].offset = offset;
			m_infoPosition[15].length = sizeof(pNt64->OptionalHeader.SizeOfInitializedData);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[15]);
			offset += sizeof(pNt64->OptionalHeader.SizeOfInitializedData);

			str.Format(TEXT("SizeOfUninitializedData: %d"), pNt64->OptionalHeader.SizeOfUninitializedData);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[16].offset = offset;
			m_infoPosition[16].length = sizeof(pNt64->OptionalHeader.SizeOfUninitializedData);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[16]);
			offset += sizeof(pNt64->OptionalHeader.SizeOfUninitializedData);

			str.Format(TEXT("AddressOfEntryPoint: 0x%08X"), pNt64->OptionalHeader.AddressOfEntryPoint);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[17].offset = offset;
			m_infoPosition[17].length = sizeof(pNt64->OptionalHeader.AddressOfEntryPoint);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[17]);
			offset += sizeof(pNt64->OptionalHeader.AddressOfEntryPoint);

			str.Format(TEXT("BaseOfCode: 0x%08X"), pNt64->OptionalHeader.BaseOfCode);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[18].offset = offset;
			m_infoPosition[18].length = sizeof(pNt64->OptionalHeader.BaseOfCode);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[18]);
			offset += sizeof(pNt64->OptionalHeader.BaseOfCode);

			str.Format(TEXT("ImageBase: 0x%I64X"), pNt64->OptionalHeader.ImageBase);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[19].offset = offset;
			m_infoPosition[19].length = sizeof(pNt64->OptionalHeader.ImageBase);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[19]);
			offset += sizeof(pNt64->OptionalHeader.ImageBase);

			str.Format(TEXT("SectionAlignment: 0x%08X"), pNt64->OptionalHeader.SectionAlignment);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[20].offset = offset;
			m_infoPosition[20].length = sizeof(pNt64->OptionalHeader.SectionAlignment);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[20]);
			offset += sizeof(pNt64->OptionalHeader.SectionAlignment);

			str.Format(TEXT("FileAlignment: 0x%08X"), pNt64->OptionalHeader.FileAlignment);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[21].offset = offset;
			m_infoPosition[21].length = sizeof(pNt64->OptionalHeader.FileAlignment);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[21]);
			offset += sizeof(pNt64->OptionalHeader.FileAlignment);

			str.Format(TEXT("MajorOperatingSystemVersion: %d"), pNt64->OptionalHeader.MajorOperatingSystemVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[22].offset = offset;
			m_infoPosition[22].length = sizeof(pNt64->OptionalHeader.MajorOperatingSystemVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[22]);
			offset += sizeof(pNt64->OptionalHeader.MajorOperatingSystemVersion);

			str.Format(TEXT("MinorOperatingSystemVersion: %d"), pNt64->OptionalHeader.MinorOperatingSystemVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[23].offset = offset;
			m_infoPosition[23].length = sizeof(pNt64->OptionalHeader.MinorOperatingSystemVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[23]);
			offset += sizeof(pNt64->OptionalHeader.MinorOperatingSystemVersion);

			str.Format(TEXT("MajorImageVersion: %d"), pNt64->OptionalHeader.MajorImageVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[24].offset = offset;
			m_infoPosition[24].length = sizeof(pNt64->OptionalHeader.MajorImageVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[24]);
			offset += sizeof(pNt64->OptionalHeader.MajorImageVersion);

			str.Format(TEXT("MinorImageVersion: %d"), pNt64->OptionalHeader.MinorImageVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[25].offset = offset;
			m_infoPosition[25].length = sizeof(pNt64->OptionalHeader.MinorImageVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[25]);
			offset += sizeof(pNt64->OptionalHeader.MinorImageVersion);

			str.Format(TEXT("MajorSubsystemVersion: %d"), pNt64->OptionalHeader.MajorSubsystemVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[26].offset = offset;
			m_infoPosition[26].length = sizeof(pNt64->OptionalHeader.MajorSubsystemVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[26]);
			offset += sizeof(pNt64->OptionalHeader.MajorSubsystemVersion);

			str.Format(TEXT("MinorSubsystemVersion: %d"), pNt64->OptionalHeader.MinorSubsystemVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[27].offset = offset;
			m_infoPosition[27].length = sizeof(pNt64->OptionalHeader.MinorSubsystemVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[27]);
			offset += sizeof(pNt64->OptionalHeader.MinorSubsystemVersion);

			str.Format(TEXT("Win32VersionValue: 0x%08X"), pNt64->OptionalHeader.Win32VersionValue);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[28].offset = offset;
			m_infoPosition[28].length = sizeof(pNt64->OptionalHeader.Win32VersionValue);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[28]);
			offset += sizeof(pNt64->OptionalHeader.Win32VersionValue);

			str.Format(TEXT("SizeOfImage: 0x%08X"), pNt64->OptionalHeader.SizeOfImage);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[29].offset = offset;
			m_infoPosition[29].length = sizeof(pNt64->OptionalHeader.SizeOfImage);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[29]);
			offset += sizeof(pNt64->OptionalHeader.SizeOfImage);

			str.Format(TEXT("SizeOfHeaders: 0x%08X"), pNt64->OptionalHeader.SizeOfHeaders);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[30].offset = offset;
			m_infoPosition[30].length = sizeof(pNt64->OptionalHeader.SizeOfHeaders);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[30]);
			offset += sizeof(pNt64->OptionalHeader.SizeOfHeaders);

			str.Format(TEXT("CheckSum: 0x%08X"), pNt64->OptionalHeader.CheckSum);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[31].offset = offset;
			m_infoPosition[31].length = sizeof(pNt64->OptionalHeader.CheckSum);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[31]);
			offset += sizeof(pNt64->OptionalHeader.CheckSum);

			str.Format(TEXT("Subsystem: %d"), pNt64->OptionalHeader.Subsystem);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[32].offset = offset;
			m_infoPosition[32].length = sizeof(pNt64->OptionalHeader.Subsystem);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[32]);
			offset += sizeof(pNt64->OptionalHeader.Subsystem);

			str.Format(TEXT("DllCharacteristics: 0x%08X"), pNt64->OptionalHeader.DllCharacteristics);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[33].offset = offset;
			m_infoPosition[33].length = sizeof(pNt64->OptionalHeader.DllCharacteristics);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[33]);
			offset += sizeof(pNt64->OptionalHeader.DllCharacteristics);

			str.Format(TEXT("SizeOfStackReserve: 0x%I64X"), pNt64->OptionalHeader.SizeOfStackReserve);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[34].offset = offset;
			m_infoPosition[34].length = sizeof(pNt64->OptionalHeader.SizeOfStackReserve);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[34]);
			offset += sizeof(pNt64->OptionalHeader.SizeOfStackReserve);

			str.Format(TEXT("SizeOfStackCommit: 0x%I64X"), pNt64->OptionalHeader.SizeOfStackCommit);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[35].offset = offset;
			m_infoPosition[35].length = sizeof(pNt64->OptionalHeader.SizeOfStackCommit);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[35]);
			offset += sizeof(pNt64->OptionalHeader.SizeOfStackCommit);

			str.Format(TEXT("SizeOfHeapReserve: 0x%I64X"), pNt64->OptionalHeader.SizeOfHeapReserve);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[36].offset = offset;
			m_infoPosition[36].length = sizeof(pNt64->OptionalHeader.SizeOfHeapReserve);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[36]);
			offset += sizeof(pNt64->OptionalHeader.SizeOfHeapReserve);

			str.Format(TEXT("SizeOfHeapCommit: 0x%I64X"), pNt64->OptionalHeader.SizeOfHeapCommit);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[37].offset = offset;
			m_infoPosition[37].length = sizeof(pNt64->OptionalHeader.SizeOfHeapCommit);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[37]);
			offset += sizeof(pNt64->OptionalHeader.SizeOfHeapCommit);

			str.Format(TEXT("LoaderFlags: 0x%08X"), pNt64->OptionalHeader.LoaderFlags);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[38].offset = offset;
			m_infoPosition[38].length = sizeof(pNt64->OptionalHeader.LoaderFlags);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[38]);
			offset += sizeof(pNt64->OptionalHeader.LoaderFlags);

			str.Format(TEXT("NumberOfRvaAndSizes: %d"), pNt64->OptionalHeader.NumberOfRvaAndSizes);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[39].offset = offset;
			m_infoPosition[39].length = sizeof(pNt64->OptionalHeader.NumberOfRvaAndSizes);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[39]);
			offset += sizeof(pNt64->OptionalHeader.NumberOfRvaAndSizes);
		}
		else
		{
			hOptionalHeader = m_tree_header.InsertItem(_T("IMAGE_OPTIONAL_HEADER32"), hRoot);
			m_infoPosition[10].offset = offset;
			m_infoPosition[10].length = pNt64->FileHeader.SizeOfOptionalHeader;
			m_tree_header.SetItemData(hOptionalHeader, (ULONG_PTR)&m_infoPosition[10]);

			str.Format(TEXT("Magic: 0x%04X"), pNt32->OptionalHeader.Magic);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[11].offset = offset;
			m_infoPosition[11].length = sizeof(pNt32->OptionalHeader.Magic);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[11]);
			offset += sizeof(pNt32->OptionalHeader.Magic);

			str.Format(TEXT("MajorLinkerVersion: %d"), pNt32->OptionalHeader.MajorLinkerVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[12].offset = offset;
			m_infoPosition[12].length = sizeof(pNt32->OptionalHeader.MajorLinkerVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[12]);
			offset += sizeof(pNt32->OptionalHeader.MajorLinkerVersion);

			str.Format(TEXT("MinorLinkerVersion: %d"), pNt32->OptionalHeader.MinorLinkerVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[13].offset = offset;
			m_infoPosition[13].length = sizeof(pNt32->OptionalHeader.MinorLinkerVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[13]);
			offset += sizeof(pNt32->OptionalHeader.MinorLinkerVersion);

			str.Format(TEXT("SizeOfCode: 0x%08X"), pNt32->OptionalHeader.SizeOfCode);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[14].offset = offset;
			m_infoPosition[14].length = sizeof(pNt32->OptionalHeader.SizeOfCode);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[14]);
			offset += sizeof(pNt32->OptionalHeader.SizeOfCode);

			str.Format(TEXT("SizeOfInitializedData: 0x%08X"), pNt32->OptionalHeader.SizeOfInitializedData);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[15].offset = offset;
			m_infoPosition[15].length = sizeof(pNt32->OptionalHeader.SizeOfInitializedData);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[15]);
			offset += sizeof(pNt32->OptionalHeader.SizeOfInitializedData);

			str.Format(TEXT("SizeOfUninitializedData: 0x%08X"), pNt32->OptionalHeader.SizeOfUninitializedData);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[16].offset = offset;
			m_infoPosition[16].length = sizeof(pNt32->OptionalHeader.SizeOfUninitializedData);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[16]);
			offset += sizeof(pNt32->OptionalHeader.SizeOfUninitializedData);

			str.Format(TEXT("AddressOfEntryPoint: 0x%08X"), pNt32->OptionalHeader.AddressOfEntryPoint);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[17].offset = offset;
			m_infoPosition[17].length = sizeof(pNt32->OptionalHeader.AddressOfEntryPoint);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[17]);
			offset += sizeof(pNt32->OptionalHeader.AddressOfEntryPoint);

			str.Format(TEXT("BaseOfCode: 0x%08X"), pNt32->OptionalHeader.BaseOfCode);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[18].offset = offset;
			m_infoPosition[18].length = sizeof(pNt32->OptionalHeader.BaseOfCode);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[18]);
			offset += sizeof(pNt32->OptionalHeader.BaseOfCode);

			str.Format(TEXT("BaseOfData: 0x%08X"), pNt32->OptionalHeader.BaseOfData);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[19].offset = offset;
			m_infoPosition[19].length = sizeof(pNt32->OptionalHeader.BaseOfData);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[19]);
			offset += sizeof(pNt32->OptionalHeader.BaseOfData);

			str.Format(TEXT("ImageBase: 0x%08X"), pNt32->OptionalHeader.ImageBase);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[20].offset = offset;
			m_infoPosition[20].length = sizeof(pNt32->OptionalHeader.ImageBase);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[20]);
			offset += sizeof(pNt32->OptionalHeader.ImageBase);

			str.Format(TEXT("SectionAlignment: 0x%08X"), pNt32->OptionalHeader.SectionAlignment);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[21].offset = offset;
			m_infoPosition[21].length = sizeof(pNt32->OptionalHeader.SectionAlignment);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[21]);
			offset += sizeof(pNt32->OptionalHeader.SectionAlignment);

			str.Format(TEXT("FileAlignment: 0x%08X"), pNt32->OptionalHeader.FileAlignment);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[22].offset = offset;
			m_infoPosition[22].length = sizeof(pNt32->OptionalHeader.FileAlignment);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[22]);
			offset += sizeof(pNt32->OptionalHeader.FileAlignment);

			str.Format(TEXT("MajorOperatingSystemVersion: %d"), pNt32->OptionalHeader.MajorOperatingSystemVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[23].offset = offset;
			m_infoPosition[23].length = sizeof(pNt32->OptionalHeader.MajorOperatingSystemVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[23]);
			offset += sizeof(pNt32->OptionalHeader.MajorOperatingSystemVersion);

			str.Format(TEXT("MinorOperatingSystemVersion: %d"), pNt32->OptionalHeader.MinorOperatingSystemVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[24].offset = offset;
			m_infoPosition[24].length = sizeof(pNt32->OptionalHeader.MinorOperatingSystemVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[24]);
			offset += sizeof(pNt32->OptionalHeader.MinorOperatingSystemVersion);

			str.Format(TEXT("MajorImageVersion: %d"), pNt32->OptionalHeader.MajorImageVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[25].offset = offset;
			m_infoPosition[25].length = sizeof(pNt32->OptionalHeader.MajorImageVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[25]);
			offset += sizeof(pNt32->OptionalHeader.MajorImageVersion);

			str.Format(TEXT("MinorImageVersion: %d"), pNt32->OptionalHeader.MinorImageVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[26].offset = offset;
			m_infoPosition[26].length = sizeof(pNt32->OptionalHeader.MinorImageVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[26]);
			offset += sizeof(pNt32->OptionalHeader.MinorImageVersion);

			str.Format(TEXT("MajorSubsystemVersion: %d"), pNt32->OptionalHeader.MajorSubsystemVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[27].offset = offset;
			m_infoPosition[27].length = sizeof(pNt32->OptionalHeader.MajorSubsystemVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[27]);
			offset += sizeof(pNt32->OptionalHeader.MajorSubsystemVersion);

			str.Format(TEXT("MinorSubsystemVersion: %d"), pNt32->OptionalHeader.MinorSubsystemVersion);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[28].offset = offset;
			m_infoPosition[28].length = sizeof(pNt32->OptionalHeader.MinorSubsystemVersion);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[28]);
			offset += sizeof(pNt32->OptionalHeader.MinorSubsystemVersion);

			str.Format(TEXT("Win32VersionValue: 0x%08X"), pNt32->OptionalHeader.Win32VersionValue);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[29].offset = offset;
			m_infoPosition[29].length = sizeof(pNt32->OptionalHeader.Win32VersionValue);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[29]);
			offset += sizeof(pNt32->OptionalHeader.Win32VersionValue);

			str.Format(TEXT("SizeOfImage: 0x%08X"), pNt32->OptionalHeader.SizeOfImage);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[30].offset = offset;
			m_infoPosition[30].length = sizeof(pNt32->OptionalHeader.SizeOfImage);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[30]);
			offset += sizeof(pNt32->OptionalHeader.SizeOfImage);

			str.Format(TEXT("SizeOfHeaders: 0x%08X"), pNt32->OptionalHeader.SizeOfHeaders);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[31].offset = offset;
			m_infoPosition[31].length = sizeof(pNt32->OptionalHeader.SizeOfHeaders);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[31]);
			offset += sizeof(pNt32->OptionalHeader.SizeOfHeaders);

			str.Format(TEXT("CheckSum: 0x%08X"), pNt32->OptionalHeader.CheckSum);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[32].offset = offset;
			m_infoPosition[32].length = sizeof(pNt32->OptionalHeader.CheckSum);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[32]);
			offset += sizeof(pNt32->OptionalHeader.CheckSum);

			str.Format(TEXT("Subsystem: %d"), pNt32->OptionalHeader.Subsystem);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[33].offset = offset;
			m_infoPosition[33].length = sizeof(pNt32->OptionalHeader.Subsystem);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[33]);
			offset += sizeof(pNt32->OptionalHeader.Subsystem);

			str.Format(TEXT("DllCharacteristics: 0x%08X"), pNt32->OptionalHeader.DllCharacteristics);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[34].offset = offset;
			m_infoPosition[34].length = sizeof(pNt32->OptionalHeader.DllCharacteristics);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[34]);
			offset += sizeof(pNt32->OptionalHeader.DllCharacteristics);

			str.Format(TEXT("SizeOfStackReserve: 0x%08X"), pNt32->OptionalHeader.SizeOfStackReserve);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[35].offset = offset;
			m_infoPosition[35].length = sizeof(pNt32->OptionalHeader.SizeOfStackReserve);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[35]);
			offset += sizeof(pNt32->OptionalHeader.SizeOfStackReserve);

			str.Format(TEXT("SizeOfStackCommit: 0x%08X"), pNt32->OptionalHeader.SizeOfStackCommit);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[36].offset = offset;
			m_infoPosition[36].length = sizeof(pNt32->OptionalHeader.SizeOfStackCommit);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[36]);
			offset += sizeof(pNt32->OptionalHeader.SizeOfStackCommit);

			str.Format(TEXT("SizeOfHeapReserve: 0x%08X"), pNt32->OptionalHeader.SizeOfHeapReserve);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[37].offset = offset;
			m_infoPosition[37].length = sizeof(pNt32->OptionalHeader.SizeOfHeapReserve);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[37]);
			offset += sizeof(pNt32->OptionalHeader.SizeOfHeapReserve);

			str.Format(TEXT("SizeOfHeapCommit: 0x%08X"), pNt32->OptionalHeader.SizeOfHeapCommit);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[38].offset = offset;
			m_infoPosition[38].length = sizeof(pNt32->OptionalHeader.SizeOfHeapCommit);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[38]);
			offset += sizeof(pNt32->OptionalHeader.SizeOfHeapCommit);

			str.Format(TEXT("LoaderFlags: 0x%08X"), pNt32->OptionalHeader.LoaderFlags);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[39].offset = offset;
			m_infoPosition[39].length = sizeof(pNt32->OptionalHeader.LoaderFlags);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[39]);
			offset += sizeof(pNt32->OptionalHeader.LoaderFlags);

			str.Format(TEXT("NumberOfRvaAndSizes: 0x%08X"), pNt32->OptionalHeader.NumberOfRvaAndSizes);
			hChild = m_tree_header.InsertItem(str, hOptionalHeader);
			m_infoPosition[40].offset = offset;
			m_infoPosition[40].length = sizeof(pNt32->OptionalHeader.NumberOfRvaAndSizes);
			m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[40]);
		}
	}
	catch (...)
	{
		m_edit_bottomInfo.SetWindowText(TEXT("头信息 分析失败,内存访问异常! 检查Header 结构是否正常!"));
		return -1;
	}

	return 0;
}

// 分析节信息
int CDialogPE::mAnalyzeSectionInfo()
{
	try {
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);
		CString str;
		int n;

		m_list_section.EnableWindow(TRUE);

		for (int i = 0; i < pNt32->FileHeader.NumberOfSections; ++i)
		{
			str.Format(TEXT("%d"), i);
			n = m_list_section.InsertItem(i, str);

			CStringA name((char*)(pSec[i].Name), 8);
			str = name;
			m_list_section.SetItemText(n, 1, str);

			str.Format(TEXT("0x%08X"), pSec[i].Misc.VirtualSize);
			m_list_section.SetItemText(n, 2, str);

			str.Format(TEXT("0x%08X"), pSec[i].VirtualAddress);
			m_list_section.SetItemText(n, 3, str);

			str.Format(TEXT("0x%08X"), pSec[i].SizeOfRawData);
			m_list_section.SetItemText(n, 4, str);

			str.Format(TEXT("0x%08X"), pSec[i].PointerToRawData);
			m_list_section.SetItemText(n, 5, str);

			str.Format(TEXT("0x%08X"), pSec[i].Characteristics);
			m_list_section.SetItemText(n, 6, str);

			m_infoPosition[50 + i].offset = (UINT32)((char*)&pSec[i] - p);
			m_infoPosition[50 + i].length = sizeof(IMAGE_SECTION_HEADER);
			m_list_section.SetItemData(n, (ULONG_PTR)&m_infoPosition[50 + i]);
		}
	}
	catch (...)
	{
		m_edit_bottomInfo.SetWindowText(TEXT("节信息 分析失败! 检查Section 结构是否正常!"));
		return -1;
	}

	return 0;
}

// 分析表信息
int CDialogPE::mAnalyzeTableInfo()
{
	try {
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		for (int i = 0; i < 15; ++i)
		{
			if (pDir[i].VirtualAddress)
				m_button_table[i].EnableWindow(TRUE);
		}

		m_button_table[15].EnableWindow(TRUE);
	}
	catch (...)
	{
		m_edit_bottomInfo.SetWindowText(TEXT("表信息 分析失败! 检查Dir 结构是否正常!"));
		return -1;
	}

	return 0;
}

// 分析PE文件,并更新各个控件状态
int CDialogPE::mAnalyzePEFile()
{
	int result = 0;

	// 初始化控件状态
	mInitItems();

	// path
	m_edit_path.SetWindowText(m_path);
	m_edit_path.EnableWindow(TRUE);

	// 不是正确的PE文件
	result = IsValidPE(m_pBuffer, m_bufferSize,m_isImageBuffer);
	if (result < 0)
		m_validPEFlags |= 1;

	// header
	result = mAnalyzeHeaderInfo();
	if (result < 0)
		m_validPEFlags |= 2;

	// section
	result = mAnalyzeSectionInfo();
	if (result < 0)
		m_validPEFlags |= 4;

	// button
	result = mAnalyzeTableInfo();
	if (result < 0)
		m_validPEFlags |= 8;

	// edit text
	m_edit_text.EnableWindow(TRUE);

	// scrollbar
	m_scrollbar_hex.EnableWindow(TRUE);

	// edit bottom info
	m_edit_bottomInfo.EnableWindow(TRUE);

	// title
	CString str = PathFindFileName(m_path);
	if (m_isImageBuffer)
		str.Append(TEXT(" (内存映像)"));
	else
		str.Append(TEXT(" (文件映像)"));

	if (m_isDirty)
		str.Append(TEXT(" (dirty)"));

	if (m_validPEFlags)
	{
		if (m_validPEFlags & 1)
			str.Append(TEXT(" (PE文件大小或指纹异常)"));
		if (m_validPEFlags & 2)
			str.Append(TEXT(" (头信息异常)"));
		if (m_validPEFlags & 4)
			str.Append(TEXT(" (节信息异常)"));
		if (m_validPEFlags & 8)
			str.Append(TEXT(" (表信息异常)"));
	}


	this->SetWindowTextW(str);

	// 重置hex显示起始位置，并刷新hex控件
	mSetHexBegin(0);               

	return 0;
}

// 更新m_pBuffer ,并刷新页面
int CDialogPE::mRefreshPage(char* pNewBuffer,int bufferSize ,int isImageBuffer)
{
	if (pNewBuffer)
	{
		if (m_pBuffer != pNewBuffer)
		{
			if (m_pBuffer)
			{
				free(m_pBuffer);
				m_isDirty = TRUE;   // 设置脏标记，表示缓冲区已经被修改了
			}
			m_pBuffer = pNewBuffer;
			m_bufferSize = bufferSize;
			m_isImageBuffer = isImageBuffer;
		}
	}

	mAnalyzePEFile();

	return 0;
}

// 切换CEdit text 和 hex  的显示
int CDialogPE::mSwitchShowFlag(int showFlag)
{
	if (showFlag == m_showFlag)
	{
		InvalidateRect(&m_hexRect);
		return 0;
	}

	m_showFlag = showFlag;

	switch (m_showFlag)
	{
	case 0:     // text模式
	{
		m_edit_text.ShowWindow(TRUE);
		m_scrollbar_hex.ShowWindow(FALSE);
		m_flagEraseBk = 1;      // 标记需要擦除背景,否则ScrollBar会残留
		break;
	}
	case 1:     // hex模式
	{
		m_edit_text.ShowWindow(FALSE);
		m_scrollbar_hex.ShowWindow(TRUE);
		break;
	}
	default:
	{
		break;
	}
	}

	// 更新右侧区域显示
	InvalidateRect(&m_hexRect);

	return 0;
}

// 获取页面范围
int CDialogPE::mGetPageRange()
{
	// 基于字体大小设置页面的行数
	CFont fontNormal;
	fontNormal.CreateFont(
		m_hexFontSize,             // nHeight - 字体高度
		0,                         // nWidth - 字体宽度(0表示自适应)
		0,                         // nEscapement - 文本角度(0.1度单位)
		0,                         // nOrientation - 字符角度
		FW_NORMAL,                 // nWeight - 字体粗细
		FALSE,                     // bItalic - 是否斜体
		FALSE,                     // bUnderline - 是否下划线
		FALSE,                     // bStrikeOut - 是否删除线
		DEFAULT_CHARSET,           // nCharSet - 字符集
		OUT_DEFAULT_PRECIS,        // nOutPrecision - 输出精度
		CLIP_DEFAULT_PRECIS,       // nClipPrecision - 剪裁精度
		DEFAULT_QUALITY,           // nQuality - 输出质量
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily - 字体系列
		FONT_TYPE                  // lpszFacename - 字体名称
	);
	CDC* pDC = GetDC();
	pDC->SelectObject(&fontNormal);
	CSize normalFontSize = pDC->GetTextExtent(TEXT("0"), 1);
	ReleaseDC(pDC);
	return m_hexRect.Height() / (normalFontSize.cy + normalFontSize.cy / 2) - 1;
}

// 设置滚动条范围
int CDialogPE::mSetScrollBarHexInfo()
{
	m_scrollbar_hex.EnableWindow(TRUE);
	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(si);
	si.nMin = 0;
	si.nMax = m_bufferSize / 16 + (m_bufferSize % 16 ? 1 : 0);
	si.nPage = mGetPageRange();
	si.nPos = m_hexBegin >> 4;
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
	m_scrollbar_hex.SetScrollInfo(&si, TRUE);

	return 0;
}

// 设置hex开始位置
int CDialogPE::mSetHexBegin(int hexBegin)
{
	hexBegin &= 0xFFFFFFF0;

	int range = m_bufferSize / 16 + (m_bufferSize % 16 ? 1 : 0);
	int pageRange = mGetPageRange();

	if (hexBegin < 0)
		m_hexBegin = 0;
	else if ( (range >= pageRange ) &&  (hexBegin >> 4 > range - pageRange))
		m_hexBegin = (range - pageRange) << 4;
	else
		m_hexBegin = hexBegin;

	m_scrollbar_hex.SetScrollPos(m_hexBegin >> 4);

	InvalidateRect(m_hexRect);

	return 0;
}

// 设置高亮显示区域
int CDialogPE::mSetHexBeginColored(int begin, int length)
{
	m_hexBeginWithBkColor = begin;
	m_hexLengthWithBkColor = length;
	mSetHexBegin(begin);
	return 0;
}

// 绘制hex
int CDialogPE::mDrawHex(CPaintDC* pDC)
{
	CBitmap bitmapBuffer;	// 位图对象
	CDC memDC;				// 内存DC
	CRect rect;
	CString str;
	CSize normalFontSize;
	CSize smallFontSize;
	UINT32 lineMargin = 0;

	// 双缓冲 配置
	memDC.CreateCompatibleDC(pDC);			// 创建一个内存DC
	bitmapBuffer.CreateCompatibleBitmap(pDC, m_hexRect.Width(), m_hexRect.Height());		// 创建一个兼容的位图
	memDC.SelectObject(&bitmapBuffer);		// 选择位图到内存DC

	// 字体定义: 大字体显示hex,小字体显示符号
	CFont fontNormal, fontSmall;

	fontNormal.CreateFont(
		m_hexFontSize,             // nHeight - 字体高度
		0,                         // nWidth - 字体宽度(0表示自适应)
		0,                         // nEscapement - 文本角度(0.1度单位)
		0,                         // nOrientation - 字符角度
		FW_NORMAL,                 // nWeight - 字体粗细
		FALSE,                     // bItalic - 是否斜体
		FALSE,                     // bUnderline - 是否下划线
		FALSE,                     // bStrikeOut - 是否删除线
		DEFAULT_CHARSET,           // nCharSet - 字符集
		OUT_DEFAULT_PRECIS,        // nOutPrecision - 输出精度
		CLIP_DEFAULT_PRECIS,       // nClipPrecision - 剪裁精度
		DEFAULT_QUALITY,           // nQuality - 输出质量
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily - 字体系列
		FONT_TYPE                  // lpszFacename - 字体名称
	);

	fontSmall.CreateFont(
		(int)(m_hexFontSize * 0.9), // nHeight - 字体高度
		0,                         // nWidth - 字体宽度(0表示自适应)
		0,                         // nEscapement - 文本角度(0.1度单位)
		0,                         // nOrientation - 字符角度
		FW_NORMAL,                 // nWeight - 字体粗细
		FALSE,                     // bItalic - 是否斜体
		FALSE,                     // bUnderline - 是否下划线
		FALSE,                     // bStrikeOut - 是否删除线
		DEFAULT_CHARSET,           // nCharSet - 字符集
		OUT_DEFAULT_PRECIS,        // nOutPrecision - 输出精度
		CLIP_DEFAULT_PRECIS,       // nClipPrecision - 剪裁精度
		DEFAULT_QUALITY,           // nQuality - 输出质量
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily - 字体系列
		FONT_TYPE                   // lpszFacename - 字体名称
	);

	memDC.SelectObject(&fontSmall);
	smallFontSize = memDC.GetTextExtent(TEXT("0"), 1);
	memDC.SelectObject(&fontNormal);
	normalFontSize = memDC.GetTextExtent(TEXT("0"), 1);
	lineMargin = normalFontSize.cy / 2;

	// 默认背景色
	rect.top = 0;
	rect.left = 0;
	rect.right = m_hexRect.Width();
	rect.bottom = m_hexRect.Height();
	memDC.FillRect(rect, &CBrush(m_defaultBkColor));

	// 默认背景色
	memDC.SetBkColor(m_defaultBkColor);

	// 顶部 [0,F] 绘制
	rect.top = 0;
	rect.left = normalFontSize.cx * LEFT_HEX_ADDR_CHAR_NUM;
	for (int i = 0; i < 16; ++i)
	{
		str.Format(TEXT("%02X"), i);
		rect.left = normalFontSize.cx * LEFT_HEX_ADDR_CHAR_NUM + i * 4 * normalFontSize.cx;
		rect.right = rect.left + 2 * normalFontSize.cx;
		rect.bottom = rect.top + normalFontSize.cy;
		memDC.DrawTextEx(str, &rect, DT_LEFT | DT_VCENTER, NULL);
	}

	// 打印hex
	int n = (m_hexRect.Height() / (normalFontSize.cy + lineMargin)) - 1;  // m_hexRect区域能显示的行数,  要去掉最顶部的[0,F]行


	rect.top = normalFontSize.cy + lineMargin;
	for (int i = 0; i <= n; ++i)
	{
		if (m_hexBegin + i * 16 >= m_bufferSize)
			break;

		memDC.SelectObject(&fontNormal);

		rect.bottom = rect.top + normalFontSize.cy;
		// 地址
		rect.left = 0;
		rect.right = normalFontSize.cx * LEFT_HEX_ADDR_CHAR_NUM;
		str.Format(TEXT("%08X"), m_hexBegin + i * 16);
		memDC.DrawTextEx(str, &rect, DT_LEFT | DT_VCENTER, NULL);

		// hex
		for (int j = 0; j < 16; ++j)
		{
			rect.left = normalFontSize.cx * LEFT_HEX_ADDR_CHAR_NUM + j * 4 * normalFontSize.cx;
			rect.right = rect.left + 2 * normalFontSize.cx;
			str.Format(TEXT("%02X"), (UCHAR) * (m_pBuffer + m_hexBegin + i * 16 + j));

			if (m_hexBeginWithBkColor <= m_hexBegin + i * 16 + j && m_hexBegin + i * 16 + j < m_hexBeginWithBkColor + m_hexLengthWithBkColor)
			{
				COLORREF c = memDC.SetBkColor(m_hexColor);
				memDC.DrawTextEx(str, &rect, DT_LEFT | DT_VCENTER, NULL);
				memDC.SetBkColor(c);
			}
			else
				memDC.DrawTextEx(str, &rect, DT_LEFT | DT_VCENTER, NULL);
		}

		// hex对应的ascii
		memDC.SelectObject(&fontSmall);
		for (int j = 0; j < 16; ++j)
		{
			rect.left = normalFontSize.cx * LEFT_HEX_ADDR_CHAR_NUM + 16 * 4 * normalFontSize.cx + j * smallFontSize.cx;
			rect.right = rect.left + smallFontSize.cx;
			str.Format(TEXT("%c"), *(m_pBuffer + m_hexBegin + i * 16 + j) > 32 ? *(m_pBuffer + m_hexBegin + i * 16 + j) : '.'); // 非打印字符替换为'.'

			if (m_hexBeginWithBkColor <= m_hexBegin + i * 16 + j && m_hexBegin + i * 16 + j < m_hexBeginWithBkColor + m_hexLengthWithBkColor)
			{
				COLORREF c = memDC.SetBkColor(m_hexColor);
				memDC.DrawTextEx(str, &rect, DT_LEFT | DT_BOTTOM | DT_SINGLELINE, NULL);
				memDC.SetBkColor(c);
			}
			else
				memDC.DrawTextEx(str, &rect, DT_LEFT | DT_BOTTOM | DT_SINGLELINE, NULL);
		}

		rect.top += normalFontSize.cy + lineMargin;
	}

	pDC->BitBlt(m_hexRect.left, m_hexRect.top, m_hexRect.Width(), m_hexRect.Height(), &memDC, 0, 0, SRCCOPY);

	return 0;
}

// 设置编辑框文本
int CDialogPE::mSetEditText(CString text)
{
	mSwitchShowFlag(0);
	m_edit_text.SetWindowText(text);
	m_edit_text.UpdateWindow();

	return 0;
}

// 获取基本信息
void CDialogPE::mGetPEBaseInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		// 处理器类型
		switch (pNt32->FileHeader.Machine)
		{
		case 0x8664:
			text += TEXT("x64处理器平台\r\n");
			break;
		case 0x14c:
			text += TEXT("I386或后续兼容处理器平台\r\n");
			break;
		default:
			break;
		}

		// PE属性
		text += (pNt32->OptionalHeader.Magic == 0x10b) ? TEXT("PE32    ") : TEXT("PE32+    ");
		text += (pNt32->FileHeader.Characteristics & 0x2000) ? TEXT("dll \r\n") : TEXT("非dll \r\n");
		text += (pNt32->OptionalHeader.DllCharacteristics & 0x20) ? TEXT("可重定位 ") : TEXT("不可重定位 ");
		text += (pNt32->OptionalHeader.DllCharacteristics & 0x200) ? TEXT("不使用SEH ") : TEXT("使用SEH ");
		text += (pNt32->OptionalHeader.DllCharacteristics & 0x200) ? TEXT("driver \r\n") : TEXT("非driver \r\n");
		text.AppendFormat(TEXT("入口点OPE: 0x%08X\r\n"), pNt32->OptionalHeader.AddressOfEntryPoint);
		text.AppendFormat(TEXT("头大小   : 0x%08X\r\n"), pNt32->OptionalHeader.SizeOfHeaders);
		text.AppendFormat(TEXT("内存对齐 : 0x%08X\r\n"), pNt32->OptionalHeader.SectionAlignment);
		text.AppendFormat(TEXT("文件对齐 : 0x%08X\r\n"), pNt32->OptionalHeader.FileAlignment);
		text.AppendFormat(TEXT("内存大小 : 0x%08X\r\n"), pNt32->OptionalHeader.SizeOfImage);
		text.AppendFormat(TEXT("文件大小 : 0x%08X\r\n"), getPEFileSize(p));

		// 内存基址
		if (isX64(p))
			text.AppendFormat(TEXT("ImageBase: 0x%016I64X\r\n"), pNt64->OptionalHeader.ImageBase);
		else
			text.AppendFormat(TEXT("ImageBase: 0x%08X\r\n"), pNt32->OptionalHeader.ImageBase);

		// 子系统
		switch (pNt32->OptionalHeader.Subsystem)
		{
		case 1:
			text += TEXT("子系统: windows driver\r\n");
			break;
		case 2:
			text += TEXT("子系统: windows GUI\r\n");
			break;
		case 3:
			text += TEXT("子系统: windows CONSOLE\r\n");
			break;
		default:
			break;
		}

		mSetEditText(text);
	}
	catch (...)
	{
		mSetEditText((PTCHAR)TEXT("获取基本信息失败!"));
	}
}

// 获取导出表信息
void CDialogPE::mGetPEexportTableInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		// 没有导出表 直接返回
		if (!pDir[0].VirtualAddress)
		{
			text = TEXT("无导出表");
			mSetEditText(text);
			return;
		}

		IMAGE_EXPORT_DIRECTORY* pExportTable = (IMAGE_EXPORT_DIRECTORY*)(m_isImageBuffer ? (p + pDir[0].VirtualAddress) : RVA_TO_FILE_BUFFER_VA(p, pDir[0].VirtualAddress));
		if (!pExportTable)
		{
			text = TEXT("导出表无效");
			mSetEditText(text);
			return;
		}

		text = TEXT("导出表信息:\r\n");
		text.AppendFormat(TEXT("Characteristics: 0x%08X\r\n"), pExportTable->Characteristics);
		text.AppendFormat(TEXT("TimeDateStamp: 0x%08X\r\n"), pExportTable->TimeDateStamp);
		text.AppendFormat(TEXT("MajorVersion: %d\r\n"), pExportTable->MajorVersion);
		text.AppendFormat(TEXT("MinorVersion: %d\r\n"), pExportTable->MinorVersion);
		text.AppendFormat(TEXT("Name: 0x%08X  "), pExportTable->Name);
		text += (char*)(m_isImageBuffer ? (p + pExportTable->Name) : RVA_TO_FILE_BUFFER_VA(p, pExportTable->Name));
		text += TEXT("\r\n");
		text.AppendFormat(TEXT("Base: 0x%08X\r\n"), pExportTable->Base);
		text.AppendFormat(TEXT("NumberOfFunctions: %d\r\n"), pExportTable->NumberOfFunctions);
		text.AppendFormat(TEXT("NumberOfNames: %d\r\n"), pExportTable->NumberOfNames);
		text.AppendFormat(TEXT("AddressOfFunctions: 0x%08X\r\n"), pExportTable->AddressOfFunctions);
		text.AppendFormat(TEXT("AddressOfNames: 0x%08X\r\n"), pExportTable->AddressOfNames);
		text.AppendFormat(TEXT("AddressOfNameOrdinals: 0x%08X\r\n"), pExportTable->AddressOfNameOrdinals);

		text.AppendFormat(TEXT("\r\n\r\n导出函数列表:\r\n"));

		// 三个子表
		PDWORD pFunctions = (PDWORD)(m_isImageBuffer ? (p + pExportTable->AddressOfFunctions) : RVA_TO_FILE_BUFFER_VA(p, pExportTable->AddressOfFunctions));			// 函数RVA数组
		PWORD pOrdinals = (PWORD)(m_isImageBuffer ? (p + pExportTable->AddressOfNameOrdinals) : RVA_TO_FILE_BUFFER_VA(p, pExportTable->AddressOfNameOrdinals));		// 序号数组
		PDWORD pNames = (PDWORD)(m_isImageBuffer ? (p + pExportTable->AddressOfNames) : RVA_TO_FILE_BUFFER_VA(p, pExportTable->AddressOfNames));						// 函数名RVA数组

		for (DWORD i = 0; i < pExportTable->NumberOfFunctions; ++i)
		{
			if (pFunctions[i] == 0)
				continue;

			text.AppendFormat(TEXT("%04d:  地址:0x%08X"), i, pFunctions[i]);	
			for (DWORD j = 0; j < pExportTable->NumberOfNames; ++j)
			{
				if (pOrdinals[j] == i)
				{
					text.AppendFormat(TEXT("  名称:"));
					text += (char*)(m_isImageBuffer ? (p + pNames[j]) : RVA_TO_FILE_BUFFER_VA(p, pNames[j]));
					break;
				}
			}
			text.AppendFormat(TEXT("\r\n"));
		}

		mSetEditText(text);
	}
	catch (...)
	{
		CString text(TEXT("获取导出表信息失败"));
		mSetEditText(text);
	}
}

// 获取导入表信息
void CDialogPE::mGetPEImportTableInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);
		PUINT64 pThunkData64;
		PUINT32 pThunkData32;

		// 没有导入表 直接返回
		if (!pDir[1].VirtualAddress)
		{
			text = TEXT("无导入表");
			mSetEditText(text);
			return;
		}

		IMAGE_IMPORT_DESCRIPTOR* pImportTable = (IMAGE_IMPORT_DESCRIPTOR*)(m_isImageBuffer ? (p + pDir[1].VirtualAddress) : RVA_TO_FILE_BUFFER_VA(p, pDir[1].VirtualAddress));
		if (!pImportTable)
		{
			text = TEXT("导入表无效");
			mSetEditText(text);
			return;
		}


		while (pImportTable->Name)
		{
			text.AppendFormat(TEXT("\r\n\r\n------------------- 导入表信息: "));
			text += (char*)(m_isImageBuffer ? (p + pImportTable->Name) : RVA_TO_FILE_BUFFER_VA(p, pImportTable->Name));
			text += TEXT("\r\n");
			text.AppendFormat(TEXT("OriginalFirstThunk: 0x%08X\r\n"), pImportTable->OriginalFirstThunk);
			text.AppendFormat(TEXT("TimeDateStamp: 0x%08X\r\n"), pImportTable->TimeDateStamp);
			text.AppendFormat(TEXT("ForwarderChain: 0x%08X\r\n"), pImportTable->ForwarderChain);
			text.AppendFormat(TEXT("Name: 0x%08X \r\n"), pImportTable->Name);
			text.AppendFormat(TEXT("FirstThunk: 0x%08X\r\n"), pImportTable->FirstThunk);

			// IAT表
			if (isX64(p))
			{
				if (pImportTable->OriginalFirstThunk)
				{
					pThunkData64 = (PUINT64)(m_isImageBuffer ? (p + pImportTable->OriginalFirstThunk) : RVA_TO_FILE_BUFFER_VA(p, pImportTable->OriginalFirstThunk));
				}
				else
				{
					if (m_isImageBuffer)
					{
						text = TEXT("导入表无效, 因为内存镜像还没有INT");
						mSetEditText(text);
						return;
					}
					pThunkData64 = (PUINT64)(m_isImageBuffer ? (p + pImportTable->FirstThunk) : RVA_TO_FILE_BUFFER_VA(p, pImportTable->FirstThunk)); // IAT表
				}

				// 以0结尾结束
				while (*pThunkData64)
				{
					if (*pThunkData64 & (1LL << 63LL))
					{
						text.AppendFormat(TEXT("序号导入: %d \r\n"), (int)(*pThunkData64 & 0xFFFF));
					}
					else
					{
						text.AppendFormat(TEXT("名称导入:"));
						text += (char*)(m_isImageBuffer ? (p + *pThunkData64) : RVA_TO_FILE_BUFFER_VA(p, *pThunkData64)) + sizeof(WORD); // 有2字节多余.
						text += TEXT("\r\n");
					}

					pThunkData64++;
				}
			}
			else
			{
				if (pImportTable->OriginalFirstThunk)
				{
					pThunkData32 = (PUINT32)(m_isImageBuffer ? (p + pImportTable->OriginalFirstThunk) : RVA_TO_FILE_BUFFER_VA(p, pImportTable->OriginalFirstThunk));
				}
				else
				{
					if (m_isImageBuffer)
					{
						text = TEXT("导入表无效, 内存镜像 还没有INT");
						mSetEditText(text);
						return;
					}
					pThunkData32 = (PUINT32)(m_isImageBuffer ? (p + pImportTable->FirstThunk) : RVA_TO_FILE_BUFFER_VA(p, pImportTable->FirstThunk)); // IAT表
				}

				while (*pThunkData32)
				{
					if (*pThunkData32 & (1LL << 31LL))
					{
						text.AppendFormat(TEXT("序号导入: %d \r\n"), (int)(*pThunkData32 & 0xFFFF));
					}
					else
					{
						text.AppendFormat(TEXT("名称导入:"));
						text += (char*)(m_isImageBuffer ? (p + *pThunkData32) : RVA_TO_FILE_BUFFER_VA(p, *pThunkData32)) + sizeof(WORD);
						text += TEXT("\r\n");
					}

					pThunkData32++;
				}
			}

			pImportTable++;
		}

		mSetEditText(text);
	}
	catch (...)
	{
		CString text(TEXT("获取导入表信息失败"));
		mSetEditText(text);
	}
}

// 遍历资源树
void CDialogPE::mEnumResourceTree(int deep, CStringW& str, IMAGE_RESOURCE_DIRECTORY* root, PCHAR baseAddr)
{
	IMAGE_RESOURCE_DIRECTORY* pTable = root;
	CStringW strTab;

	for (int i = 0; i < pTable->NumberOfIdEntries + pTable->NumberOfNamedEntries; i++)
	{
		PIMAGE_RESOURCE_DIRECTORY_ENTRY pEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pTable + 1) + i;
		str += strTab;
		if (pEntry->NameIsString)
		{
			PIMAGE_RESOURCE_DIR_STRING_U pName = (PIMAGE_RESOURCE_DIR_STRING_U)(baseAddr + pEntry->NameOffset);
			if (deep == 0)
				str += L"类型:";
			if (deep == 1)
				str += L"\t编号:";
			if (deep == 2)
				str += L"\t\t代码页:";
			CStringW tmp(pName->NameString, pName->Length);
			str += tmp;
			str += L"\r\n";
		}
		else
		{
			if (deep == 0)
				str += L"类型number:";
			if (deep == 1)
				str += L"\t编号number:";
			if (deep == 2)
				str += L"\t\t代码页number:";
			str.AppendFormat(L"%04X\r\n", pEntry->Id);
		}

		if (deep < 2)
			mEnumResourceTree(deep + 1, str, (IMAGE_RESOURCE_DIRECTORY*)(baseAddr + pEntry->OffsetToDirectory), baseAddr);
		else
		{
			IMAGE_RESOURCE_DATA_ENTRY* pDataEntry = (IMAGE_RESOURCE_DATA_ENTRY*)(baseAddr + pEntry->OffsetToData);
			str += L"\t\t\t数据偏移: \t";
			str.AppendFormat(L"Offset:%08X\r\n", pDataEntry->OffsetToData);
			str += strTab;
			str += L"\t\t\t数据大小: \t";
			str.AppendFormat(L"Size:%08X\r\n", pDataEntry->Size);
			str += strTab;
			str += L"\t\t\t代码页:\t\t";
			str.AppendFormat(L"CodePage:%08X\r\n", pDataEntry->CodePage);
		}
	}
}

// 获取PE资源表信息
void CDialogPE::mGetPEResourceTableInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		// 没有导出表 直接返回
		if (!pDir[2].VirtualAddress)
		{
			text = TEXT("无资源表");
			mSetEditText(text);
			return;
		}

		IMAGE_RESOURCE_DIRECTORY* pResourceTable = (IMAGE_RESOURCE_DIRECTORY*)(m_isImageBuffer ? (p + pDir[2].VirtualAddress) : RVA_TO_FILE_BUFFER_VA(p, pDir[2].VirtualAddress));
		if (!pResourceTable)
		{
			text = TEXT("资源表无效");
			mSetEditText(text);
			return;
		}

		CStringW strW;
		mEnumResourceTree(0, strW, pResourceTable,(PCHAR)pResourceTable);
		text = strW;


		mSetEditText(text);
	}
	catch (...)
	{
		CString text(TEXT("获取资源表信息失败"));
		mSetEditText(text);
	}
}

// 获取PE异常表信息
void CDialogPE::mGetPESEHTableInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		// 没有导出表 直接返回
		if (!pDir[3].VirtualAddress)
		{
			text = TEXT("无异常表");
			mSetEditText(text);
			return;
		}

		IMAGE_RUNTIME_FUNCTION_ENTRY* pRunTimeFuncEntry= (IMAGE_RUNTIME_FUNCTION_ENTRY*)(m_isImageBuffer ? (p + pDir[3].VirtualAddress) : RVA_TO_FILE_BUFFER_VA(p, pDir[3].VirtualAddress));
		if (!pRunTimeFuncEntry)
		{
			text = TEXT("异常表无效");
			mSetEditText(text);
			return;
		}

#if 0
		typedef struct _IMAGE_RUNTIME_FUNCTION_ENTRY {
			DWORD BeginAddress;     // 函数起始RVA（相对于映像基址),  		数组是按BeginAddress排序的.便于二分查找.
			DWORD EndAddress;       // 函数结束RVA（BeginAddress <= EndAddress）
			DWORD UnwindInfoAddress; // 展开信息结构的RVA
		} IMAGE_RUNTIME_FUNCTION_ENTRY, * PIMAGE_RUNTIME_FUNCTION_ENTRY;
#endif

		typedef union _UNWIND_CODE {
			struct {
				UCHAR CodeOffset;   // 距序言结束的偏移（指令字节）
				UCHAR UnwindOp : 4; // 展开操作码
				/*
					UWOP_PUSH_NONVOL (0)：压入非易失寄存器
					UWOP_ALLOC_SMALL (1)：栈上分配小空间（8-128字节）
					UWOP_ALLOC_LARGE (2)：分配大空间
					UWOP_SET_FPREG   (3)：设置帧指针
					UWOP_SAVE_NONVOL (4)：保存非易失寄存器到栈
					UWOP_SAVE_XMM128 (8)：保存XMM寄存器
				*/
				UCHAR OpInfo : 4;   // 操作信息
			};
			USHORT FrameOffset;     // 用于UWOP_ALLOC_LARGE等
		} UNWIND_CODE;

		// 展开信息, 是异常表的核心.描述异常函数的展开逻辑.
		typedef struct _UNWIND_INFO {
			UCHAR Version : 3;      // 版本（通常为1）
			UCHAR Flags : 5;        // 标志（UNW_FLAG_EHANDLER等）
			/*
			    UNW_FLAG_EHANDLER (0x01)：函数有异常处理程序
				UNW_FLAG_UHANDLER (0x02)：函数有终止处理程序
				UNW_FLAG_CHAININFO (0x04)：此展开信息是链信息，指向另一个函数条目 : 
									// 如果UNW_FLAG_CHAININFO被设置，则ExceptionHandler字段指向另一个IMAGE_RUNTIME_FUNCTION_ENTRY结构
									//（表示链中的下一个函数）。这用于优化相邻函数具有相同展开信息的情况。
			*/
			UCHAR SizeOfProlog;     // 序言字节数
			UCHAR CountOfCodes;     // 展开代码条目数
			UCHAR FrameRegister : 4;// 帧寄存器（如果使用帧指针）
			UCHAR FrameOffset : 4;  // 帧偏移（缩放倍数）
			// 展开代码数组（可变长，每个UNWIND_CODE占2字节）
			_UNWIND_CODE UnwindCode[1];		// UnwindCodes 数组中的每个元素描述栈操作（如 push、sub rsp 等），用于异常时反向恢复栈：
				// 可选：异常处理程序RVA或函数指针
			/*
			........ N个UNWIND_CODE条目，描述了如何从异常恢复栈。
			*/
			DWORD ExceptionHandler;		// 可选
										// Flags包含UNW_FLAG_EHANDLER(0x01),这个函数就是异常处理程序.
				// Flags包含UNW_FLAG_UHANDLER(0x02),这个函数就是终止处理程序.
			DWORD ExceptionData;		// 可选 :  异常处理程序数据（变长，由异常处理程序解释）
		} UNWIND_INFO, * PUNWIND_INFO;

		for (DWORD i = 0; i < pDir[3].Size / sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY); i++)
		{
			text.AppendFormat(TEXT("\r\nBeginAddress:%08X\r\n"), pRunTimeFuncEntry[i].BeginAddress);		// 函数开始地址
			text.AppendFormat(TEXT("EndAddress:%08X\r\n"), pRunTimeFuncEntry[i].EndAddress);				// 函数结束地址
			text.AppendFormat(TEXT("UnwindInfoAddress:%08X\r\n"), pRunTimeFuncEntry[i].UnwindData);			// 展开信息

			PUNWIND_INFO pUnwindInfo = (PUNWIND_INFO)(m_isImageBuffer ? (p + pRunTimeFuncEntry[i].UnwindData) : RVA_TO_FILE_BUFFER_VA(p, pRunTimeFuncEntry[i].UnwindData));
			if (pUnwindInfo)
			{
				text.AppendFormat(TEXT("\tUnwindInfo:\r\n"));
				text.AppendFormat(TEXT("\t\tVersion:%02X\r\n"), pUnwindInfo->Version);
				text.AppendFormat(TEXT("\t\tFlags:%02X"), pUnwindInfo->Flags);
				switch (pUnwindInfo->Flags)
				{
					case 0x01:
						text.AppendFormat(TEXT("\tUNW_FLAG_EHANDLER\r\n"));
						break;
					case 0x02:
						text.AppendFormat(TEXT("\tUNW_FLAG_UHANDLER\r\n"));
						break;
					case 0x03:
						text.AppendFormat(TEXT("\tUNW_FLAG_EHANDLER | UNW_FLAG_UHANDLER\r\n"));
						break;
					default:
						break;
				}
				text.AppendFormat(TEXT("\t\tSizeOfProlog:%02X\r\n"), pUnwindInfo->SizeOfProlog);
				text.AppendFormat(TEXT("\t\tCountOfCodes:%02X\r\n"), pUnwindInfo->CountOfCodes);
				text.AppendFormat(TEXT("\t\tFrameRegister:%02X\r\n"), pUnwindInfo->FrameRegister);
				text.AppendFormat(TEXT("\t\tFrameOffset:%02X\r\n"), pUnwindInfo->FrameOffset);
				for (int j = 0; j < pUnwindInfo->CountOfCodes; j++)
				{
					text.AppendFormat(TEXT("\t\tCodeOffset:%02X  UnwindOp:%02X OpInfo:%02X\r\n"), pUnwindInfo->UnwindCode[j].CodeOffset, 
						pUnwindInfo->UnwindCode[j].UnwindOp, pUnwindInfo->UnwindCode[j].OpInfo);
				}
				if (pUnwindInfo->Flags & 0x01 )
					text.AppendFormat(TEXT("\t\t异常处理程序RVA:%08X\r\n"), pUnwindInfo->ExceptionHandler);
				if (pUnwindInfo->Flags & 0x02)
					text.AppendFormat(TEXT("\t\t终止处理程序RVA:%08X\r\n"), pUnwindInfo->ExceptionHandler);
			}
			text+=TEXT("\r\n");
		}

		mSetEditText(text);
	}
	catch (...)
	{
		CString text(TEXT("获取异常表信息失败"));
		mSetEditText(text);
	}
}

// 获取PE证书表信息
void CDialogPE::mGetPECertificateTableInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		// 没有导出表 直接返回
		if (!pDir[4].VirtualAddress)
		{
			text = TEXT("无安全证书表");
			mSetEditText(text);
			return;
		}

		if (m_isImageBuffer)
		{
			text = TEXT("内存映像不支持查看安全证书表");
			mSetEditText(text);
			return;
		}

		// 安全证书主结构
		typedef struct _WIN_CERTIFICATE {
			DWORD   dwLength;          // 整个证书块的总长度（含本结构体+PKCS#7数据）
			WORD    wRevision;         // 证书版本（固定为0x0200，对应PKCS#7 v1.5）
			WORD    wCertificateType; // 证书类型（0x0002=PKCS#7签名）
			BYTE    bCertificate[1];    // 可变长度：PKCS#7/CMS格式的签名数据
		} WIN_CERTIFICATE, * PWIN_CERTIFICATE;


		// 安全证书的偏移是FOA而不是RVA
		WIN_CERTIFICATE* pCertifcateTable = (WIN_CERTIFICATE*)(p + pDir[4].VirtualAddress);
		if (!pCertifcateTable)
		{
			text = TEXT("安全证书表无效");
			mSetEditText(text);
			return;
		}

		text.AppendFormat(TEXT("dwLength:%08X\r\n"), pCertifcateTable->dwLength);
		text.AppendFormat(TEXT("wRevision:%04X\r\n"), pCertifcateTable->wRevision);
		text.AppendFormat(TEXT("wCertificateType:%04X\r\n"), pCertifcateTable->wCertificateType);
		for (DWORD i = 0; i < pCertifcateTable->dwLength; i++)
		{
			text.AppendFormat(TEXT("%02X "), pCertifcateTable->bCertificate[i]);
			if ((i + 1) % 16 == 0)
				text.AppendFormat(TEXT("\r\n"));
		}

		mSetEditText(text);
	}
	catch (...)
	{
		CString text(TEXT("获取资源表信息失败"));
		mSetEditText(text);
	}


}

// 获取PE基址重定位表信息
void CDialogPE::mGetPEBaseRelocationTableInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		// 没有导出表 直接返回
		if (!pDir[5].VirtualAddress)
		{
			text = TEXT("无重定位表");
			mSetEditText(text);
			return;
		}

		IMAGE_BASE_RELOCATION* pBaseRelocationTable = (IMAGE_BASE_RELOCATION*)(m_isImageBuffer ? (p + pDir[5].VirtualAddress) : RVA_TO_FILE_BUFFER_VA(p, pDir[5].VirtualAddress));
		if (!pBaseRelocationTable)
		{
			text = TEXT("重定位表无效");
			mSetEditText(text);
			return;
		}

		UINT16* pRelocationData;
		while (pBaseRelocationTable->VirtualAddress && !(pBaseRelocationTable->VirtualAddress & 0xFFF)/*ntosknl.exe有问题,不是0结尾,判断是否对齐来结尾*/)
		{
			text.AppendFormat(TEXT("\r\nVirtualAddress:%08X\r\n"), pBaseRelocationTable->VirtualAddress);
			text.AppendFormat(TEXT("SizeOfBlock:%08X\r\n"), pBaseRelocationTable->SizeOfBlock);
			pRelocationData = (UINT16*)((BYTE*)pBaseRelocationTable + sizeof(IMAGE_BASE_RELOCATION));
			for (UINT32 i = 0; i < ((pBaseRelocationTable->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(UINT16)); i++)
			{
				text.AppendFormat(TEXT("\tType:%04X  Offset:%04X\r\n"), pRelocationData[i] >> 12, pRelocationData[i] & 0x0FFF);
			}

			pBaseRelocationTable = (IMAGE_BASE_RELOCATION*)((BYTE*)pBaseRelocationTable + pBaseRelocationTable->SizeOfBlock);
		}

		mSetEditText(text);
	}
	catch (...)
	{
		CString text(TEXT("获取重定位表信息失败"));
		mSetEditText(text);
	}
}

// 获取PE调试信息表信息
void CDialogPE::mGetPEDebugTableInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		// 没有导出表 直接返回
		if (!pDir[6].VirtualAddress)
		{
			text = TEXT("无调试信息表");
			mSetEditText(text);
			return;
		}

		IMAGE_DEBUG_DIRECTORY* pDebugTable = (IMAGE_DEBUG_DIRECTORY*)(m_isImageBuffer ? (p + pDir[6].VirtualAddress) : RVA_TO_FILE_BUFFER_VA(p, pDir[6].VirtualAddress));
		if (!pDebugTable)
		{
			text = TEXT("调试信息表无效");
			mSetEditText(text);
			return;
		}

		for (UINT32 i = 0; i < pDir[6].Size / sizeof(IMAGE_DEBUG_DIRECTORY); i++)
		{
			text.AppendFormat(TEXT("Characteristics:%08X\r\n"), pDebugTable[i].Characteristics);
			text.AppendFormat(TEXT("TimeDateStamp:%08X\r\n"), pDebugTable[i].TimeDateStamp);
			text.AppendFormat(TEXT("MajorVersion:%04X\r\n"), pDebugTable[i].MajorVersion);
			text.AppendFormat(TEXT("MinorVersion:%04X\r\n"), pDebugTable[i].MinorVersion);
			text.AppendFormat(TEXT("Type:%04X\r\n"), pDebugTable[i].Type);
			text.AppendFormat(TEXT("SizeOfData:%08X\r\n"), pDebugTable[i].SizeOfData);
			text.AppendFormat(TEXT("AddressOfRawData:%08X\r\n"), pDebugTable[i].AddressOfRawData);
			text.AppendFormat(TEXT("PointerToRawData:%08X\r\n"), pDebugTable[i].PointerToRawData);
		}

		mSetEditText(text);
	}
	catch (...)
	{
		CString text(TEXT("获取调试信息失败"));
		mSetEditText(text);
	}

}

// 获取PE架构信息表信息
void CDialogPE::mGetPEArchitectureTableInfo()
{
	CString text(TEXT("架构信息表未使用"));
	mSetEditText(text);
}

// 获取PE全局指针表信息
void CDialogPE::mGetPEGlobalPTRTableInfo()
{
	CString text(TEXT("全局指针表未使用"));
	mSetEditText(text);
}

// 获取PETLS表信息
void CDialogPE::mGetTLSTableInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		// 没有导出表 直接返回
		if (!pDir[9].VirtualAddress)
		{
			text = TEXT("无TLS表");
			mSetEditText(text);
			return;
		}

		if (isX64(p))
		{
			IMAGE_TLS_DIRECTORY64* pTLSTable64 = (IMAGE_TLS_DIRECTORY64*)(m_isImageBuffer ? (p + pDir[9].VirtualAddress) : RVA_TO_FILE_BUFFER_VA(p, pDir[9].VirtualAddress));
			if (!pTLSTable64)
			{
				text = TEXT("TLS表无效");
				mSetEditText(text);
				return;
			}
			text.AppendFormat(TEXT("StartAddressOfRawData:%016I64X\r\n"), pTLSTable64->StartAddressOfRawData);
			text.AppendFormat(TEXT("EndAddressOfRawData:%016I64X\r\n"), pTLSTable64->EndAddressOfRawData);
			text.AppendFormat(TEXT("AddressOfIndex:%016I64X\r\n"), pTLSTable64->AddressOfIndex);
			text.AppendFormat(TEXT("AddressOfCallBacks:%016I64X\r\n"), pTLSTable64->AddressOfCallBacks);
			text.AppendFormat(TEXT("SizeOfZeroFill:%08X\r\n"), pTLSTable64->SizeOfZeroFill);
			text.AppendFormat(TEXT("Characteristics:%08X\r\n"), pTLSTable64->Characteristics);
			UINT32 pCallBacksRVA = (UINT32)(pTLSTable64->AddressOfCallBacks - pNt64->OptionalHeader.ImageBase);	// 获取RVA
			PUINT64 pCallBacks;

			if (m_isImageBuffer)
				pCallBacks = (PUINT64)(p + pCallBacksRVA);
			else
				pCallBacks = (PUINT64)(p + rvaToFoa(p, pCallBacksRVA));
			while (*pCallBacks)
			{
				text.AppendFormat(TEXT("\t%016I64X\r\n"), *pCallBacks);
				pCallBacks++;
			}
		}
		else
		{
			IMAGE_TLS_DIRECTORY32* pTLSTable32 = (IMAGE_TLS_DIRECTORY32*)(m_isImageBuffer ? (p + pDir[9].VirtualAddress) : RVA_TO_FILE_BUFFER_VA(p, pDir[9].VirtualAddress));
			if (!pTLSTable32)
			{
				text = TEXT("TLS表无效");
				mSetEditText(text);
				return;
			}
			text.AppendFormat(TEXT("StartAddressOfRawData:%08X\r\n"), pTLSTable32->StartAddressOfRawData);
			text.AppendFormat(TEXT("EndAddressOfRawData:%08X\r\n"), pTLSTable32->EndAddressOfRawData);
			text.AppendFormat(TEXT("AddressOfIndex:%08X\r\n"), pTLSTable32->AddressOfIndex);
			text.AppendFormat(TEXT("AddressOfCallBacks:%08X\r\n"), pTLSTable32->AddressOfCallBacks);
			text.AppendFormat(TEXT("SizeOfZeroFill:%08X\r\n"), pTLSTable32->SizeOfZeroFill);
			text.AppendFormat(TEXT("Characteristics:%08X\r\n"), pTLSTable32->Characteristics);
			UINT32 pCallBacksRVA = (UINT32)(pTLSTable32->AddressOfCallBacks - pNt32->OptionalHeader.ImageBase);	// 获取RVA
			PUINT32 pCallBacks;
			if (m_isImageBuffer)
				pCallBacks = (PUINT32)(p + pCallBacksRVA);
			else
				pCallBacks = (PUINT32)(p + rvaToFoa(p, pCallBacksRVA));
			while (*pCallBacks)
			{
				text.AppendFormat(TEXT("\t%08X\r\n"), *pCallBacks);		
				pCallBacks++;
			}
		}

		mSetEditText(text);
	}
	catch (...)
	{
		CString text(TEXT("获取TLS表信息失败"));
		mSetEditText(text);
	}

}

// 获取PE加载配置表信息
void CDialogPE::mGetPELoadConfigTableInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		// 没有导出表 直接返回
		if (!pDir[10].VirtualAddress)
		{
			text = TEXT("无加载配置表");
			mSetEditText(text);
			return;
		}

		if (isX64(p))
		{
			IMAGE_LOAD_CONFIG_DIRECTORY64* pLoadConfigTable = (IMAGE_LOAD_CONFIG_DIRECTORY64*)(m_isImageBuffer ? (p + pDir[10].VirtualAddress) : RVA_TO_FILE_BUFFER_VA(p, pDir[10].VirtualAddress));
			if (!pLoadConfigTable)
			{
				text = TEXT("加载配置表无效");
				mSetEditText(text);
				return;
			}

			text.AppendFormat(TEXT("SecurityCookie:%016I64X\r\n"), pLoadConfigTable->SecurityCookie);
			text.AppendFormat(TEXT("SEHandlerTable:%016I64X\r\n"), pLoadConfigTable->SEHandlerTable);
			text.AppendFormat(TEXT("SEHandlerCount:%016I64X\r\n"), pLoadConfigTable->SEHandlerCount);

			PUINT32 pSEHFuncRVA = (PUINT32)(m_isImageBuffer ? (p + pLoadConfigTable->SEHandlerTable) : RVA_TO_FILE_BUFFER_VA(p, pLoadConfigTable->SEHandlerTable));
			for (UINT32 i = 0; i < pLoadConfigTable->SEHandlerCount; ++i)
				text.AppendFormat(TEXT("SEHandler:%08X\r\n"), pSEHFuncRVA[i]);
		}
		else
		{
			IMAGE_LOAD_CONFIG_DIRECTORY32* pLoadConfigTable = (IMAGE_LOAD_CONFIG_DIRECTORY32*)(m_isImageBuffer ? (p + pDir[10].VirtualAddress) : RVA_TO_FILE_BUFFER_VA(p, pDir[10].VirtualAddress));
			if (!pLoadConfigTable)
			{
				text = TEXT("加载配置表无效");
				mSetEditText(text);
				return;
			}

			text.AppendFormat(TEXT("SecurityCookie:%08X\r\n"), pLoadConfigTable->SecurityCookie);
			text.AppendFormat(TEXT("SEHandlerTable:%08X\r\n"), pLoadConfigTable->SEHandlerTable);
			text.AppendFormat(TEXT("SEHandlerCount:%08X\r\n"), pLoadConfigTable->SEHandlerCount);

			UINT32 SEHFuncRVA = (UINT32)( pLoadConfigTable->SEHandlerTable - pNt32->OptionalHeader.ImageBase);	// 获取RVA
			PUINT32 pSEHFunc;
			if (m_isImageBuffer)
				pSEHFunc = (PUINT32)(p + SEHFuncRVA);
			else
				pSEHFunc = (PUINT32)( RVA_TO_FILE_BUFFER_VA(p, SEHFuncRVA));

			for (UINT32 i = 0; i < pLoadConfigTable->SEHandlerCount; ++i)
				text.AppendFormat(TEXT("SEHandler:%08X\r\n"), pSEHFunc[i]);
		}

		mSetEditText(text);
	}
	catch (...)
	{
		CString text(TEXT("获取加载配置表信息失败"));
		mSetEditText(text);
	}

}

// 获取PE绑定导入表信息
void CDialogPE::mGetPEBoundImportTableInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		// 没有导出表 直接返回
		if (!pDir[11].VirtualAddress)
		{
			text = TEXT("无绑定导入表");
			mSetEditText(text);
			return;
		}

		IMAGE_BOUND_IMPORT_DESCRIPTOR* pBoundImportTable = (IMAGE_BOUND_IMPORT_DESCRIPTOR*)(m_isImageBuffer ? (p + pDir[11].VirtualAddress) : RVA_TO_FILE_BUFFER_VA(p, pDir[11].VirtualAddress));
		PCHAR pBase = (PCHAR)pBoundImportTable;
		if (!pBoundImportTable)
		{
			text = TEXT("绑定导入表无效");
			mSetEditText(text);
			return;
		}

		while (pBoundImportTable->OffsetModuleName)
		{
			text.AppendFormat(TEXT("\r\nOffsetModuleName:%08X   "), pBoundImportTable->OffsetModuleName);
			text += (PCHAR)(pBase + pBoundImportTable->OffsetModuleName);
			text += TEXT("\r\n");
			text.AppendFormat(TEXT("TimeDateStamp:%08X\r\n"), pBoundImportTable->TimeDateStamp);
			text.AppendFormat(TEXT("NumberOfModuleForwarderRefs:%08X\r\n"), pBoundImportTable->NumberOfModuleForwarderRefs);
			if (pBoundImportTable->NumberOfModuleForwarderRefs)
			{
				IMAGE_BOUND_FORWARDER_REF* pForwarderRef = (IMAGE_BOUND_FORWARDER_REF*)((PCHAR)pBoundImportTable + sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR));
				for (UINT32 i = 0; i < pBoundImportTable->NumberOfModuleForwarderRefs; ++i)
				{
					text.AppendFormat(TEXT("\tOffsetModuleNameForwarded:%08X   "), pForwarderRef[i].OffsetModuleName);
					text += (PCHAR)(pBase + pForwarderRef[i].OffsetModuleName);
					text += TEXT("\r\n");
					text.AppendFormat(TEXT("\tTimeDateStamp:%08X\r\n"), pForwarderRef[i].TimeDateStamp);
					++pForwarderRef;
				}

				pBoundImportTable = (IMAGE_BOUND_IMPORT_DESCRIPTOR*)((PCHAR)pForwarderRef + sizeof(IMAGE_BOUND_FORWARDER_REF) * pBoundImportTable->NumberOfModuleForwarderRefs);
			}
			else
				pBoundImportTable = (IMAGE_BOUND_IMPORT_DESCRIPTOR*)((PCHAR)pBoundImportTable + sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR) + pBoundImportTable->NumberOfModuleForwarderRefs * sizeof(IMAGE_BOUND_FORWARDER_REF));
		}

		mSetEditText(text);
	}
	catch (...)
	{
		CString text(TEXT("获取绑定导入表信息失败"));
		mSetEditText(text);
	}

}

// 获取PE IAT 表信息
void CDialogPE::mGetPEIATTableInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		// 没有导出表 直接返回
		if (!pDir[12].VirtualAddress)
		{
			text = TEXT("无IAT表");
			mSetEditText(text);
			return;
		}

		text = TEXT("IAT表不适合打印");

		mSetEditText(text);
	}
	catch (...)
	{
		CString text(TEXT("获取IAT表信息失败"));
		mSetEditText(text);
	}

}

// 获取PE延迟导入表信息
void CDialogPE::mGetPEDelayLoadImportTableInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		// 没有导出表 直接返回
		if (!pDir[13].VirtualAddress)
		{
			text = TEXT("无延迟导入表");
			mSetEditText(text);
			return;
		}

		IMAGE_DELAYLOAD_DESCRIPTOR* pDelayLoadImportTable = (IMAGE_DELAYLOAD_DESCRIPTOR*)(m_isImageBuffer ? (p + pDir[13].VirtualAddress) : RVA_TO_FILE_BUFFER_VA(p, pDir[13].VirtualAddress));
		if (!pDelayLoadImportTable)
		{
			text = TEXT("延迟导入表无效");
			mSetEditText(text);
			return;
		}

		while (pDelayLoadImportTable->DllNameRVA)
		{	
			text.AppendFormat(TEXT("\r\nAttributes.AllAttributes:%08X\r\n"), pDelayLoadImportTable->Attributes.AllAttributes);
			text.AppendFormat(TEXT("DllNameRVA:%08X   "), pDelayLoadImportTable->DllNameRVA);
			text += (PCHAR)( m_isImageBuffer ? (p + pDelayLoadImportTable->DllNameRVA) : RVA_TO_FILE_BUFFER_VA(p, pDelayLoadImportTable->DllNameRVA));
			text += TEXT("\r\n");
			text.AppendFormat(TEXT("ModuleHandleRVA:%08X\r\n"), pDelayLoadImportTable->ModuleHandleRVA);
			text.AppendFormat(TEXT("ImportAddressTableRVA:%08X\r\n"), pDelayLoadImportTable->ImportAddressTableRVA);
			text.AppendFormat(TEXT("ImportNameTableRVA:%08X\r\n"), pDelayLoadImportTable->ImportNameTableRVA);
			text.AppendFormat(TEXT("BoundImportAddressTableRVA:%08X\r\n"), pDelayLoadImportTable->BoundImportAddressTableRVA);
			text.AppendFormat(TEXT("UnloadInformationTableRVA:%08X\r\n"), pDelayLoadImportTable->UnloadInformationTableRVA);
			text.AppendFormat(TEXT("TimeDateStamp:%08X\r\n"), pDelayLoadImportTable->TimeDateStamp);

			if (isX64(p))
			{
				PUINT64 pThunk64INT = (PUINT64)( m_isImageBuffer ? (p + pDelayLoadImportTable->ImportNameTableRVA) : RVA_TO_FILE_BUFFER_VA(p, pDelayLoadImportTable->ImportNameTableRVA));
				PUINT64 pThunk64IAT = (PUINT64)( m_isImageBuffer ? (p + pDelayLoadImportTable->ImportAddressTableRVA) : RVA_TO_FILE_BUFFER_VA(p, pDelayLoadImportTable->ImportAddressTableRVA));
				while (*pThunk64INT)
				{
					text.AppendFormat(TEXT("\r\n\tImportName:%016I64X\r\n"), *pThunk64INT);
					text.AppendFormat(TEXT("\tImportAddress:%016I64X\r\n"), *pThunk64IAT);
					if (*pThunk64INT & (1LL << 63LL))
						text.AppendFormat(TEXT("\t序号导入:%016I64X\r\n"), *pThunk64INT & 0xFFFF);
					else
					{
						IMAGE_IMPORT_BY_NAME* pByName = (IMAGE_IMPORT_BY_NAME*)( m_isImageBuffer ? (p + (*pThunk64INT)) : RVA_TO_FILE_BUFFER_VA(p, (UINT32)*pThunk64INT));
						text += TEXT("\t名字导入:\t");
						text += (PCHAR)(pByName->Name);
						text += TEXT("\r\n");
					}
					++pThunk64INT;
					++pThunk64IAT;
				}

			}
			else
			{
				PUINT32 pThunkINT = (PUINT32)( m_isImageBuffer ? (p + pDelayLoadImportTable->ImportNameTableRVA) : RVA_TO_FILE_BUFFER_VA(p, pDelayLoadImportTable->ImportNameTableRVA));
				PUINT32 pThunkIAT = (PUINT32)( m_isImageBuffer ? (p + pDelayLoadImportTable->ImportAddressTableRVA) : RVA_TO_FILE_BUFFER_VA(p, pDelayLoadImportTable->ImportAddressTableRVA));
				while (*pThunkINT)
				{
					text.AppendFormat(TEXT("\r\n\tImportName:%08X\r\n"), *pThunkINT);
					text.AppendFormat(TEXT("\tImportAddress:%08X\r\n"), *pThunkIAT);
					if (*pThunkINT & (1 << 31))
						text.AppendFormat(TEXT("\t序号导入:%08X\r\n"), *pThunkINT & 0xFFFF);
					else
					{
						IMAGE_IMPORT_BY_NAME* pByName = (IMAGE_IMPORT_BY_NAME*)( m_isImageBuffer ? (p + (*pThunkINT)) : RVA_TO_FILE_BUFFER_VA(p, *pThunkINT));
						text += TEXT("\t名字导入:\t");
						text += (PCHAR)(pByName->Name);
						text += TEXT("\r\n");
					}
					++pThunkINT;
					++pThunkIAT;
				}
			}
			pDelayLoadImportTable++;
		}

		mSetEditText(text);
	}
	catch (...)
	{
		CString text(TEXT("获取延迟导入表信息失败"));
		mSetEditText(text);
	}
}

// 获取PE COM 表信息
void CDialogPE::mGetPEComTableInfo()
{
	try
	{
		CString text;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		// 没有导出表 直接返回
		if (!pDir[14].VirtualAddress)
		{
			text = TEXT("无COM表");
			mSetEditText(text);
			return;
		}

		IMAGE_COR20_HEADER* pCOMTable = (IMAGE_COR20_HEADER*)(m_isImageBuffer ? (p + pDir[14].VirtualAddress) : RVA_TO_FILE_BUFFER_VA(p, pDir[14].VirtualAddress));
		if (!pCOMTable)
		{
			text = TEXT("COM表无效");
			mSetEditText(text);
			return;
		}

		text.AppendFormat(TEXT("cb:%08X\r\n"), pCOMTable->cb);
		text.AppendFormat(TEXT("MajorRuntimeVersion:%08X\r\n"), pCOMTable->MajorRuntimeVersion);
		text.AppendFormat(TEXT("MinorRuntimeVersion:%08X\r\n"), pCOMTable->MinorRuntimeVersion);
		text.AppendFormat(TEXT("MetaDataRVA:%08X\r\n"), pCOMTable->MetaData.VirtualAddress);
		text.AppendFormat(TEXT("MetaDataSize:%08X\r\n"), pCOMTable->MetaData.Size);
		text.AppendFormat(TEXT("Flags:%08X\r\n"), pCOMTable->Flags);
		text.AppendFormat(TEXT("EntryPointToken:%08X\r\n"), pCOMTable->EntryPointToken);
		text.AppendFormat(TEXT("ResourcesRVA:%08X\r\n"), pCOMTable->Resources.VirtualAddress);
		text.AppendFormat(TEXT("ResourcesSize:%08X\r\n"), pCOMTable->Resources.Size);
		text.AppendFormat(TEXT("StrongNameSignatureRVA:%08X\r\n"), pCOMTable->StrongNameSignature.VirtualAddress);
		text.AppendFormat(TEXT("StrongNameSignatureSize:%08X\r\n"), pCOMTable->StrongNameSignature.Size);
		text.AppendFormat(TEXT("CodeManagerTableRVA:%08X\r\n"), pCOMTable->CodeManagerTable.VirtualAddress);
		text.AppendFormat(TEXT("CodeManagerTableSize:%08X\r\n"), pCOMTable->CodeManagerTable.Size);
		text.AppendFormat(TEXT("VTableFixupsRVA:%08X\r\n"), pCOMTable->VTableFixups.VirtualAddress);
		text.AppendFormat(TEXT("VTableFixupsSize:%08X\r\n"), pCOMTable->VTableFixups.Size);
		text.AppendFormat(TEXT("ExportAddressTableJumpsRVA:%08X\r\n"), pCOMTable->ExportAddressTableJumps.VirtualAddress);
		text.AppendFormat(TEXT("ExportAddressTableJumpsSize:%08X\r\n"), pCOMTable->ExportAddressTableJumps.Size);
		text.AppendFormat(TEXT("ManagedNativeHeaderRVA:%08X\r\n"), pCOMTable->ManagedNativeHeader.VirtualAddress);
		text.AppendFormat(TEXT("ManagedNativeHeaderSize:%08X\r\n"), pCOMTable->ManagedNativeHeader.Size);

		mSetEditText(text);
	}
	catch (...)
	{
		CString text(TEXT("获取COM表信息失败"));
		mSetEditText(text);
	}
}

// 借壳执行
int CDialogPE::mFakeShellRun(CString shellPath, PCHAR pBufferExe/*file buffer*/, UINT32 sizeofBufferExe)
{
	try {

		if (m_isImageBuffer)
			return -1;

		if (isX64(pBufferExe) != isX64(m_pBuffer))
			return -2;

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (!CreateProcess(
			shellPath,
			NULL,               // Command line
			NULL,               // Process handle not inheritable
			NULL,               // Thread handle not inheritable
			FALSE,              // Set handle inheritance to FALSE
			CREATE_NEW_CONSOLE | CREATE_SUSPENDED, //  子进程新建控制台.否则父子就用一个控制台,挂起形式创建
			NULL,               // Use parent's environment block
			NULL,               // Use parent's starting directory 
			&si,                // Pointer to STARTUPINFO structure
			&pi)                // Pointer to PROCESS_INFORMATION structure
			)
		{
			return -3;
		}
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		// 重新打开进程线程,以获取权限
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_OPERATION, FALSE, pi.dwProcessId);
		if (!hProcess)
			return -4;

		BOOL isWow64Process1 = FALSE;
		BOOL isWow64Process2 = FALSE;

		if (!IsWow64Process(hProcess, &isWow64Process1))
		{
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
			return -5;
		}
		if (!IsWow64Process(GetCurrentProcess(), &isWow64Process2))
		{
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
			return -6;
		}
		if (isWow64Process1 != isWow64Process2)
		{
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
			return -7;
		}

		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, false, pi.dwThreadId);
		if (!hThread)
		{
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
			return -8;
		}


		// 获取  线程初始上下文环境
		CONTEXT context = { 0 };
		context.ContextFlags = CONTEXT_FULL;	// 获取所有值.
		if (!GetThreadContext(hThread, &context))
		{
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
			CloseHandle(hThread);
			return -9;
		}

		// 将fileBuffer 转 imageBuffer
		PCHAR pNewBuffer = NULL;
		UINT32 newBufferSize = 0;
		int result = fileBufferToImageBuffer(m_pBuffer, m_bufferSize, &pNewBuffer, &newBufferSize);
		if (result < 0)
		{
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
			CloseHandle(hThread);
			return -5;
		}

		// 获取模块基址
		INT_PTR image_base = 0;
#if _WIN64
		ReadProcessMemory(hProcess, LPCVOID(context.Rdx + 0x10), &image_base, 8, NULL);
#else
		ReadProcessMemory(hProcess, LPCVOID(context.Ebx + 8), &image_base, 4, NULL);
#endif

		// 卸载目标进程的旧模块,Win10的安全机制问题. 不让直接写exe内存位置. 先卸载,再申请空间写.
		typedef long NTSTATUS;
		typedef NTSTATUS(__stdcall* pfnZwUnmapViewOfSection)(HANDLE ProcessHandle, PVOID  BaseAddress);
		HMODULE hModule = LoadLibrary(TEXT("ntdll.dll"));
		pfnZwUnmapViewOfSection func = (pfnZwUnmapViewOfSection)GetProcAddress(hModule, "ZwUnmapViewOfSection");
		NTSTATUS status = func(hProcess, (PVOID)image_base);  // 这是ntdll.dll 卸载模块的未导出函数
		if (status < 0)
		{
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
			CloseHandle(hThread);
			free(pNewBuffer);
			return -6;
		}

		// 申请新空间,并复制新模块到进程空间.
		PCHAR pNew = (PCHAR)VirtualAllocEx(hProcess, (LPVOID)image_base, newBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!pNew)
		{
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
			CloseHandle(hThread);
			free(pNewBuffer);
			return -7;
		}

		if (!WriteProcessMemory(hProcess, (LPVOID)pNew, pNewBuffer, newBufferSize, NULL))
		{
			TerminateProcess(hProcess, 0);
			CloseHandle(hThread);
			CloseHandle(hProcess);
			free(pNewBuffer);
			VirtualFreeEx(hProcess, (LPVOID)pNew, 0, MEM_RELEASE);
			return -6;
		}

		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

#ifdef _WIN64
		context.Rcx = image_base + pNt64->OptionalHeader.AddressOfEntryPoint;
#else
		context.Eax = image_base + pNt32->OptionalHeader.AddressOfEntryPoint;
#endif

		context.ContextFlags = CONTEXT_FULL;	// 获取所有值.
		if (!SetThreadContext(hThread, &context))
		{
			TerminateProcess(hProcess, 0);
			CloseHandle(hThread);
			CloseHandle(hProcess);
			free(pNewBuffer);
			VirtualFreeEx(hProcess, (LPVOID)pNew, 0, MEM_RELEASE);
			return -8;
		}

		ResumeThread(hThread);

		free(pNewBuffer);

		// 等待子进程结束
		// WaitForSingleObject(hProcess, INFINITE);

		CloseHandle(hThread);
		CloseHandle(hProcess);
	}
	catch (...)
	{
		return -9;
	}

	return 0;
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 事件处理

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// 初始化对话框
BOOL CDialogPE::OnInitDialog()
{
	int result = 0;

	CDialogEx::OnInitDialog();

	// 加载快捷键资源表
	m_hAccelTable = LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(ACCELERATOR_PE_DLG));

	// 创建控件
	result = mCreateItems();
	if (result < 0)
		return FALSE;

	// 刷新界面控件状态
	result = mRefreshPage(m_pBuffer, m_bufferSize,m_isImageBuffer);
	if (result < 0)
		return FALSE;

	return TRUE;
}

// 前置消息处理
BOOL CDialogPE::PreTranslateMessage(MSG* pMsg)
{
	// 处理加速键
	if (m_hAccelTable != NULL)
	{
		if (::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
			return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

// 销毁
void CDialogPE::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();

	delete this;
}

// 绘图
void CDialogPE::OnPaint()
{
	if (m_showFlag != 1)
		return;

	CPaintDC dc(this);

	mDrawHex(&dc);
}

// 关闭对话框
void CDialogPE::OnCancel()
{
	DestroyWindow();
}

// 处理按键事件
void CDialogPE::OnButtonDataDirectoryClick(UINT id)
{
	switch (id)
	{
	case CBUTTON_TABLE_ID_BEGIN:
		mGetPEexportTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 1:
		mGetPEImportTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 2:
		mGetPEResourceTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 3:
		mGetPESEHTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 4:
		mGetPECertificateTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 5:
		mGetPEBaseRelocationTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 6:
		mGetPEDebugTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 7:
		mGetPEArchitectureTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 8:
		mGetPEGlobalPTRTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 9:
		mGetTLSTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 10:
		mGetPELoadConfigTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 11:
		mGetPEBoundImportTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 12:
		mGetPEIATTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 13:
		mGetPEDelayLoadImportTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 14:
		mGetPEComTableInfo();
		break;
	case CBUTTON_TABLE_ID_BEGIN + 15:
		mGetPEBaseInfo();
		break;
	default:
		break;
	}
}

// 修改节
void CDialogPE::OnMenuModifySection()
{
	try {
		if (m_isImageBuffer)
		{
			AfxMessageBox(TEXT("内存镜像不支持修改节"));
			return;
		}

		int index = m_list_section.GetNextItem(-1, LVNI_SELECTED);
		if (index == -1)
			return;

		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		CString name((char*)pSec[index].Name, 8);
		CString size, charac;
		size.Format(TEXT("0x%08X"), pSec[index].SizeOfRawData);
		charac.Format(TEXT("0x%08X"), pSec[index].Characteristics);

		CDialogSection dlg(1, name, size, charac);
		INT_PTR result = dlg.DoModal();
		if (result != IDOK)
			return;

		CStringA nameA;
		nameA += name;
		UINT32 size_ = _tcstoul(size.GetString(), NULL, 16);
		UINT32 charac_ = _tcstoul(charac.GetString(), NULL, 16);

		PCHAR pBufferNew = NULL;
		UINT32 newBufferSize = 0;
		result = modifySectionInFileBuffer(m_pBuffer, m_bufferSize,index, (PCHAR)nameA.GetString(), size_, charac_, &pBufferNew, &newBufferSize);
		if (result < 0)
		{
			AfxMessageBox(TEXT("修改节失败"));	
			return;
		}
		mRefreshPage(pBufferNew, newBufferSize,0);
	}
	catch (...)
	{
		AfxMessageBox(TEXT("修改节失败:异常"));
	}
}

// 添加节
void CDialogPE::OnMenuAddSection()
{
	try {
		if (m_isImageBuffer)
		{
			AfxMessageBox(TEXT("内存镜像不支持修改节"));
			return;
		}

		CString name, size, charac;

		CDialogSection dlg(0, name, size, charac);
		INT_PTR result = dlg.DoModal();
		if (result != IDOK)
			return;

		CStringA nameA;
		nameA +=name;
		UINT32 size_ = _tcstoul(size.GetString(), NULL, 16);
		UINT32 charac_ = _tcstoul(charac.GetString(), NULL, 16);

		PCHAR pBufferNew = NULL;
		UINT32 newBufferSize = 0;
		result = addSectionInFileBuffer(m_pBuffer,m_bufferSize, (PCHAR)nameA.GetString(), size_, charac_, &pBufferNew, &newBufferSize);
		if (result < 0)
		{
			AfxMessageBox(TEXT("添加节失败"));
			return;
		}
		mRefreshPage(pBufferNew, newBufferSize,0);
	}
	catch (...)
	{
		AfxMessageBox(TEXT("添加节失败:异常"));
	}
}

// 合并节
void CDialogPE::OnMenuMergeSection()
{
	try {
		if (m_isImageBuffer)
		{
			AfxMessageBox(TEXT("内存镜像不支持修改节"));
			return;
		}

		int index = m_list_section.GetNextItem(-1, LVNI_SELECTED);
		if (index == -1)
			return;

		int result;
		PCHAR pBufferNew = NULL;
		UINT32 newBufferSize = 0;
		result = mergeSectionInFileBuffer(m_pBuffer,m_bufferSize, index, &pBufferNew , &newBufferSize);
		if (result < 0)
		{
			AfxMessageBox(TEXT("合并节失败"));
			return;
		}

		mRefreshPage(pBufferNew, newBufferSize,0);
	}
	catch (...)
	{
		AfxMessageBox(TEXT("合并节失败:异常"));
	}

}

// Section控件右键菜单事件处理函数
void CDialogPE::OnListCtrlSectionSelectMenu(NMHDR* pNMHDR, LRESULT* pResult)
{
	CMenu menu;
	menu.LoadMenu(MENU_PE_DLG_SECTION);
	CMenu* pPopupMenu = menu.GetSubMenu(0); // 获取第一个子菜单

	CPoint point;
	GetCursorPos(&point); // 获取当前鼠标位置
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

// 滚动条事件
void CDialogPE::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO si;
	pScrollBar->GetScrollInfo(&si);

	int id = pScrollBar->GetDlgCtrlID();

	int hexBegin = 0;

	// hex 滚动条 V
	if (id == CSCROLLBAR_HEX_V_ID)
	{
		switch (nSBCode)
		{
		case SB_TOP:
			hexBegin = 0;
			break;
		case SB_BOTTOM:
			hexBegin = 0x7FFFFFFF;
			break;
		case SB_LINEUP:
			hexBegin = m_hexBegin - 16;
			break;
		case SB_LINEDOWN:
			hexBegin = m_hexBegin + 16;
			break;
		case SB_PAGEUP:
			hexBegin = m_hexBegin - (si.nPage << 4);
			break;
		case SB_PAGEDOWN:
			hexBegin = m_hexBegin + (si.nPage << 4);
			break;
		case SB_THUMBPOSITION:
			hexBegin = si.nTrackPos << 4;
			break;
		case SB_THUMBTRACK:
			hexBegin = si.nTrackPos << 4;
			break;
		default:
			CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
			return;
		}

		mSetHexBegin(hexBegin);

		m_scrollbar_hex.SetScrollPos(m_hexBegin >> 4);
	}
}

// 鼠标滚轮事件
BOOL CDialogPE::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	short num = -zDelta / WHEEL_DELTA; // 滚轮滚动方向和步长	

	// Ctrl+滚轮 改变字体大小
	short state = GetKeyState(VK_CONTROL);
	if (state & 0x8000)
	{
		m_hexFontSize += num;
		if (m_hexFontSize < 1)
			m_hexFontSize = 1;
		mSetScrollBarHexInfo();
		InvalidateRect(m_hexRect);
		return TRUE;
	}

	mSetHexBegin(m_hexBegin + 16 * num);

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

// 背景擦除事件: 解决白屏闪烁问题
BOOL CDialogPE::OnEraseBkgnd(CDC* pDC)
{
	// 需要重绘的情况设置为1，否则不重绘,  例如CScrollBar的显示刷新
	if (m_flagEraseBk == 1)
	{
		m_flagEraseBk = 0;
		return CDialogEx::OnEraseBkgnd(pDC);
	}

	return TRUE;
}

// 加速键:     跳转到指定地址
void CDialogPE::OnHexGoto()
{
	CString str;
	CDialogInput input((PTCHAR)TEXT("请输入地址"), str);
	INT_PTR result = input.DoModal();
	if (result == IDOK)
	{
		UINT32 addr = _tcstoul(str.GetString(), NULL, 16);
		mSetHexBeginColored(addr, 1);
	}
}

// 树控件选中项改变 高亮显示对应区域
void CDialogPE::OnTreeCtrlHeaderInfoSelectChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hSelectedItem = pNMTreeView->itemNew.hItem;

	INFO_POSITION* pData = (INFO_POSITION*)m_tree_header.GetItemData(hSelectedItem);
	mSetHexBeginColored(pData->offset, pData->length);
}

// 列表控件选中项改变 高亮显示对应区域
void CDialogPE::OnListCtrlSectionSelectChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	int index = m_list_section.GetNextItem(-1, LVNI_SELECTED);
	if (index == -1)
		return;

	INFO_POSITION* pData = (INFO_POSITION*)m_list_section.GetItemData(index);
	mSetHexBeginColored(pData->offset, pData->length);
}

// 界面 hex 和 edit 切换
void CDialogPE::OnMenuSwitchShowFlag()
{
	UINT32 n = (m_showFlag + 1) % MAX_SHOW_FLAG;
	mSwitchShowFlag(n);
}

// 保存文件按钮
void CDialogPE::OnMenuSave()
{
	try {
		PCHAR pNewBuffer = NULL;
		UINT32 newBufferSize = 0;
		int needFree = 0;
		int result;
		if (m_isImageBuffer)
		{
			result = imageBufferToFileBuffer(m_pBuffer,m_bufferSize,&pNewBuffer, &newBufferSize);
			if (result < 0)
			{
				AfxMessageBox(TEXT("文件转换失败"));
				return;
			}
			needFree = 1;
		}
		else
		{
			pNewBuffer = m_pBuffer;
			newBufferSize = m_bufferSize;
		}

		CFileDialog dlgFile(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("PE文件|*.*||"), NULL);
		if (dlgFile.DoModal() != IDOK)
		{
			if (needFree)
				free(pNewBuffer);
			return;
		}
		CStringA strPathA;
		strPathA = dlgFile.GetPathName();
		if (strPathA.IsEmpty())
		{
			if (needFree)
				free(pNewBuffer);
			return;
		}

		result = mSaveFileData(strPathA.GetString(), pNewBuffer, newBufferSize);
		if (result < 0)
			AfxMessageBox(TEXT("保存失败"));
		else
			AfxMessageBox(TEXT("保存成功"));

		if (needFree)
			free(pNewBuffer);

	}
	catch (...)
	{
		AfxMessageBox(TEXT("保存失败:异常"));
	}
}

// 内存转文件按钮
void CDialogPE::OnMenuImageToFile()
{
	if (m_isImageBuffer)
	{
		PCHAR pNewBuffer = NULL;
		UINT32 newBufferSize = 0;
		int result = imageBufferToFileBuffer(m_pBuffer, m_bufferSize, &pNewBuffer, &newBufferSize);
		if (result < 0)
		{
			AfxMessageBox(TEXT("文件转换失败"));
			return;
		}
		mRefreshPage(pNewBuffer, newBufferSize,0);
	}
}

// 文件转内存按钮
void CDialogPE::OnMenuFileToImage()
{
	if (!m_isImageBuffer)
	{
		PCHAR pNewBuffer = NULL;
		UINT32 newBufferSize = 0;
		int result = fileBufferToImageBuffer(m_pBuffer, m_bufferSize, &pNewBuffer, &newBufferSize);
		if (result < 0)
		{
			AfxMessageBox(TEXT("文件转换失败"));
			return;
		}
		mRefreshPage(pNewBuffer, newBufferSize,1);
	}
}

// 导入表注入
void CDialogPE::OnMenuImportInject()
{
	try {
		if (m_isImageBuffer)
		{
			AfxMessageBox(TEXT("导入表注入虚先转为文件镜像"));
			return;
		}

		mCreateConsole(TEXT("导入表注入"));

		printf("选择注入方式:\r\n");
		printf("1 : 名字注入\r\n");
		printf("2 : 序号注入\r\n");
		printf("其他 : 退出\r\n");

		int n = 0;

		scanf_s("%d", &n);

		if (n == 1)
		{
			printf("格式: 模块名 函数名\r\n");
			char moduleName[256] = { 0 };
			char functionName[256] = { 0 };
			scanf_s("%s %s", moduleName, 256, functionName, 256);
			printf("模块名:%s\r\n", moduleName);
			printf("函数名:%s\r\n", functionName);
			PCHAR pNewBuffer = NULL;
			UINT32 newBufferSize = 0;
			int result = importTableInjectionByNameInFileBuffer(m_pBuffer, m_bufferSize, moduleName, functionName, &pNewBuffer, &newBufferSize);
			if (result < 0)
				AfxMessageBox(TEXT("注入失败"));
			else
			{
				mRefreshPage(pNewBuffer, newBufferSize, 0);
				AfxMessageBox(TEXT("注入成功"));
			}
		}

		if (n == 2)
		{
			printf("格式: 模块名 序号\r\n");
			char moduleName[256] = { 0 };
			int ordinal = 0;
			scanf_s("%s %d", moduleName, 256, &ordinal);
			printf("模块名:%s\r\n", moduleName);
			printf("序号:%d\r\n", ordinal);
			PCHAR pNewBuffer = NULL;
			UINT32 newBufferSize = 0;
			int result = importTableInjectionByNumberInFileBuffer(m_pBuffer, m_bufferSize, moduleName, ordinal, &pNewBuffer, &newBufferSize);
			if (result < 0)
				AfxMessageBox(TEXT("注入失败"));
			else
			{
				mRefreshPage(pNewBuffer, newBufferSize, 0);
				AfxMessageBox(TEXT("注入成功"));
			}
		}

		mCloseConsole();
	}
	catch (...)
	{
		AfxMessageBox(TEXT("注入失败:异常"));
	}
}

// 借壳执行按钮
void CDialogPE::OnMenuFakeShellExe()
{
	try {
		int needFree = 0;
		TCHAR text[256] = { 0 };

		CHAR path[MAX_PATH] = { 0 };
		mCreateConsole(TEXT("借壳执行"));
		printf("输入要执行的程序路径:\r\n");
		scanf_s("%s", path, MAX_PATH);
		printf("路径:%s\r\n", path);
		mCloseConsole();
		if (path[0] == 0)
		{
			AfxMessageBox(TEXT("路径不能为空"));
			return;
		}
		CString str;
		str += path;


		int result;
		PCHAR pNewBuffer = NULL;
		UINT32 newBufferSize = 0;
		if (m_isImageBuffer)
		{
			result = imageBufferToFileBuffer(m_pBuffer, m_bufferSize, &pNewBuffer, &newBufferSize);
			if (result < 0)
			{
				AfxMessageBox(TEXT("文件转换失败"));
				return;
			}
			needFree = 1;
		}
		else
		{
			pNewBuffer = m_pBuffer;
			newBufferSize = m_bufferSize;
		}

		result = mFakeShellRun(str, pNewBuffer, newBufferSize);
		if (result < 0)
			AfxMessageBox(TEXT("执行失败"));
		else
			AfxMessageBox(TEXT("执行成功"));
		if (needFree)
			free(pNewBuffer);

	}
	catch (...)
	{
		AfxMessageBox(TEXT("执行失败:异常"));
	}
}

// 添加壳按钮
void CDialogPE::OnMenuAddShell()
{
	try {
		int result;
		int needFree = 0;
		// 获取壳子文件
		CStringA strPathA;
		CHAR curDir[MAX_PATH] = { 0 };
		GetCurrentDirectoryA(MAX_PATH, curDir);
		strPathA = curDir;
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(m_pBuffer);

		strPathA += FILE_SHELL_PATH;
		if (isX64(p))
			strPathA += "x64\\";
		else
			strPathA += "x86\\";

		strPathA += FILE_SHELL_NAME;

		PCHAR pBufferShell = NULL;
		int shellBufferSize = 0;
		result = mGetFileData(strPathA.GetString(), &pBufferShell, &shellBufferSize);
		if (result < 0)
		{
			AfxMessageBox(TEXT("获取壳文件失败"));
			return;
		}

		// 获取exe文件
		PCHAR pBufferExe = NULL;
		UINT32 exeBufferSize = 0;
		if (m_isImageBuffer)
		{
			result = imageBufferToFileBuffer(m_pBuffer, m_bufferSize, &pBufferExe, &exeBufferSize);
			if (result < 0)
			{
				free(pBufferShell);
				AfxMessageBox(TEXT("文件转换失败"));
				return;
			}
			needFree = 1;
		}
		else
		{
			pBufferExe = m_pBuffer;
			exeBufferSize = m_bufferSize;
		}

		// 将exe文件添加到壳文件新增的节中.
		PCHAR pNewBuffer = NULL;
		UINT32 newBufferSize = 0;
		result = shellFileBufferApendExeFileBuffer(pBufferShell, shellBufferSize, pBufferExe, exeBufferSize, &pNewBuffer, &newBufferSize);
		if (result < 0)
		{
			free(pBufferShell);
			if (needFree)
				free(pBufferExe);
			AfxMessageBox(TEXT("添加壳失败"));
			return;
		}

		free(pBufferShell);
		if (needFree)
			free(pBufferExe);

		// 刷新界面
		mRefreshPage(pNewBuffer, newBufferSize, 0);

		AfxMessageBox(TEXT("添加壳成功"));
	}
	catch(...)
	{
		AfxMessageBox(TEXT("添加壳失败:异常"));
	}
}

// 测试按钮
void CDialogPE::OnMenuTest()
{
	int a = getExportItemRvaByNameInFileBuffer(m_pBuffer, "EntryFunc");
	a = 1;
}



