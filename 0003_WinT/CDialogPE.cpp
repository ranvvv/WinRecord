#include "pch.h"
#include "WinT.h"
#include "CDialogPE.h"
#include "afxdialogex.h"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// PE 分析界面

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

IMPLEMENT_DYNAMIC(CDialogPE, CDialogEx)
BEGIN_MESSAGE_MAP(CDialogPE, CDialogEx)
END_MESSAGE_MAP()

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 消息处理

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


CDialogPE::CDialogPE(PCHAR pBuffer, UINT32 length, UINT32 isMemImage, CString path, CWnd* pParent) : CDialogEx(IDD_DIALOG_PE, pParent)
{
	this->m_pBuffer = pBuffer;
	this->m_length = length;
	this->m_isMemImage = isMemImage;
	this->m_path = path;
}

CDialogPE::~CDialogPE()
{
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 消息处理

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


BOOL CDialogPE::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    m_menu.LoadMenu(IDR_MENU_DLG_PE);
    SetMenu(&m_menu);

    BOOL bCreate = FALSE;
    DWORD dwStyle =0;
    int left=0, top=0, right=0, bottom=0;

    // 创建 STATIC 控件 path
    left = MARGIN_COMMON;
    top = MARGIN_COMMON;
    right = left + CSTATIC_PATH_WIDTH;
    bottom = top + ITEM_HEIGHT_COMMON;
    dwStyle = WS_CHILD | WS_VISIBLE;
    bCreate = m_static_path.Create(_T("路径:"),dwStyle,CRect(left, top, right, bottom), this,CSTATIC_PATH_ID);
    if (!bCreate)
    {
        AfxMessageBox(_T("动态创建 STATIC 控件失败！"));
        return FALSE;
    }

    // 创建 EDIT 控件 path
    left = right + MARGIN_COMMON;
    right = MARGIN_COMMON + HALF_WIDTH;
    dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER |  ES_READONLY;        
    bCreate = m_edit_path.Create(dwStyle,CRect(left, top, right, bottom), this,CEDIT_PATH_ID);
    if (!bCreate)
    {
        AfxMessageBox(_T("动态创建 EDIT 控件失败！"));
        return FALSE;
    }

    // 创建 TREE 控件 header
    top = bottom + MARGIN_COMMON;
    left = MARGIN_COMMON;
    right = left + HALF_WIDTH;
    bottom = top + CTREE_HEADER_HEIGHT;
    dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | // 基础窗口样式（必需）
        TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | // 连接线+展开按钮
        TVS_SHOWSELALWAYS | TVS_SINGLEEXPAND; // 保持选中+点击文本展开
    bCreate = m_tree_header.Create(dwStyle,CRect(left, top, right, bottom), this,CTREE_HEADER_ID);
    if (!bCreate)
    {
        AfxMessageBox(_T("动态创建 TREE 控件失败！"));
        return FALSE;
    }

    top = bottom + MARGIN_COMMON;
    left = MARGIN_COMMON;
    right = left + HALF_WIDTH;
    bottom = top + CLIST_SECTION_HEIGHT;
    dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT;
    bCreate = m_list_section.Create(dwStyle,CRect(left, top, right, bottom), this,CLIST_SECTION_ID);
    if (!bCreate)
    {
        AfxMessageBox(_T("动态创建 LIST 控件失败！"));
        return FALSE;
    }

    m_list_section.InsertColumn(0, _T("名称"), LVCFMT_LEFT, 100);
    m_list_section.InsertColumn(1, _T("偏移"), LVCFMT_LEFT, 100);
    m_list_section.InsertColumn(2, _T("大小"), LVCFMT_LEFT, 100);
    m_list_section.InsertColumn(3, _T("属性"), LVCFMT_LEFT, 100);
    m_list_section.InsertColumn(4, _T("备注"), LVCFMT_LEFT, 100);

    dwStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;

    for (int i = 0; i < 4; ++i)
    {
        top = bottom + MARGIN_COMMON;
        bottom = top + CBUTTON_TABLE_HEIGHT;
        for (int j = 0; j < 4; ++j)
        {
            left = MARGIN_COMMON + j*CBUTTON_TABLE_WIDTH + j * MARGIN_COMMON;
            right = left + CBUTTON_TABLE_WIDTH;
            bCreate = m_button_table[i*4 + j].Create(_T(""), dwStyle, CRect(left, top, right, bottom), this, CBUTTON_TABLE_ID_BEGIN + i * 4 + j);
            if (!bCreate)
            {
                AfxMessageBox(_T("动态创建 BUTTON 控件失败！"));
                return FALSE;
            }
        }
    }

	return TRUE; 
}

void CDialogPE::OnCancel()
{
    DestroyWindow();
}

void CDialogPE::PostNcDestroy()
{
    CDialogEx::PostNcDestroy();

    delete this;
}


