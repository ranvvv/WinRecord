#include "pch.h"
#include "WinT.h"
#include "CDialogPE.h"
#include "afxdialogex.h"
#include "PE.h"
#include "CDialogSection.h"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// PE分析对话框类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

IMPLEMENT_DYNAMIC(CDialogPE, CDialogEx)

BEGIN_MESSAGE_MAP(CDialogPE, CDialogEx)
    // CButton控件 : 点击事件
    ON_COMMAND_RANGE(CBUTTON_TABLE_ID_BEGIN, CBUTTON_TABLE_ID_BEGIN + 15, &CDialogPE::OnButtonClickDataDirectory)
    // CListCtrl Section控件 : 右键点击
    ON_NOTIFY(NM_RCLICK, CLIST_SECTION_ID, OnListCtrlSelectMenu)

    // 菜单项: 修改节
    ON_COMMAND(ID_SECTION_MODIFY, &CDialogPE::OnSectionModify)
    // 菜单项: 添加节
    ON_COMMAND(ID_SECTION_ADD, &CDialogPE::OnSectionAdd)
    // 菜单项: 合并节
    ON_COMMAND(ID_SECTION_MERGE, &CDialogPE::OnSectionMerge)
    // 菜单项: 测试保存
    ON_COMMAND(ID_TEST_SAVE, &CDialogPE::OnTestSave)
    // 菜单项: 测试按钮
    ON_COMMAND(ID_DLG_PE_MENU_TEST, &CDialogPE::OnDlgPeMenuTest)
    // 绘图
    ON_WM_PAINT()
    // 鼠标滚轮
    ON_WM_MOUSEHWHEEL()
    // 混动条
    ON_WM_VSCROLL()
    ON_WM_MOUSEWHEEL()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 构造函数和析构函数

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


CDialogPE::CDialogPE(PCHAR pBuffer, UINT32 length, UINT32 isMemImage, CString path, CWnd* pParent) : CDialogEx(IDD_PE_DLG, pParent)
{
	m_pBuffer = pBuffer;
	m_length = length;
	m_isMemImage = isMemImage;
	m_path = path;
    m_isDirty = FALSE;

    m_showFlag = 1;
    m_hexBegin = 0;
    m_hexBeginWithBkColor = 0;
    m_hexLengthWithBkColor = 0;
    m_hexColor = RGB( 255,0 ,0);
    m_hexFontSize = 12;
    m_hexRect.SetRect(0, 0, 0, 0);
    m_hexModifyCursor = 0;
    m_defaultBkColor = RGB(255, 255, 255);
}

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
    BOOL bCreate = FALSE;
    DWORD dwStyle = 0;
    LONG lStyle;
    int left = 0, top = 0, right = 0, bottom = 0;

    // 加载菜单
    m_menu.LoadMenu(IDR_MENU_DLG_PE);
    SetMenu(&m_menu);

    // 创建 STATIC 控件 path
    left = MARGIN_COMMON;
    top = MARGIN_COMMON;
    right = left + CSTATIC_PATH_WIDTH;
    bottom = top + ITEM_HEIGHT_COMMON;
    dwStyle = WS_CHILD | WS_VISIBLE;
    bCreate = m_static_path.Create(_T("路径:"), dwStyle, CRect(left, top, right, bottom), this, CSTATIC_PATH_ID);
    if (!bCreate)
        return -1;

    // 创建 EDIT 控件 path
    left = right + MARGIN_COMMON;
    right = MARGIN_COMMON + HALF_WIDTH;
    dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY;
    bCreate = m_edit_path.Create(dwStyle, CRect(left, top, right, bottom), this, CEDIT_PATH_ID);
    if (!bCreate)
        return -1;

    // 创建 TREE 控件 header
    top = bottom + MARGIN_COMMON;
    left = MARGIN_COMMON;
    right = left + HALF_WIDTH;
    bottom = top + CTREE_HEADER_HEIGHT;
    dwStyle = WS_VISIBLE | WS_TABSTOP | WS_CHILD | WS_BORDER | WS_VSCROLL | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES | TVS_DISABLEDRAGDROP | TVS_NOTOOLTIPS;
    bCreate = m_tree_header.Create(dwStyle, CRect(left, top, right, bottom), this, CTREE_HEADER_ID);
    if (!bCreate)
        return -1;

    // 创建 LIST 控件 section
    top = bottom + MARGIN_COMMON;
    left = MARGIN_COMMON;
    right = left + HALF_WIDTH;
    bottom = top + CLIST_SECTION_HEIGHT;
    dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_VSCROLL;
    bCreate = m_list_section.Create(dwStyle, CRect(left, top, right, bottom), this, CLIST_SECTION_ID);
    if (!bCreate)
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
    dwStyle |= LVS_EX_CHECKBOXES;
    m_list_section.SetExtendedStyle(dwStyle);
    // 设置列头
    m_list_section.InsertColumn(0, _T("索引"), LVCFMT_LEFT, 40);
    m_list_section.InsertColumn(1, _T("名称"), LVCFMT_LEFT, 58);
    m_list_section.InsertColumn(2, _T("内存大小"), LVCFMT_LEFT, 90);
    m_list_section.InsertColumn(3, _T("内存基址"), LVCFMT_LEFT, 90);
    m_list_section.InsertColumn(4, _T("文件大小"), LVCFMT_LEFT, 90);
    m_list_section.InsertColumn(5, _T("文件基址"), LVCFMT_LEFT, 90);
    m_list_section.InsertColumn(6, _T("属性"), LVCFMT_LEFT, 90);

    // 创建 BUTTON 控件 table
    dwStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;
    for (int i = 0; i < 4; ++i)
    {
        top = bottom + MARGIN_COMMON;
        bottom = top + CBUTTON_TABLE_HEIGHT;
        for (int j = 0; j < 4; ++j)
        {
            left = MARGIN_COMMON + j * CBUTTON_TABLE_WIDTH + j * MARGIN_COMMON;
            right = left + CBUTTON_TABLE_WIDTH;
            bCreate = m_button_table[i * 4 + j].Create(_T(""), dwStyle, CRect(left, top, right, bottom), this, CBUTTON_TABLE_ID_BEGIN + i * 4 + j);
            if (!bCreate)
                return -1;
        }
    }

    // 创建 EDIT 控件 text 用来显示文本信息
    top = MARGIN_COMMON;
    left = MARGIN_COMMON + HALF_WIDTH + MARGIN_COMMON;
    right = left + HALF_WIDTH;
    m_hexRect.SetRect(left, top, right, bottom);
    dwStyle = WS_HSCROLL | WS_VSCROLL | WS_CHILD | ES_READONLY | ES_MULTILINE | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL;
    m_edit_text.Create(dwStyle, m_hexRect , this, CEDIT_TEXT_ID);

    // 创建滚动条控件
    left = right;
    right = left + CSCROLLBAR_HEX_V_WIDTH;
    dwStyle = WS_CHILD | WS_VISIBLE | SBS_VERT;
    m_scrollbar_hex.Create(dwStyle, CRect(left, top, right, bottom), this, CSCROLLBAR_HEX_V_ID);


    return 0;
}

// 初始化控件
int CDialogPE::mInitItems()
{
    m_edit_path.SetWindowText(TEXT(""));
    m_edit_path.EnableWindow(FALSE);
    m_tree_header.DeleteAllItems();
    m_tree_header.EnableWindow(FALSE);
    m_list_section.DeleteAllItems();
    m_list_section.EnableWindow(FALSE);

    PTCHAR name = NULL;
    for (int i = 0; i < 16; ++i)
        m_button_table[i].EnableWindow(FALSE);

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
    m_button_table[10].SetWindowText(TEXT("SEH异常处理表"));
    m_button_table[11].SetWindowText(TEXT("绑定导入表"));
    m_button_table[12].SetWindowText(TEXT("IAT表"));
    m_button_table[13].SetWindowText(TEXT("延迟导入表"));
    m_button_table[14].SetWindowText(TEXT("COM信息表"));
    m_button_table[15].SetWindowText(TEXT("保留"));

    if (m_showFlag != 0)
        m_edit_text.ShowWindow(FALSE);

    m_scrollbar_hex.ShowWindow(m_showFlag == 1);
    mSetScrollBarHex();

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
        HTREEITEM hChild = NULL;
        HTREEITEM hFileHeader = NULL;
        HTREEITEM hOptionalHeader = NULL;

        offset = pDos->e_lfanew;

        // NT_HEADER
        HTREEITEM hRoot = m_tree_header.InsertItem(_T("NT_HEADER"));
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

        hFileHeader = m_tree_header.InsertItem(_T("FILE_HEADER"), hRoot);
        m_infoPosition[2].offset = offset;
        m_infoPosition[2].length = sizeof(pNt32->FileHeader);
        m_tree_header.SetItemData(hFileHeader, (ULONG_PTR)&m_infoPosition[2]);

        /*
        typedef struct _IMAGE_FILE_HEADER {
        WORD    Machine;
        WORD    NumberOfSections;
        DWORD   TimeDateStamp;
        DWORD   PointerToSymbolTable;
        DWORD   NumberOfSymbols;
        WORD    SizeOfOptionalHeader;
        WORD    Characteristics;
        } IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;
        */

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
            hOptionalHeader = m_tree_header.InsertItem(_T("OPTIONAL_HEADER64"), hRoot);
            m_infoPosition[10].offset = offset;
            m_infoPosition[10].length = pNt64->FileHeader.SizeOfOptionalHeader;
            m_tree_header.SetItemData(hOptionalHeader, (ULONG_PTR)&m_infoPosition[10]);

#if 0
            // 可选头 64 
			typedef struct _IMAGE_OPTIONAL_HEADER64 {
				WORD        Magic;
				BYTE        MajorLinkerVersion;
				BYTE        MinorLinkerVersion;
				DWORD       SizeOfCode;
				DWORD       SizeOfInitializedData;
				DWORD       SizeOfUninitializedData;
				DWORD       AddressOfEntryPoint;
				DWORD       BaseOfCode;
				ULONGLONG   ImageBase;
				DWORD       SectionAlignment;
				DWORD       FileAlignment;
				WORD        MajorOperatingSystemVersion;
				WORD        MinorOperatingSystemVersion;
				WORD        MajorImageVersion;
				WORD        MinorImageVersion;
				WORD        MajorSubsystemVersion;
				WORD        MinorSubsystemVersion;
				DWORD       Win32VersionValue;
				DWORD       SizeOfImage;
				DWORD       SizeOfHeaders;
				DWORD       CheckSum;
				WORD        Subsystem;
				WORD        DllCharacteristics;
				ULONGLONG   SizeOfStackReserve;
				ULONGLONG   SizeOfStackCommit;
				ULONGLONG   SizeOfHeapReserve;
				ULONGLONG   SizeOfHeapCommit;
				DWORD       LoaderFlags;
				DWORD       NumberOfRvaAndSizes;
				IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
			} IMAGE_OPTIONAL_HEADER64, * PIMAGE_OPTIONAL_HEADER64;
#endif
            
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

            str.Format(TEXT("SizeOfImage: 0x%I64X"), pNt64->OptionalHeader.SizeOfImage);
            hChild = m_tree_header.InsertItem(str, hOptionalHeader);
            m_infoPosition[29].offset = offset;
            m_infoPosition[29].length = sizeof(pNt64->OptionalHeader.SizeOfImage);
            m_tree_header.SetItemData(hChild, (ULONG_PTR)&m_infoPosition[29]);
            offset += sizeof(pNt64->OptionalHeader.SizeOfImage);

            str.Format(TEXT("SizeOfHeaders: 0x%I64X"), pNt64->OptionalHeader.SizeOfHeaders);
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

            hOptionalHeader = m_tree_header.InsertItem(_T("OPTIONAL_HEADER32"), hRoot);
            m_infoPosition[10].offset = offset;
            m_infoPosition[10].length = pNt64->FileHeader.SizeOfOptionalHeader;
            m_tree_header.SetItemData(hOptionalHeader, (ULONG_PTR)&m_infoPosition[10]);

#if 0
            // 可选头32
			typedef struct _IMAGE_OPTIONAL_HEADER {
				//
				// Standard fields.
				//

				WORD    Magic;
				BYTE    MajorLinkerVersion;
				BYTE    MinorLinkerVersion;
				DWORD   SizeOfCode;
				DWORD   SizeOfInitializedData;
				DWORD   SizeOfUninitializedData;
				DWORD   AddressOfEntryPoint;
				DWORD   BaseOfCode;
				DWORD   BaseOfData;

				//
				// NT additional fields.
				//

				DWORD   ImageBase;
				DWORD   SectionAlignment;
				DWORD   FileAlignment;
				WORD    MajorOperatingSystemVersion;
				WORD    MinorOperatingSystemVersion;
				WORD    MajorImageVersion;
				WORD    MinorImageVersion;
				WORD    MajorSubsystemVersion;
				WORD    MinorSubsystemVersion;
				DWORD   Win32VersionValue;
				DWORD   SizeOfImage;
				DWORD   SizeOfHeaders;
				DWORD   CheckSum;
				WORD    Subsystem;
				WORD    DllCharacteristics;
				DWORD   SizeOfStackReserve;
				DWORD   SizeOfStackCommit;
				DWORD   SizeOfHeapReserve;
				DWORD   SizeOfHeapCommit;
				DWORD   LoaderFlags;
				DWORD   NumberOfRvaAndSizes;
				IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
			} IMAGE_OPTIONAL_HEADER32, * PIMAGE_OPTIONAL_HEADER32;
#endif

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
    catch(...)
    {
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
            m_list_section.SetItemText(n, 1, CString(name));

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

            m_infoPosition[50 + i].offset =(UINT32)((char*)pSec - p);
            m_infoPosition[50 + i].length = sizeof(IMAGE_SECTION_HEADER);
            m_list_section.SetItemData(n, (ULONG_PTR)&m_infoPosition[50 + i]);
        }
    }
    catch (...)
    {
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
            {
                m_button_table[i].EnableWindow(TRUE);
            }
        }
    }
    catch (...)
    {
        return -1;
    }

    return 0;
}

// 分析PE文件,并更新控件状态
int CDialogPE::mAnalyzePEFile()
{
    int result = 0;

    this->SetWindowTextW(PathFindFileName(m_path));

    m_edit_path.SetWindowText(m_path);
    m_edit_path.EnableWindow(TRUE);

    result = mAnalyzeHeaderInfo();
    if (result < 0)
        return -1;

    result = mAnalyzeSectionInfo();
    if (result < 0)
        return -1;

    result = mAnalyzeTableInfo();
    if (result < 0)
        return -1;


    return 0;
}

// 刷新页面信息
int CDialogPE::mRefreshPage(char* pNewBuffer)
{
    if (pNewBuffer)
    {
        free(m_pBuffer);
        m_pBuffer = pNewBuffer;
        m_length = getPEFileSize(m_pBuffer);
        m_isDirty = TRUE;   // 设置脏标记，表示缓冲区已经被修改了
    }

    mInitItems();
    mAnalyzePEFile();

    // 更新窗口标题，如果缓冲区被修改了，则在后面加上 (dirty) 字样
    if (m_isDirty)
    {
        CString title;
        GetWindowText(title);
        title.Append(TEXT(" (dirty)"));
        SetWindowText(title);
    }

    return 0;
}

// 切换页面显示
int CDialogPE::mSwitchShowFlag(int showFlag)
{
    if (showFlag == m_showFlag)
        return 0;

    m_showFlag = showFlag;

    switch (m_showFlag)
    {
        case 0:
        {
            m_edit_text.ShowWindow(TRUE);
            m_scrollbar_hex.ShowWindow(FALSE);
            break;
        }
        case 1:
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

    // 更新右侧区域
    InvalidateRect(m_hexRect);

    return 0;
}

// 获取hex显示的页范围
UINT32 CDialogPE::mGetHexPageRange()
{
    CDC* pDC = GetDC();
    CFont fontNormal;
    fontNormal.CreatePointFont(mPixelsToPoints(pDC, m_hexFontSize), TEXT("Courier New")); // 等宽字体
    pDC->SelectObject(&fontNormal);
    CSize normalFontSize = pDC->GetTextExtent(TEXT("0"), 1);
    UINT32 lineMargin = normalFontSize.cy / 2;
    int pageRange = (m_hexRect.Height() / (normalFontSize.cy + lineMargin)) - 1;
    ReleaseDC(pDC);

    return pageRange;
}

// 设置滚动条范围
int CDialogPE::mSetScrollBarHex()
{
    int range = m_length / 16 + (m_length % 16 ? 1 : 0);
    UINT32 pageRange = mGetHexPageRange();

    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.nMin = 0;
    si.nMax = range;
    si.nPage = pageRange;
    si.nPos = m_hexBegin >> 4;
    si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
    m_scrollbar_hex.SetScrollInfo(&si, TRUE);

    return 0;
}

// 像素转换为点
int CDialogPE::mPixelsToPoints(CDC* pDc, int pixels)
{
    int a = (int)((pixels * 720) / pDc->GetDeviceCaps(LOGPIXELSX));
    return a;
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

    m_hexBegin &= 0xFFFFFFF0;   // 对齐

    // 双缓冲 配置
    memDC.CreateCompatibleDC(pDC);			// 创建一个内存DC
    bitmapBuffer.CreateCompatibleBitmap(pDC, m_hexRect.Width(), m_hexRect.Height());		// 创建一个兼容的位图
    memDC.SelectObject(&bitmapBuffer);		// 选择位图到内存DC

    // 字体定义: 大字体显示hex,小字体显示符号
    CFont fontNormal, fontSmall;
    fontNormal.CreatePointFont(mPixelsToPoints(&memDC, m_hexFontSize), TEXT("Courier New")); // 等宽字体
    fontSmall.CreatePointFont(mPixelsToPoints(&memDC, (int)(m_hexFontSize * 0.9)), TEXT("Courier New")); // 等宽字体
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
    int n = mGetHexPageRange();  // m_hexRect区域能显示的行数,  要去掉最顶部的[0,F]行

    m_hexBeginWithBkColor = 3;
    m_hexLengthWithBkColor = 50;

    rect.top = normalFontSize.cy + lineMargin;
    for (int i = 0; i < n; ++i)
    {
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
            str.Format(TEXT("%02X"), *(m_pBuffer + m_hexBegin + i * 16 + j));

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

// 设置hex开始位置
int CDialogPE::mSetHexBegin(int hexBegin)
{
    int range = m_length / 16 + (m_length % 16 ? 1 : 0);
    int pageRange = mGetHexPageRange();

    if (hexBegin < 0)
        m_hexBegin = 0;
    else if (hexBegin >> 4 > range - pageRange)
        m_hexBegin = (range - pageRange) << 4;
    else
        m_hexBegin = hexBegin;

    InvalidateRect(m_hexRect);

    return 0;
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 消息处理

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 初始化对话框
BOOL CDialogPE::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    int result = 0;

    result = mCreateItems();
    if (result < 0)
        return FALSE;

    result = mInitItems();
    if (result < 0)
        return FALSE;

    result = mAnalyzePEFile();
    if (result < 0)
        return FALSE;

	return TRUE; 
}

// 处理按键事件
void CDialogPE::OnButtonClickDataDirectory(UINT id)
{
    switch (id)
    {
    case CBUTTON_TABLE_ID_BEGIN:
        break;
    case CBUTTON_TABLE_ID_BEGIN + 1:
        break;
    case CBUTTON_TABLE_ID_BEGIN + 2:
        break;
    case CBUTTON_TABLE_ID_BEGIN + 3:
        break;
    default:
        break;
    }
}

// 关闭对话框
void CDialogPE::OnCancel()
{
    DestroyWindow();
}

// 销毁
void CDialogPE::PostNcDestroy()
{
    CDialogEx::PostNcDestroy();

    delete this;
}

// 测试按钮
void CDialogPE::OnDlgPeMenuTest()
{
    UINT32 n = (m_showFlag + 1)% MAX_SHOW_FLAG;
    mSwitchShowFlag(n);
}

// 修改节
void CDialogPE::OnSectionModify()
{
    try {
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

        CStringA nameA(name);
        UINT32 size_ = _tcstoul(size, NULL, 16);
        UINT32 charac_ = _tcstoul(charac, NULL, 16);

        PCHAR pBufferNew = modifySection(m_pBuffer, index, (PCHAR)nameA.GetString(), size_, charac_);
        if (pBufferNew)
        {
            mRefreshPage(pBufferNew);
        }
    }
    catch (...)
    {
        AfxMessageBox(TEXT("修改节失败"));
    }
}

// 添加节
void CDialogPE::OnSectionAdd()
{
    try {
        CString name, size, charac;

        CDialogSection dlg(0, name, size, charac);
        INT_PTR result = dlg.DoModal();
        if (result != IDOK)
            return;

        CStringA nameA(name);
        UINT32 size_ = _tcstoul(size, NULL, 16);
        UINT32 charac_ = _tcstoul(charac, NULL, 16);

        PCHAR pBufferNew = addSection(m_pBuffer, (PCHAR)nameA.GetString(), size_, charac_);
        if (pBufferNew)
            mRefreshPage(pBufferNew);
    }
    catch (...)
    {
        AfxMessageBox(TEXT("添加节失败"));
    }
}

// 合并节
void CDialogPE::OnSectionMerge()
{
    try {
        int index = m_list_section.GetNextItem(-1, LVNI_SELECTED);
        if (index == -1)
            return;

        PCHAR pBufferNew = mergeSection(m_pBuffer, index);
        if (pBufferNew)
        {
            mRefreshPage(pBufferNew);
        }
    }
    catch (...)
    {
        AfxMessageBox(TEXT("合并节失败"));
    }

}

// 保存测试
void CDialogPE::OnTestSave()
{
    FILE* pf;
    fopen_s(&pf, "f:\\a.exe", "wb");
    if (!pf)
        return;
    fwrite(m_pBuffer, 1, m_length, pf);
    fclose(pf);
}

// Section控件右键菜单事件处理函数
void CDialogPE::OnListCtrlSelectMenu(NMHDR* pNMHDR, LRESULT* pResult)
{
    CMenu menu;
    menu.LoadMenu(IDR_MENU_PE_SECTION);
    CMenu* pPopupMenu = menu.GetSubMenu(0); // 获取第一个子菜单

    CPoint point;
    GetCursorPos(&point); // 获取当前鼠标位置
    pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

// 绘图
void CDialogPE::OnPaint()
{
    CPaintDC dc(this); 

    if (m_showFlag != 1)
        return;

    // dc.FillRect(m_hexRect, &CBrush(RGB(111, 111, 255)));

    mDrawHex(&dc);


}

// 滚动条事件
void CDialogPE::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    SCROLLINFO si;
    pScrollBar->GetScrollInfo(&si);

    int id = pScrollBar->GetDlgCtrlID();

    int hexBegin=0;

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
        
        m_scrollbar_hex.SetScrollPos(m_hexBegin>>4);
    }
}

// 鼠标滚轮事件
BOOL CDialogPE::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    short state = GetKeyState(VK_CONTROL);
    short num = -zDelta / WHEEL_DELTA; // 滚轮滚动方向和步长	
    if (state & 0x8000)
    {
        m_hexFontSize += num;
        if (m_hexFontSize < 1)
            m_hexFontSize = 1;
        mSetScrollBarHex();
        InvalidateRect(m_hexRect);
        return TRUE;
    }

    mSetHexBegin(m_hexBegin + 16 * num);

    return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

// 背景擦除事件: 解决白屏闪烁问题
BOOL CDialogPE::OnEraseBkgnd(CDC* pDC)
{
    // 背景擦除是白屏闪烁的原因.
    return TRUE;

    return CDialogEx::OnEraseBkgnd(pDC);
}
