#include "pch.h"
#include "framework.h"
#include "WinT.h"
#include "WinTDlg.h"
#include "afxdialogex.h"
#include "CDialogPE.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CWinTDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_PE, &CWinTDlg::OnBnClickedButtonPe)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


CWinTDlg::CWinTDlg(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_WINT_DLG, pParent)
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

// PE文件分析
void CWinTDlg::OnBnClickedButtonPe()
{	
	// 正式代码
	/*
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("PE文件|*.*||"), NULL);
	if (dlgFile.DoModal() != IDOK)
		return;
	CStringA strPathA(dlgFile.GetPathName());
	if (strPathA.IsEmpty())
		return;
	*/

	// 测试代码
	CStringA strPathA = "G:\\code\\WinRecord\\0003_WinT\\dist\\WinT.exe";// 测试代码

	char* pBuffer = NULL;
	int nSize = 0;
	int result = mGetFileData(strPathA.GetString(), &pBuffer, &nSize);
	if (result < 0)
	{
		AfxMessageBox(TEXT("读取文件失败！"));
		return;
	}

	CDialogPE* pPE = new CDialogPE(pBuffer, nSize, 0, CString(strPathA));
	pPE->Create(IDD_PE_DLG, this);
	pPE->ShowWindow(SW_SHOW);
	pPE->UpdateData(FALSE);
}

// 拖拽文件 进行PE文件分析
void CWinTDlg::OnDropFiles(HDROP hDropInfo)
{
	// CDialogEx::OnDropFiles(hDropInfo);

	// 1. 获取拖拽的文件总数
	UINT nFileCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	// 2. 遍历所有拖拽的文件
	CString strFilePath;
	for (UINT i = 0; i < nFileCount; i++)
	{
		// 获取文件路径
		DragQueryFile(hDropInfo, i, strFilePath.GetBuffer(MAX_PATH), MAX_PATH); // 这里利用GetBuffer(MAX_PATH)定制缓冲区.
		strFilePath.ReleaseBuffer(); // 重新计算字符串长度, 注意 不是释放缓冲区,而是让strFilePath可以正常使用
		if (strFilePath.IsEmpty())
			continue;

		char* pBuffer = NULL;
		int nSize = 0;
		CStringA strPathA(strFilePath);
		int result = mGetFileData(strPathA.GetString(), &pBuffer, &nSize);
		if (result < 0)
			continue;

		CDialogPE* pPE = new CDialogPE(pBuffer, nSize, 0, CString(strPathA));
		pPE->Create(IDD_PE_DLG, this);
		pPE->ShowWindow(SW_SHOW);
		pPE->UpdateData(FALSE);
	}

	// 4. 必须调用：释放拖拽相关资源
	DragFinish(hDropInfo);
}
