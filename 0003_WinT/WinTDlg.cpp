#include "pch.h"
#include "framework.h"
#include "WinT.h"
#include "WinTDlg.h"
#include "afxdialogex.h"
#include "CDialogPE.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CWinTDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_PE, &CWinTDlg::OnBnClickedButtonPe)
END_MESSAGE_MAP()


CWinTDlg::CWinTDlg(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_WINT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

BOOL CWinTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	OnBnClickedButtonPe();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CWinTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CWinTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWinTDlg::OnBnClickedButtonPe()
{	
	/*  正式代码
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("PE文件|*.*||"), NULL);
	if (dlgFile.DoModal() != IDOK)
		return;
	CString strPath = dlgFile.GetPathName();
	*/

	CString strPath = TEXT("G:\\code\\WinRecord\\WinT\\dist\\WinT.exe");// 测试代码

	CStringA strPathA = CStringA(strPath);
	if (!strPathA.GetLength())
	{
		MessageBox(_T("文件路径为空"), _T("提示"), MB_OK);
		return;
	}

	FILE* pFile = NULL;
	fopen_s(&pFile, strPathA, "rb");
	if (!pFile)
	{
		MessageBox(_T("打开文件失败"), _T("提示"), MB_OK);
		return;
	}

	fseek(pFile, 0, SEEK_END);
	int nSize = ftell(pFile);
	if (nSize == 0)
	{
		MessageBox(_T("文件大小为0"), _T("提示"), MB_OK);
		return;
	}
	fseek(pFile, 0, SEEK_SET);

	char* pBuffer = (char*)calloc(nSize, sizeof(char));
	if (!pBuffer)
	{
		MessageBox(_T("分配内存失败"), _T("提示"), MB_OK);
		return;
	}

	int n = fread(pBuffer, 1, nSize, pFile);
	if (n != nSize)
	{
		MessageBox(_T("读取文件失败"), _T("提示"), MB_OK);
		return;
	}
	fclose(pFile);

	CDialogPE* pPE = new CDialogPE(pBuffer, nSize, 0, strPath);
	pPE->Create(IDD_DIALOG_PE, this);
	pPE->ShowWindow(SW_SHOW);
	pPE->UpdateData(FALSE);
}
