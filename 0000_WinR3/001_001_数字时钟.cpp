#include"common.h"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				 数字时钟

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#define ID_TIMER 1

static void DisplayDigit(HDC hdc, int iNumber)
{
	static BOOL fSevenSegment[10][7] =
	{
		1, 1, 1, 0, 1, 1, 1,    //0
		0, 0, 1, 0, 0, 1, 0,    //1
		1, 0, 1, 1, 1, 0, 1,    //2
		1, 0, 1, 1, 0, 1, 1,    //3
		0, 1, 1, 1, 0, 1, 0,    //4
		1, 1, 0, 1, 0, 1, 1,    //5
		1, 1, 0, 1, 1, 1, 1,    //6
		1, 0, 1, 0, 0, 1, 0,    //7
		1, 1, 1, 1, 1, 1, 1,    //8
		1, 1, 1, 1, 0, 1, 1     //9
	};

	static POINT ptSegment[7][6] =
	{
		7, 6, 11, 2, 31, 2,	35,	6, 31, 10, 11, 10,
		6, 7, 10, 11, 10, 31,6, 35, 2, 31, 2, 11,
		36, 7, 40, 11, 40, 31,	36, 35, 32, 31, 32, 11,
		7, 36, 11, 32, 31, 32,	35, 36, 31, 40, 11, 40,
		6, 37, 10, 41, 10, 61,	6, 65, 2, 61, 2, 41,
		36, 37, 40, 41, 40, 61,	36, 65, 32, 61, 32, 41,
		7, 66, 11, 62, 31, 62,	35, 66, 31, 70, 11, 70
	};

	for (int iSeg(0); iSeg != 7; ++iSeg)
	{
		if (fSevenSegment[iNumber][iSeg])
			Polygon(hdc, ptSegment[iSeg], 6);
	}
}

static void DisplayTwoDigits(HDC hdc, int iNumber, BOOL fSuppress)
{
	if (!fSuppress || (iNumber / 10 != 0))
		DisplayDigit(hdc, iNumber / 10);

	OffsetWindowOrgEx(hdc, -42, 0, NULL);  // 移动绘图原点. 再在(0,0)绘制相当于(42,0)处绘制.
	DisplayDigit(hdc, iNumber % 10);
	OffsetWindowOrgEx(hdc, -42, 0, NULL);  // 绘图原点的变化不会影响 逻辑原点到视口原点的映射.
}

static void DisplayColon(HDC hdc)
{
	POINT ptColon[2][4] = { 2, 21, 6, 17, 10, 21, 6, 25,2, 51, 6, 47, 10, 51, 6, 55 };

	Polygon(hdc, ptColon[0], 4);
	Polygon(hdc, ptColon[1], 4);

	OffsetWindowOrgEx(hdc, -12, 0, NULL);
}

static void DisplayTime(HDC hdc, BOOL f24Hour, BOOL fSuppress)
{
	SYSTEMTIME st;

	GetLocalTime(&st);

	if (f24Hour)
		DisplayTwoDigits(hdc, st.wHour, fSuppress);
	else
		DisplayTwoDigits(hdc, (st.wHour %= 12) ? st.wHour : 12, fSuppress);
	//return;
	DisplayColon(hdc);
	DisplayTwoDigits(hdc, st.wMinute, FALSE);
	DisplayColon(hdc);
	DisplayTwoDigits(hdc, st.wSecond, FALSE);
}

static LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL f24Hour, fSuppress;
	static HBRUSH hBrushRed;
	static int cxClient, cyClient;
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR szBuffer[2];

	switch (message)
	{
	case WM_CREATE:
		hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
		SetTimer(hwnd, ID_TIMER, 1000, NULL);			// 每个1秒一个 WM_TIMER
		//fall through
	case WM_SETTINGCHANGE:
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuffer, 2);
		f24Hour = (szBuffer[0] == TEXT('1'));	// 24小时制，则为1

		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szBuffer, 2);
		fSuppress = (szBuffer[0] == TEXT('0')); // 是否省略前导0，则为1

		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;
	case WM_TIMER:
		InvalidateRect(hwnd, NULL, TRUE);	// 每次定时器到时，重绘窗口
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		SetMapMode(hdc, MM_ISOTROPIC);                             // 自定义映射模式
		SetWindowExtEx(hdc, 276, 72, NULL);                        // 逻辑范围
		SetWindowOrgEx(hdc, 138, 36, NULL);                        // 逻辑点 (138,36) 映射到 视口原点

		SetViewportExtEx(hdc, cxClient / 2, cyClient / 2, NULL);   // 视口范围
		SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL);   // 设备点(cxClient / 2, cyClient / 2) 作为视口(0,0)点

		SelectObject(hdc, GetStockObject(NULL_PEN));
		SelectObject(hdc, hBrushRed);

		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = 10;
		rect.bottom = 10;
		FillRect(hdc, &rect, hBrushRed);

		OffsetWindowOrgEx(hdc, -20, 0, NULL);

		rect.left = 0;
		rect.top = 0;
		rect.right = 10;
		rect.bottom = 10;
		FillRect(hdc, &rect, CreateSolidBrush(RGB(0, 255, 0)));

		DisplayTime(hdc, f24Hour, fSuppress);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		KillTimer(hwnd, ID_TIMER);
		DeleteObject(hBrushRed);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

static int TestWinMain()
{
	// 1. 注册窗口类
	WNDCLASSEX wcx = { 0 };

	TCHAR class_name[] = TEXT("test_window");
	wcx.cbSize = sizeof(wcx);          // size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW; // redraw if size changes 
	wcx.lpfnWndProc = WinProc;     // points to window procedure 
	wcx.cbClsExtra = 0;                // no extra class memory 
	wcx.cbWndExtra = 0;                // no extra window memory 
	wcx.hInstance = GetModuleHandle(NULL);         // handle to instance 
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);              // predefined app. icon 
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);// predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // white background brush 
	wcx.lpszMenuName = TEXT("MainMenu");    // name of menu resource 
	wcx.lpszClassName = class_name; // TEXT("MainWClass");  // name of window class 
	wcx.hIconSm = NULL;//LoadImage(hInstance, MAKEINTRESOURCE(5),IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),LR_DEFAULTCOLOR);

	ATOM a = RegisterClassEx(&wcx);
	if (!a)
	{
		mDbgPrint(TEXT("RegisterClassEx error: %d\n"), GetLastError());
		return 0;
	}

	// 2. 创建窗口
	HWND hwnd = CreateWindowEx(0, class_name, TEXT("窗口程序"), WS_OVERLAPPEDWINDOW, 0, 0, 300, 300, NULL, NULL, GetModuleHandle(NULL), NULL);
	if (NULL == hwnd)
	{
		mDbgPrint(TEXT("CreateWindowEx error: %d\n"), GetLastError());
		return 0;
	}

	// 3. 显示窗口
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);			    // 发送首个WM_PAINT消息

	// 4. 接受消息并处理
	MSG msg;
	BOOL bRet;
	while ((bRet = GetMessage(&msg, hwnd, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			mDbgPrint(TEXT("GetMessage error: %d\n"), GetLastError());
			break;
		}
		else
		{
			TranslateMessage(&msg); // 转换消息 : 将虚拟键代码消息转换为字符消息,也就是 'a' 等消息
			DispatchMessage(&msg);  // 分发消息 : 进到内核,从内核发起消息处理函数的调用. 
		}
	}

	return 0;
}

void p001_001()
{
	TestWinMain();
}