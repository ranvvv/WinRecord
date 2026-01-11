#include "pch.h"
#include "WinT.h"
#include "CDialogProcess.h"
#include "afxdialogex.h"
#include<tlhelp32.h>
#include<Psapi.h>
#include "CDialogPE.h"
#include "Common.h"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 进程管理对话框

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


IMPLEMENT_DYNAMIC(CDialogProcess, CDialogEx)

BEGIN_MESSAGE_MAP(CDialogProcess, CDialogEx)
	// 进程List切换选中
	ON_NOTIFY(LVN_ITEMCHANGED, CLIST_PROCESS_ID, OnListCtrlProcessSelectChange)
	// 进程List右键菜单事件处理函数
	ON_NOTIFY(NM_RCLICK, CLIST_PROCESS_ID, OnListCtrlProcessSelectMenu)
	// 模块List双击事件
	ON_NOTIFY(NM_DBLCLK, CLIST_MODULE_ID, OnNMDblclkListModule)
	// 菜单项: 注入模块
	ON_COMMAND(PROCESS_DLG_MENU_INJECT, &CDialogProcess::OnDlgMenuInject)
END_MESSAGE_MAP()


CDialogProcess::CDialogProcess(CWnd* pParent /*=nullptr*/)	: CDialogEx(IDD_PROCESS_DLG, pParent)
{

}

CDialogProcess::~CDialogProcess()
{

}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 功能函数

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 创建控件
int CDialogProcess::mCreateItems()
{
	BOOL bCreate = FALSE;
	DWORD dwStyle = 0;
	LONG lStyle = 0;
	int left = 0, top = 0, right = 0, bottom = 0;

	top = V_MARGIN;
	left = 3;
	right = NAME_WIDTH;
	bottom = top + NAME_HEIGHT;


	// 创建 CStatic 控件 path
	dwStyle = WS_CHILD | WS_VISIBLE;
	bCreate = m_static_process.Create(_T("进程:"), dwStyle, CRect(left, top, right, bottom), this, CSTATIC_PROCESS_ID);
	if (!bCreate)
		return -1;


	// 创建 CListCtrl 控件 section

	top = bottom + V_MARGIN;
	right = LIST_WIDTH;
	bottom = top + LIST_HEIGHT;

	dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_VSCROLL;
	bCreate = m_list_process.Create(dwStyle, CRect(left, top, right, bottom), this, CLIST_PROCESS_ID);
	if (!bCreate)
		return -1;


	// 扩展样式
	lStyle = GetWindowLong(m_list_process.m_hWnd, GWL_STYLE);		// 获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; 										// 清除显示方式位
	lStyle |= LVS_REPORT; 											// 设置style
	lStyle |= LVS_SINGLESEL;										// 单选模式
	SetWindowLong(m_list_process.m_hWnd, GWL_STYLE, lStyle);		// 设置style
	// 扩展样式
	dwStyle = m_list_process.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	//dwStyle |= LVS_EX_CHECKBOXES;
	m_list_process.SetExtendedStyle(dwStyle);
	// 设置列头
	m_list_process.InsertColumn(1, _T("ID"), LVCFMT_LEFT, 58);
	m_list_process.InsertColumn(2, _T("主模块"), LVCFMT_LEFT, 150);
	m_list_process.InsertColumn(3, _T("父进程ID"), LVCFMT_LEFT, 90);
	m_list_process.InsertColumn(4, _T("线程数"), LVCFMT_LEFT, 90);
	m_list_process.InsertColumn(5, _T("位数"), LVCFMT_LEFT, 200);


	top = bottom + V_MARGIN;
	right = NAME_WIDTH;
	bottom = top + NAME_HEIGHT;

	// 创建 CStatic 控件 path
	dwStyle = WS_CHILD | WS_VISIBLE;
	bCreate = m_static_module.Create(_T("模块:"), dwStyle, CRect(left, top, right, bottom), this, CSTATIC_MODULE_ID);
	if (!bCreate)
		return -1;


	// 创建 CListCtrl 控件 section

	top = bottom + V_MARGIN;
	right = LIST_WIDTH;
	bottom = top + LIST_HEIGHT;

	dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_VSCROLL;
	bCreate = m_list_module.Create(dwStyle, CRect(left, top, right, bottom), this, CLIST_MODULE_ID);
	if (!bCreate)
		return -1;


	// 扩展样式
	lStyle = GetWindowLong(m_list_module.m_hWnd, GWL_STYLE);		// 获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; 										// 清除显示方式位
	lStyle |= LVS_REPORT; 											// 设置style
	lStyle |= LVS_SINGLESEL;										// 单选模式
	SetWindowLong(m_list_module.m_hWnd, GWL_STYLE, lStyle);		// 设置style
	// 扩展样式
	dwStyle = m_list_module.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	//dwStyle |= LVS_EX_CHECKBOXES;
	m_list_module.SetExtendedStyle(dwStyle);
	// 设置列头
	m_list_module.InsertColumn(0, _T("名称"), LVCFMT_LEFT, 150);
	m_list_module.InsertColumn(1, _T("基址"), LVCFMT_LEFT, 200);
	m_list_module.InsertColumn(2, _T("大小"), LVCFMT_LEFT, 200);
	m_list_module.InsertColumn(3, _T("完整路径"), LVCFMT_LEFT, 300);

	return 0;
}

// 初始化控件内容
int CDialogProcess::mInitItems()
{
	m_list_process.DeleteAllItems();
	m_list_module.DeleteAllItems();
	return 0;
}

// 枚举进程
void CDialogProcess::mListProcess()
{
	PROCESSENTRY32 pe32 = { 0 };
	HANDLE hProcessSnap = NULL;
	HANDLE hProcess = NULL;
	BOOL b = FALSE;
	BOOL isWow64Process = FALSE;
	CString str;
	int n;

	BOOL is64BitSystem;

	SYSTEM_INFO si = { 0 };
	GetNativeSystemInfo(&si); // 注意：用GetNativeSystemInfo而非GetSystemInfo，获取真实系统位数
	is64BitSystem = (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);


	pe32.dwSize = sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf(" CreateToolhelp32Snapshot error %d\n", GetLastError());
		return;
	}

	b = Process32First(hProcessSnap, &pe32);
	int i = 0;
	while (b)
	{
		str.Format(TEXT("%d"), pe32.th32ProcessID);
		n = m_list_process.InsertItem(i, str);

		str = pe32.szExeFile;
		m_list_process.SetItemText(n, 1, str);

		str.Format(TEXT("%d"), pe32.th32ParentProcessID);
		m_list_process.SetItemText(n, 2, str);

		str.Format(TEXT("%d"), pe32.cntThreads);
		m_list_process.SetItemText(n, 3, str);

		if (!is64BitSystem)
		{
			m_list_process.SetItemText(n, 4, _T("x86"));
		}
		else
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			if (!hProcess)
			{
				m_list_process.SetItemText(n, 4, _T("权限不足"));
			}
			else
			{
				isWow64Process = FALSE;
				if (IsWow64Process(hProcess, &isWow64Process))	// 判断是否是64位系统下的32位程		系统目录  c:/windows/System32   || c:/windows/SysWOW64
				{
					if (isWow64Process)
						m_list_process.SetItemText(n, 4, _T("x86"));
					else
						m_list_process.SetItemText(n, 4, _T("x64"));
				}
				CloseHandle(hProcess);
			}
		}

		m_list_process.SetItemData(n, (ULONG_PTR)pe32.th32ProcessID);

		b = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
}

// 枚举模块
void CDialogProcess::mListModule(DWORD pid)
{
	CString str;

	m_list_module.DeleteAllItems();

	// 使用CreateToolhelp32Snapshot 创建模块快照，然后遍历它
	HANDLE hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
		return;

	// 遍历快照中记录的模块
	MODULEENTRY32 me32 = { 0 };
	me32.dwSize = sizeof(MODULEENTRY32);
	if (::Module32First(hModuleSnap, &me32))
	{
		int i = 0;
		int n;
		do
		{
			str = me32.szModule;
			n = m_list_module.InsertItem(i, str);

			str.Format(TEXT("%p"), me32.modBaseAddr);
			m_list_module.SetItemText(n, 1, str);

			str.Format(TEXT("%X"), me32.modBaseSize);
			m_list_module.SetItemText(n, 2, str);

			str = me32.szExePath;
			m_list_module.SetItemText(n, 3, str);

			i++;
		} while (::Module32Next(hModuleSnap, &me32));
	}
	::CloseHandle(hModuleSnap);

}

// 远程线程注入
int CDialogProcess::mRemoteThreadInject(PCHAR path, DWORD pid)
{
	BOOL bRet = 0;
	FILE* fp = NULL;

	bRet = fopen_s(&fp, path, "rb");
	if (bRet || fp == NULL)
		return -1;
	else
		fclose(fp);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL)
		return -2;

	PCHAR pBuffer = (PCHAR)VirtualAllocEx(hProcess, NULL, strlen(path) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pBuffer == NULL)
	{
		CloseHandle(hProcess);
		return -3;
	}

	SIZE_T writeSize = 0;
	bRet = WriteProcessMemory(hProcess, pBuffer, path, strlen(path) + 1, &writeSize);
	if (!bRet || writeSize != strlen(path) + 1)
	{
		VirtualFreeEx(hProcess, pBuffer, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return -4;
	}

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pBuffer, 0, NULL);
	if (hThread == NULL)
	{
		VirtualFreeEx(hProcess, pBuffer, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return -5;
	}

	// VirtualFreeEx(hProcess, pBuffer, 0, MEM_RELEASE);
	CloseHandle(hThread);
	CloseHandle(hProcess);

	return 0;
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 事件处理

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 初始化控件
BOOL CDialogProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int ret;

	ret = mCreateItems();
	if (ret < 0)
		return FALSE;

	ret = mInitItems();
	if (ret < 0)
		return FALSE;

	mListProcess();

	return TRUE;
}

// 模块控件双击事件处理函数
void CDialogProcess::OnNMDblclkListModule(NMHDR* pNMHDR, LRESULT* pResult)
{
	int index = m_list_module.GetNextItem(-1, LVNI_SELECTED);
	if (index == -1)
		return;

	CString base = m_list_module.GetItemText(index, 1);
	CString size = m_list_module.GetItemText(index, 2);
	CString path = m_list_module.GetItemText(index, 3);


	index = m_list_process.GetNextItem(-1, LVNI_SELECTED);
	if (index == -1)
		return;
	UINT32 pid = (UINT32)m_list_process.GetItemData(index);


	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_READ, FALSE, pid);
	if (hProcess == NULL)
	{
		AfxMessageBox(L"打开进程失败");
		return;
	}

	UINT64 base_addr = _wcstoui64(base, NULL, 16);
	UINT64 size_addr = _wcstoui64(size, NULL, 16);

	PCHAR buffer = new CHAR[size_addr];
	SIZE_T read = 0;
	ReadProcessMemory(hProcess, (LPCVOID)base_addr, buffer, size_addr, &read);
	if (read != size_addr)
	{
		AfxMessageBox(L"读取内存失败");
		delete[] buffer;
		return;
	}

	CDialogPE* pPE = new CDialogPE(buffer, (UINT32)size_addr, 1, path);
	pPE->Create(IDD_PE_DLG, this);
	pPE->ShowWindow(SW_SHOW);
	pPE->UpdateData(FALSE);

	CloseHandle(hProcess);
}

// 进程控件选中项改变事件处理函数
void CDialogProcess::OnListCtrlProcessSelectChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	int index = m_list_process.GetNextItem(-1, LVNI_SELECTED);
	if (index == -1)
		return;

	UINT32 data = (UINT32)m_list_process.GetItemData(index);

	mListModule(data);
}

// Section控件右键菜单事件处理函数
void CDialogProcess::OnListCtrlProcessSelectMenu(NMHDR* pNMHDR, LRESULT* pResult)
{
	CMenu menu;
	menu.LoadMenu(MENU_PROCESS_DLG_PROCESS);
	CMenu* pPopupMenu = menu.GetSubMenu(0); // 获取第一个子菜单

	CPoint point;
	GetCursorPos(&point); // 获取当前鼠标位置
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

// 注入
void CDialogProcess::OnDlgMenuInject()
{
	CHAR path[MAX_PATH] = { 0 };
	int mode=0;

	int index = m_list_process.GetNextItem(-1, LVNI_SELECTED);
	if (index == -1)
		return;
	UINT32 data = (UINT32)m_list_process.GetItemData(index);

	mCreateConsole(TEXT("进程注入:"));
	printf("选择注入方式:\r\n");
	printf("0: 退出\r\n");
	printf("1: 远程线程注入\r\n");
	scanf_s("%d", &mode);
	if (mode == 0)
		goto end;

	printf("选择注入的DLL:\r\n");
	scanf_s("%s", path, MAX_PATH);
	if (path[0] == 0)
		goto end;

	switch (mode)
	{
	case 1:
		mRemoteThreadInject(path,data);
		break;
	default:
		break;
	}

end:
	mCloseConsole();
}





