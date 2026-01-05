#include"common.h"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//			    滚动条 + 文本打印的使用

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#define NUMLINES ((int) (sizeof sysmetrics / sizeof sysmetrics [0]))

struct
{
	int     iIndex;
	const TCHAR* szLabel;
	const TCHAR* szDesc;
}sysmetrics[] =
{
SM_CXSCREEN,             TEXT("SM_CXSCREEN"),
TEXT("Screen width in pixels"),
SM_CYSCREEN,             TEXT("SM_CYSCREEN"),
TEXT("Screen height in pixels"),
SM_CXVSCROLL,            TEXT("SM_CXVSCROLL"),
TEXT("Vertical scroll width"),
SM_CYHSCROLL,            TEXT("SM_CYHSCROLL"),
TEXT("Horizontal scroll height"),
SM_CYCAPTION,            TEXT("SM_CYCAPTION"),
TEXT("Caption bar height"),
SM_CXBORDER,             TEXT("SM_CXBORDER"),
TEXT("Window border width"),
SM_CYBORDER,             TEXT("SM_CYBORDER"),
TEXT("Window border height"),
SM_CXFIXEDFRAME,         TEXT("SM_CXFIXEDFRAME"),
TEXT("Dialog window frame width"),
SM_CYFIXEDFRAME,         TEXT("SM_CYFIXEDFRAME"),
TEXT("Dialog window frame height"),
SM_CYVTHUMB,             TEXT("SM_CYVTHUMB"),
TEXT("Vertical scroll thumb height"),
SM_CXHTHUMB,             TEXT("SM_CXHTHUMB"),
TEXT("Horizontal scroll thumb width"),
SM_CXICON,               TEXT("SM_CXICON"),
TEXT("Icon width"),
SM_CYICON,               TEXT("SM_CYICON"),
TEXT("Icon height"),
SM_CXCURSOR,             TEXT("SM_CXCURSOR"),
TEXT("Cursor width"),
SM_CYCURSOR,             TEXT("SM_CYCURSOR"),
TEXT("Cursor height"),
SM_CYMENU,               TEXT("SM_CYMENU"),
TEXT("Menu bar height"),
SM_CXFULLSCREEN,         TEXT("SM_CXFULLSCREEN"),
TEXT("Full screen client area width"),
SM_CYFULLSCREEN,         TEXT("SM_CYFULLSCREEN"),
TEXT("Full screen client area height"),
SM_CYKANJIWINDOW,        TEXT("SM_CYKANJIWINDOW"),
TEXT("Kanji window height"),
SM_MOUSEPRESENT,         TEXT("SM_MOUSEPRESENT"),
TEXT("Mouse present flag"),
SM_CYVSCROLL,            TEXT("SM_CYVSCROLL"),
TEXT("Vertical scroll arrow height"),
SM_CXHSCROLL,            TEXT("SM_CXHSCROLL"),
TEXT("Horizontal scroll arrow width"),
SM_DEBUG,                TEXT("SM_DEBUG"),
TEXT("Debug version flag"),
SM_SWAPBUTTON,           TEXT("SM_SWAPBUTTON"),
TEXT("Mouse buttons swapped flag"),
SM_CXMIN,                TEXT("SM_CXMIN"),
TEXT("Minimum window width"),
SM_CYMIN,                TEXT("SM_CYMIN"),
TEXT("Minimum window height"),
SM_CXSIZE,               TEXT("SM_CXSIZE"),
TEXT("Min/Max/Close button width"),
SM_CYSIZE,               TEXT("SM_CYSIZE"),
TEXT("Min/Max/Close button height"),
SM_CXSIZEFRAME,          TEXT("SM_CXSIZEFRAME"),
TEXT("Window sizing frame width"),
SM_CYSIZEFRAME,          TEXT("SM_CYSIZEFRAME"),
TEXT("Window sizing frame height"),
SM_CXMINTRACK,           TEXT("SM_CXMINTRACK"),
TEXT("Minimum window tracking width"),
SM_CYMINTRACK,           TEXT("SM_CYMINTRACK"),
TEXT("Minimum window tracking height"),
SM_CXDOUBLECLK,          TEXT("SM_CXDOUBLECLK"),
TEXT("Double click x tolerance"),
SM_CYDOUBLECLK,          TEXT("SM_CYDOUBLECLK"),
TEXT("Double click y tolerance"),
SM_CXICONSPACING,        TEXT("SM_CXICONSPACING"),
TEXT("Horizontal icon spacing"),
SM_CYICONSPACING,        TEXT("SM_CYICONSPACING"),
TEXT("Vertical icon spacing"),
SM_MENUDROPALIGNMENT,    TEXT("SM_MENUDROPALIGNMENT"),
TEXT("Left or right menu drop"),
SM_PENWINDOWS,           TEXT("SM_PENWINDOWS"),
TEXT("Pen extensions installed"),
SM_DBCSENABLED,          TEXT("SM_DBCSENABLED"),
TEXT("Double-Byte Char Set enabled"),
SM_CMOUSEBUTTONS,        TEXT("SM_CMOUSEBUTTONS"),
TEXT("Number of mouse buttons"),
SM_SECURE,               TEXT("SM_SECURE"),
TEXT("Security present flag"),
SM_CXEDGE,               TEXT("SM_CXEDGE"),
TEXT("3-D border width"),
SM_CYEDGE,               TEXT("SM_CYEDGE"),
TEXT("3-D border height"),
SM_CXMINSPACING,         TEXT("SM_CXMINSPACING"),
TEXT("Minimized window spacing width"),
SM_CYMINSPACING,         TEXT("SM_CYMINSPACING"),
TEXT("Minimized window spacing height"),
SM_CXSMICON,             TEXT("SM_CXSMICON"),
TEXT("Small icon width"),
SM_CYSMICON,             TEXT("SM_CYSMICON"),
TEXT("Small icon height"),
SM_CYSMCAPTION,          TEXT("SM_CYSMCAPTION"),
TEXT("Small caption height"),
SM_CXSMSIZE,             TEXT("SM_CXSMSIZE"),
TEXT("Small caption button width"),
SM_CYSMSIZE,             TEXT("SM_CYSMSIZE"),
TEXT("Small caption button height"),
SM_CXMENUSIZE,           TEXT("SM_CXMENUSIZE"),
TEXT("Menu bar button width"),
SM_CYMENUSIZE,           TEXT("SM_CYMENUSIZE"),
TEXT("Menu bar button height"),
SM_ARRANGE,              TEXT("SM_ARRANGE"),
TEXT("How minimized windows arranged"),
SM_CXMINIMIZED,          TEXT("SM_CXMINIMIZED"),
TEXT("Minimized window width"),
SM_CYMINIMIZED,          TEXT("SM_CYMINIMIZED"),
TEXT("Minimized window height"),
SM_CXMAXTRACK,           TEXT("SM_CXMAXTRACK"),
TEXT("Maximum draggable width"),
SM_CYMAXTRACK,           TEXT("SM_CYMAXTRACK"),
TEXT("Maximum draggable height"),
SM_CXMAXIMIZED,          TEXT("SM_CXMAXIMIZED"),
TEXT("Width of maximized window"),
SM_CYMAXIMIZED,          TEXT("SM_CYMAXIMIZED"),
TEXT("Height of maximized window"),
SM_NETWORK,              TEXT("SM_NETWORK"),
TEXT("Network present flag"),
SM_CLEANBOOT,            TEXT("SM_CLEANBOOT"),
TEXT("How system was booted"),
SM_CXDRAG,               TEXT("SM_CXDRAG"),
TEXT("Avoid drag x tolerance"),
SM_CYDRAG,               TEXT("SM_CYDRAG"),
TEXT("Avoid drag y tolerance"),
SM_SHOWSOUNDS,           TEXT("SM_SHOWSOUNDS"),
TEXT("Present sounds visually"),
SM_CXMENUCHECK,          TEXT("SM_CXMENUCHECK"),
TEXT("Menu check-mark width"),
SM_CYMENUCHECK,          TEXT("SM_CYMENUCHECK"),
TEXT("Menu check-mark height"),
SM_SLOWMACHINE,          TEXT("SM_SLOWMACHINE"),
TEXT("Slow processor flag"),
SM_MIDEASTENABLED,       TEXT("SM_MIDEASTENABLED"),
TEXT("Hebrew and Arabic enabled flag"),
SM_MOUSEWHEELPRESENT,    TEXT("SM_MOUSEWHEELPRESENT"),
TEXT("Mouse wheel present flag"),
SM_XVIRTUALSCREEN,       TEXT("SM_XVIRTUALSCREEN"),
TEXT("Virtual screen x origin"),
SM_YVIRTUALSCREEN,       TEXT("SM_YVIRTUALSCREEN"),
TEXT("Virtual screen y origin"),
SM_CXVIRTUALSCREEN,      TEXT("SM_CXVIRTUALSCREEN"),
TEXT("Virtual screen width"),
SM_CYVIRTUALSCREEN,      TEXT("SM_CYVIRTUALSCREEN"),
TEXT("Virtual screen height"),
SM_CMONITORS,            TEXT("SM_CMONITORS"),
TEXT("Number of monitors"),
SM_SAMEDISPLAYFORMAT,    TEXT("SM_SAMEDISPLAYFORMAT"),
TEXT("Same color format flag")
};

static LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

static int TestWinMain()
{
	// 1. 注册窗口类
	WNDCLASSEX wcx = { 0 };

	TCHAR class_name[] = TEXT("test_window");
	wcx.cbSize = sizeof(wcx);          // size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW; // redraw if size changes  水平垂直重绘
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

static LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int  cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth;
	HDC         hdc;
	int         i, x, y, iVertPos, iHorzPos, iPaintBeg, iPaintEnd;
	PAINTSTRUCT ps;
	SCROLLINFO  si;
	TCHAR       szBuffer[10];
	TEXTMETRIC  tm;

	switch (message)
	{
	case WM_CREATE:
		//获取窗口的设备环境句柄
		hdc = GetDC(hwnd);

		//获取默认系统字体的尺寸
		GetTextMetrics(hdc, &tm);

		//获取平均字符宽度
		cxChar = tm.tmAveCharWidth;

		//tmPitchAndFamily字段的低位决定字体是否为等宽字体：1表示变宽字体，0表示等宽字体
		//大写字符的平均宽度。在等宽字体中，cxCaps = cyChar。在变宽字体中，cxCaps = cyChar * 1.5
		cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
		//获取字符的总高度
		cyChar = tm.tmHeight + tm.tmExternalLeading;
		//释放窗口的设备环境句柄
		ReleaseDC(hwnd, hdc);

		// 保存三列的总宽度
		iMaxWidth = 40 * cxChar + 22 * cxCaps;
		return 0;

	case WM_SIZE:  //	WM_SIZE消息：窗口大小改变时，会收到此消息
		//lParam变量的低位是客户区的宽度
		cxClient = LOWORD(lParam);
		//lParam变量的高位是客户区的高度
		cyClient = HIWORD(lParam);

		// Set vertical scroll bar range and page size

		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;            // 制定滚动条的范围和页面大小
		si.nMin = 0;                                // 范围最小值
		si.nMax = NUMLINES - 1;                     // 范围最大值
		si.nPage = cyClient / cyChar;               // 页面大小,一页多少行      // 滚动最远距离是 nMax-nPage+1 所以能显示到最后一行.
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

		// Set horizontal scroll bar range and page size

		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = 2 + iMaxWidth / cxChar;
		si.nPage = cxClient / cxChar;
		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		return 0;

		//垂直滚动
	case WM_VSCROLL:
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_VERT, &si);

		// Save the position for comparison later on
		iVertPos = si.nPos;

		//wParam参数的低位代表鼠标在滚动条上的动作，这个值被称为“通知码”
		switch (LOWORD(wParam))
		{
		case SB_TOP:
			si.nPos = si.nMin;
			break;

		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;

		case SB_LINEUP:
			si.nPos -= 1;
			break;

		case SB_LINEDOWN:
			si.nPos += 1;
			break;

		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;

		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;
		case SB_THUMBTRACK:		//SB_THUMBTRACK通知码：当用户拖动滚动条的滑块时，程序会相应的滚动窗口的内容
			//返回当前滑块的位置
			si.nPos = si.nTrackPos;
			break;

		default:
			break;
		}
		// Set the position and then retrieve it.  Due to adjustments
		//   by Windows it may not be the same as the value set.

		si.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hwnd, SB_VERT, &si);

		// If the position has changed, scroll the window and update it

		if (si.nPos != iVertPos)
		{
			ScrollWindow(hwnd, 0, cyChar * (iVertPos - si.nPos), NULL, NULL);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}
		return 0;
		//水平滚动
	case WM_HSCROLL:
		// Get all the vertial scroll bar information

		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;

		// Save the position for comparison later on

		GetScrollInfo(hwnd, SB_HORZ, &si);
		iHorzPos = si.nPos;

		switch (LOWORD(wParam))
		{
		case SB_LINELEFT:
			si.nPos -= 1;
			break;

		case SB_LINERIGHT:
			si.nPos += 1;
			break;

		case SB_PAGELEFT:
			si.nPos -= si.nPage;
			break;

		case SB_PAGERIGHT:
			si.nPos += si.nPage;
			break;
		case SB_THUMBTRACK:
			//返回当前滑块的位置
			si.nPos = si.nTrackPos;
			break;
			//SB_THUMBPOSITION通知码:当用户拖动滚动条的滑块，程序只有在用户松开了鼠标键时才会滚动窗口的内容
		case SB_THUMBPOSITION:
			si.nPos = si.nTrackPos;      //返回当前滑块的位置
			break;

		default:
			break;
		}
		// Set the position and then retrieve it.  Due to adjustments
		// by Windows it may not be the same as the value set.

		si.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		GetScrollInfo(hwnd, SB_HORZ, &si);

		// If the position has changed, scroll the window 

		if (si.nPos != iHorzPos)
		{
			ScrollWindow(hwnd, cxChar * (iHorzPos - si.nPos), 0, NULL, NULL);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}
		return 0;

	case WM_PAINT:
		//获取设备句柄
		hdc = BeginPaint(hwnd, &ps);

		// Get vertical scroll bar position

		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		GetScrollInfo(hwnd, SB_VERT, &si);
		iVertPos = si.nPos;

		// Get horizontal scroll bar position

		GetScrollInfo(hwnd, SB_HORZ, &si);
		iHorzPos = si.nPos;

		// Find painting limits

		iPaintBeg = max(0, iVertPos + ps.rcPaint.top / cyChar);
		iPaintEnd = min(NUMLINES - 1, iVertPos + ps.rcPaint.bottom / cyChar);

		for (i = iPaintBeg; i <= iPaintEnd; i++)
		{
			x = cxChar * (1 - iHorzPos);
			y = cyChar * (i - iVertPos);

			TextOut(hdc, x, y, sysmetrics[i].szLabel, lstrlen(sysmetrics[i].szLabel));

			TextOut(hdc, x + 22 * cxCaps, y, sysmetrics[i].szDesc, lstrlen(sysmetrics[i].szDesc));

			SetTextAlign(hdc, TA_RIGHT | TA_TOP);    //设置为右上对齐

			TextOut(hdc, x + 22 * cxCaps + 40 * cxChar, y, szBuffer, wsprintf(szBuffer, TEXT("%5d"), GetSystemMetrics(sysmetrics[i].iIndex)));

			SetTextAlign(hdc, TA_LEFT | TA_TOP);     //设置为默认左上对齐
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_HOME:
			SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
			break;
		case VK_END:
			SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
			break;
		case VK_PRIOR:
			SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
			break;
		case VK_NEXT:
			SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
			break;
		case VK_UP:
			SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
			break;
		case VK_DOWN:
			SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
			break;
		case VK_LEFT:
			SendMessage(hwnd, WM_HSCROLL, SB_PAGEUP, 0);
			break;
		case VK_RIGHT:
			SendMessage(hwnd, WM_HSCROLL, SB_PAGEDOWN, 0);
			break;
		default:
			break;
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		short d = HIWORD(wParam);
		printf("%d\n", d);
		if (d > 0)
			SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
		else
			SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void p001_000()
{
	TestWinMain();
}
