#include "pch.h"
#include "WinT.h"
#include "CDialogInput.h"
#include "afxdialogex.h"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 输入对话框

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

IMPLEMENT_DYNAMIC(CDialogInput, CDialogEx)
BEGIN_MESSAGE_MAP(CDialogInput, CDialogEx)
	// 输入完毕按钮
	ON_BN_CLICKED(IDC_INPUT_DLG_OK, &CDialogInput::OnBnClickedInputDlgOk)
END_MESSAGE_MAP()

CDialogInput::CDialogInput(CString title,CString& str,CWnd* pParent /*=nullptr*/ ) : CDialogEx(IDD_INPUT_DLG, pParent) , m_str(str)
{
	m_title = title;
}

CDialogInput::~CDialogInput()
{

}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 事件处理

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


BOOL CDialogInput::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(m_title);

	GetDlgItem(IDC_INPUT_DLG_INPUT)->SetFocus();

	return TRUE;  
}

void CDialogInput::OnBnClickedInputDlgOk()
{
	GetDlgItemText(IDC_INPUT_DLG_INPUT, m_str);
	EndDialog(IDOK);
}
