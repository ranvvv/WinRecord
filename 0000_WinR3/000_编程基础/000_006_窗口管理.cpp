#include"../common.h"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//			    1. 消息处理

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s1()
{
	HWND hwnd;
	SendMessage(hwnd, WM_CLOSE, 0, 0);
	PostMessage(hwnd, WM_CLOSE, 0, 0);
	keybd_event(97, 0, KEYEVENTF_KEYUP, 0);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 300, 300, 0, 0);
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//			    2. 鼠标

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s2()
{
	HWND	hwnd;
	POINT pt;
	ShowCursor(FALSE);
	GetCursorPos(&pt);
	SetCursorPos(1, 2);

	SetCapture(hwnd);
	ReleaseCapture();

	mouse_event(MOUSEEVENTF_LEFTUP, 300, 300, 0, 0); // 模拟鼠标左键抬起

}
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//			    3. 键盘

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s3()
{
	keybd_event(97, 0, 0, 0);
	keybd_event(97, 0, KEYEVENTF_KEYUP, 0);

}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//			    4. 窗口

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s4()
{
	HWND hwnd;
	POINT pt;

	ScreenToClient(hwnd, &pt);						// 屏幕坐标转客户区
	ClientToScreen(hwnd, &pt);						// 客户区坐标转屏幕

	// ================  窗口状态
	ShowWindow(hwnd, SW_SHOW);						// 显示窗口
	EnableWindow(hwnd, FALSE);						// 禁用窗口
	IsWindowVisible(hwnd);							// 判断窗口是否可见
	IsWindowEnabled(hwnd);							// 判断窗口是否可用	
	SwitchToThisWindow(hwnd, TRUE);				// 激活窗口

   // ================  窗口风格
	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	printf("style %x\n", style);
	LONG styleExt = GetWindowLong(hwnd, GWL_EXSTYLE);	// 扩展风格
	printf("styleExt %x\n", styleExt);
	SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	GetClassName(hwnd, NULL, 0);
	WNDCLASS wcx;
	GetClassInfo(GetModuleHandle(NULL), TEXT("test_window"), &wcx);

	// ===============  窗口位置
	RECT rect;
	GetWindowRect(hwnd, &rect);											// 窗口矩形区域
	GetClientRect(hwnd, &rect);											// 客户区矩形
	printf("Window RECT %d %d %d %d\n", rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	SetWindowPos(hwnd, NULL, rect.left + 20, rect.top + 20, 0, 0, SWP_NOSIZE);					// 设置窗口位置--移动,不改变大小
	SetWindowPos(hwnd, NULL, rect.left + 100, rect.top + 200, 640, 480, SWP_SHOWWINDOW);		// 设置窗口位置--移动,改变大小
	MoveWindow(hwnd, rect.left + 100, rect.top + 200, 640, 480, TRUE);				// 移动,改变大小

	SetForegroundWindow(hwnd);					// 激活窗口

	// =============== 窗口焦点
	HWND hFocus = GetFocus(); 			// 获取键盘焦点窗口
	SetFocus(hwnd);						// 捕获键盘输入
	SetCapture(hwnd);					// 捕获鼠标输入
	HWND hCapture = GetCapture();		// 获取捕获窗口
	ReleaseCapture();					// 释放鼠标输入

	// ================ 坐标
	ScreenToClient(hwnd, &pt);			// 客户区坐标转屏幕
	ClientToScreen(hwnd, &pt);			// 屏幕坐标转客户区


	// ================ 窗口标题
	SetWindowText(hwnd, TEXT("Hello"));
	TCHAR title[1024];
	GetWindowText(hwnd, title, 1024);
	int len = GetWindowTextLength(hwnd);
	_tprintf(TEXT("title %s len %d\n"), title, len);


	// ================ 资源
	//LoadResource(GetModuleHandle(0), FindResource());

	// =============== 窗口查找
	GetParent(hwnd);								// 父窗口	
	GetWindowLong(hwnd, GWLP_HWNDPARENT);			// 父窗口句柄
	GetDlgItem(hwnd, 1002);							// 子窗口
	GetDlgItemText(hwnd, 1002, title, 1024);		// 子窗口文本
	SetDlgItemText(hwnd, 1002, TEXT("Hello"));		// 子窗口文本
	GetDlgCtrlID(hwnd);								// 子窗口ID


	HWND hwndDesktop = GetDesktopWindow();									// 桌面句柄
	HWND hwndTop = FindWindow(TEXT("WinDbgFrameClass"), NULL);				// 只找顶层窗口, 也就是桌面窗口下的窗口
	HWND hwndChild = FindWindowEx(hwndTop, NULL, TEXT("DockClass"), NULL);	// 查找子窗口
	HWND hwndChildItem = GetDlgItem(hwndTop, 0x64);							// 查找子窗口
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				5. 遍历窗口

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	printf("hwnd 0x%08X  lparam:0x%08X\n", (UINT32)(UINT64)hwnd, (UINT32)lParam);
	return TRUE;
}

static void s5()
{
	HWND hwndDesktop = GetDesktopWindow();
	EnumChildWindows(hwndDesktop, EnumWindowsProc, 0x111);
}

static void s6()
{
	FindWindow(NULL, NULL);
	FindWindowEx(NULL, NULL, NULL, NULL);
	GetDlgItem(NULL, 0);
	EnumWindows(EnumWindowsProc, 0);
	EnumChildWindows(NULL, NULL, 0);

	HMODULE h = LoadLibrary(TEXT("user32.dll"));
	GetProcAddress(h, "SwitchToThisWindow");
	SwitchToThisWindow(NULL, TRUE);

}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_006()
{


}