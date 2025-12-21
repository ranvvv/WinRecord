#include"../common.h"

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 1. 通用控件

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 按钮
static LRESULT CALLBACK s1_1(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:		// dlg初始化
		CreateWindow(TEXT("BUTTON"), TEXT("点击我"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 10, 100, 30, hwnd, (HMENU)1001, GetModuleHandle(NULL), NULL);
		break;
	case WM_COMMAND:		// 子窗口消息处理
	{
		switch (LOWORD(wParam))
		{
		case 1001:
			mDbgPrint(TEXT("点击按钮\n"));
			break;
		default:
			break;
		}
		break;
	}
	case WM_CLOSE:				// 点右上角关闭按钮时发送此消息.
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return FALSE;		// 没处理交给默认,返回false
	}
	return TRUE; // 处理了返回true
}

// static
static LRESULT CALLBACK s1_2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hStaticText;

	switch (msg)
	{
	case WM_INITDIALOG:		// dlg初始化
	{
		// 示例：创建文本标签（左对齐 + 边框）
	hStaticText =  CreateWindowEx(
			0,                          // 扩展样式（一般为0）
			TEXT("STATIC"),             // 预定义类名 STATIC
			TEXT("这是静态文本标签"),    // 显示文本（图标/位图类型留空）
			WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN | SS_NOTIFY, // 窗口样式（左对齐、带边框）
			50, 50, 200, 30,            // 位置（x,y）和大小（宽,高）
			hwnd,                       // 父窗口句柄
			(HMENU)1001,      // 控件ID（自定义，用于消息识别）
			GetModuleHandle(NULL),      // 模块句柄（一般为当前进程）
			NULL                        // 附加数据（一般为NULL）
		);
		break;
	}
	case WM_COMMAND:		// 子窗口消息处理
	{
		switch (LOWORD(wParam))
		{
		case 1001:
		{
			// 方法1：SetWindowText（推荐）
			SetWindowText(hStaticText, TEXT("修改后的文本"));
			// 方法2：发送 WM_SETTEXT 消息
			SendMessage(hStaticText, WM_SETTEXT, 0, (LPARAM)TEXT("修改后的文本"));
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_CLOSE:				// 点右上角关闭按钮时发送此消息.
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return FALSE;		// 没处理交给默认,返回false
	}
	return TRUE; // 处理了返回true
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 2. 标准控件

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#include <commctrl.h> // 引入通用控件的头文件
#pragma comment(lib, "comctl32.lib")  // 链接公共控件库

// 列表
static LRESULT CALLBACK s2_1(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hList;
	switch (msg)
	{
	case WM_INITDIALOG:		// dlg初始化
	{
		// --------------- 创建列表控件 ---------------
		HWND hList = CreateWindowEx(
			0, WC_LISTVIEW, TEXT(""),
			WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT,
			10, 10, 300, 100, hwnd, (HMENU)1001, GetModuleHandle(0)	, NULL
		);
		// 设置扩展样式
		ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		// --------------- 插入列头 ---------------
		// 插入列头
		LVCOLUMN lvc = { 0 };
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc.fmt = LVCFMT_LEFT;

		// 第1列（索引0）
		lvc.cx = 80; 
		lvc.pszText = (PTCHAR)TEXT("ID");
		// ListView_InsertColumn(hList, 0, &lvc);
		SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);

		// 第2列（索引1）
		lvc.cx = 150; 
		lvc.pszText = (PTCHAR)TEXT("名称");
		ListView_InsertColumn(hList, 1, &lvc);

		// --------------- 插入行 ---------------

		// 第一行
		LVITEM lvi = { 0 };
		lvi.mask = LVIF_TEXT;
		lvi.iItem = ListView_GetItemCount(hList); // 新增行索引
		lvi.pszText = (PTCHAR)TEXT("1001");
		// int nRow = ListView_InsertItem(hList, &lvi);
		SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&lvi);

		lvi.iItem = 0; // 第1行
		lvi.iSubItem = 1; // 子项：第2列
		lvi.pszText = (PTCHAR)TEXT("小明");
		ListView_SetItem(hList, &lvi);

		// 第二行
		memset(&lvi, 0, sizeof(LVITEM));
		lvi.mask = LVIF_TEXT;
		lvi.iItem = ListView_GetItemCount(hList); // 新增行索引
		lvi.pszText = (PTCHAR)TEXT("1002");
		int nRow = ListView_InsertItem(hList, &lvi);

		lvi.iItem = nRow; // 第2行
		lvi.iSubItem = 1; // 子项：第2列
		lvi.pszText = (PTCHAR)TEXT("小王");
		ListView_SetItem(hList, &lvi);

		/*

		删除指定行	ListView_DeleteItem(hList, 行索引)
		清空所有行	ListView_DeleteAllItems(hList)
		获取行数	ListView_GetItemCount(hList)
		修改单元格文本	ListView_SetItemText(hList, 行, 列, 文本)
		获取单元格文本	ListView_GetItemText(hList, 行, 列, 缓冲区, 缓冲区大小)
		// 选中第n行
		ListView_SetItemState(hList, nRow, LVIS_SELECTED, LVIS_SELECTED);
		// 获取选中行索引（无选中返回-1）
		int nSelRow = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
		// 启用复选框（需先加扩展样式 LVS_EX_CHECKBOXES）
		ListView_SetItemState(hList, nRow, LVIS_CHECKED, LVIS_CHECKED);
		*/

		break;
	}
	case WM_NOTIFY:		// 子窗口消息处理
	{
		NMHDR* pNH = (NMHDR*)lParam;
		if (pNH->idFrom == 1001) {
			switch (pNH->code) {
			case LVN_ITEMACTIVATE: // 双击行
			{
				int nRow = ((NMLISTVIEW*)lParam)->iItem;
				printf("双击了第%d行\n", nRow);
				break;
			}
			case LVN_COLUMNCLICK: // 点击列头（可触发排序）
				break;
			}
		}
		break;
	}
	case WM_COMMAND:		// 子窗口消息处理
	{
		switch (LOWORD(wParam))
		{
		case 1001:
		{


			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_CLOSE:				// 点右上角关闭按钮时发送此消息.
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return FALSE;		// 没处理交给默认,返回false
	}
	return TRUE; // 处理了返回true
}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 3.资源配置

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static LRESULT CALLBACK s2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:		// dlg初始化
	{
		// 设置图标
		HICON hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_TEST));
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);




		break;
	}
	case WM_CLOSE:				// 点右上角关闭按钮时发送此消息.
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return FALSE;		// 没处理交给默认,返回false
	}
	return TRUE; // 处理了返回true
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$



void p000_009()
{
	DLGPROC nmdlgProc = (DLGPROC)s2_1;

	HWND hDlg = CreateDialog(GetModuleHandle(0), (LPCTSTR)MAKEINTRESOURCEA(IDD_DIALOG_TEMPLATE), 0, (DLGPROC)nmdlgProc);
	ShowWindow(hDlg, SW_SHOW);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!IsDialogMessage(hDlg, &msg)) // 只处理这个对话框的消息
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}


