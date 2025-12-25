// CDialogSection.cpp: 实现文件
//

#include "pch.h"
#include "WinT.h"
#include "CDialogSection.h"
#include "afxdialogex.h"


// CDialogSection 对话框

IMPLEMENT_DYNAMIC(CDialogSection, CDialogEx)

CDialogSection::CDialogSection(BOOL isModify, CString& name, CString& size, CString& charac, CWnd* pParent /*=nullptr*/) : m_isModify(isModify), m_name(name), m_size(size), m_charac(charac), CDialogEx(IDD_SECTION_DLG, pParent)
{	
}

CDialogSection::~CDialogSection()
{
}

void CDialogSection::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogSection, CDialogEx)
	ON_BN_CLICKED(IDC_SECTION_DLG_OK, &CDialogSection::OnBnClickedSectionDlgOk)
END_MESSAGE_MAP()


// CDialogSection 消息处理程序


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


void CDialogSection::OnBnClickedSectionDlgOk()
{
	GetDlgItemText(IDC_SECTION_DLG_NAME,m_name);
	GetDlgItemText(IDC_SECTION_DLG_SIZE, m_size);
	GetDlgItemText(IDC_SECTION_DLG_CHARAC, m_charac);

	EndDialog(IDOK);
}
