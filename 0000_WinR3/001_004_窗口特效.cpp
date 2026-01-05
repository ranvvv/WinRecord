#include"common.h"


//				1. 窗口抖动
//				2. 窗口淡出





// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				1. 窗口抖动

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void ShakeWindow(HWND hwnd) {
	RECT rect;
	for (int i = 0; i < 16; i++) {
		GetWindowRect(hwnd, &rect); // 获取窗口矩形
		MoveWindow(hwnd, rect.left + 4 * (i % 2 == 0 ? -1 : 1), // 水平方向偏移
			rect.top + 4 * (i % 2 == 0 ? -1 : 1), // 垂直方向偏移
			rect.right - rect.left, rect.bottom - rect.top, TRUE);
		Sleep(30); // 暂停30毫秒
	}
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				2. 窗口淡出

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void FadeOutWindow(HWND hwnd)
{

	DWORD dwDuration = 2000; // 持续时间（毫秒）
	// AW_BLEND;		// 使用淡入淡出效果
	// AW_ACTIVATE;		// 淡入
	// AW_HIDE;			// 淡出
	AnimateWindow(hwnd, dwDuration, AW_HIDE | AW_BLEND);
}








static LRESULT CALLBACK nmdlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// m_debugPrint(TEXT("hwnd:%x\tmsg:%x\twParam:%x\tlParam:%x\n"),hwnd,msg,wParam,lParam);

	// 消息处理
	switch (msg)
	{
	case WM_INITDIALOG:		// dlg初始化

		break;
	case WM_COMMAND:	// 子窗口消息处理
	{
		// 根据dwTime的值决定是淡入还是淡出


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
	case WM_LBUTTONDOWN:
		ShakeWindow(hwnd);
		FadeOutWindow(hwnd);
		break;
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

void p001_004()
{
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