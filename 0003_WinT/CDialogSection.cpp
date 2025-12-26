#include "pch.h"
#include "WinT.h"
#include "CDialogSection.h"
#include "afxdialogex.h"

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 节操作对话框类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

IMPLEMENT_DYNAMIC(CDialogSection, CDialogEx)

BEGIN_MESSAGE_MAP(CDialogSection, CDialogEx)
	ON_BN_CLICKED(IDC_SECTION_DLG_OK, &CDialogSection::OnBnClickedSectionDlgOk)
END_MESSAGE_MAP()

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 构造函数, 析构函数

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

CDialogSection::CDialogSection(BOOL isModify, CString& name, CString& size, CString& charac, CWnd* pParent /*=nullptr*/) : m_isModify(isModify), m_name(name), m_size(size), m_charac(charac), CDialogEx(IDD_SECTION_DLG, pParent)
{	
}

CDialogSection::~CDialogSection()
{
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 消息处理

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 初始化对话框
BOOL CDialogSection::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (m_isModify)
	{
		SetWindowText(_T("修改节"));
		((CEdit*)GetDlgItem(IDC_SECTION_DLG_NAME))->SetWindowText(m_name);
		((CEdit*)GetDlgItem(IDC_SECTION_DLG_SIZE))->SetWindowText(m_size);
		((CEdit*)GetDlgItem(IDC_SECTION_DLG_CHARAC))->SetWindowText(m_charac);
		((CEdit*)GetDlgItem(IDC_SECTION_DLG_OK))->SetWindowText(TEXT("修改"));
	}
	else
	{
		SetWindowText(_T("添加节"));
		((CEdit*)GetDlgItem(IDC_SECTION_DLG_OK))->SetWindowText(TEXT("添加"));
	}

	return TRUE;  
}

// 确定按钮
void CDialogSection::OnBnClickedSectionDlgOk()
{
	GetDlgItemText(IDC_SECTION_DLG_NAME,m_name);
	GetDlgItemText(IDC_SECTION_DLG_SIZE, m_size);
	GetDlgItemText(IDC_SECTION_DLG_CHARAC, m_charac);

	EndDialog(IDOK);
}
