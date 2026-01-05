#include"common.h"
#include<math.h>

#define ID_TIMER    1
#define TWOPI    (2 * 3.14159)

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static void SetIsotropic(HDC hdc, int cxClient, int cyClient)
{
	SetMapMode(hdc, MM_ISOTROPIC);
	SetWindowExtEx(hdc, 1000, 1000, NULL);
	SetViewportExtEx(hdc, cxClient / 2, -cyClient / 2, NULL);
	SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL);
}

static void RotatePoint(POINT pt[], int iNum, int iAngle)
{
	int i;
	POINT ptTemp;

	for (i = 0; i < iNum; i++)
	{
		ptTemp.x = (int)(pt[i].x * cos(TWOPI * iAngle / 360) + pt[i].y * sin(TWOPI * iAngle / 360));
		ptTemp.y = (int)(pt[i].y * cos(TWOPI * iAngle / 360) - pt[i].x * sin(TWOPI * iAngle / 360));
		pt[i] = ptTemp;
	}
}

static void DrawClock(HDC hdc)
{
	int iAngle;
	POINT pt[3];

	for (iAngle = 0; iAngle < 360; iAngle += 6)
	{
		pt[0].x = 0;
		pt[0].y = 900;
		RotatePoint(pt, 1, iAngle);

		pt[2].x = pt[2].y = iAngle % 5 ? 33 : 100;

		pt[0].x -= pt[2].x / 2;

		pt[0].y -= pt[2].y / 2;

		pt[1].x = pt[0].x + pt[2].x;

		pt[1].y = pt[0].y + pt[2].y;

		SelectObject(hdc, GetStockObject(BLACK_BRUSH));

		Ellipse(hdc, pt[0].x, pt[0].y, pt[1].x, pt[1].y);

	}
}

static void DrawHands(HDC hdc, SYSTEMTIME* pst, BOOL fChange)
{
	static POINT pt[3][5] =
	{
		0,-150,100,0,0,600,-100,0,0,-150,
		0,-200,50,0,0,800,-50,0,0,-200,
		0,0,0,0,0,0,0,0,0,800
	};

	int i, iAngle[3];
	POINT ptTemp[3][5];

	iAngle[0] = (pst->wHour * 30) % 360 + pst->wMinute / 2;
	iAngle[1] = pst->wMinute * 6;
	iAngle[2] = pst->wSecond * 6;

	memcpy(ptTemp, pt, sizeof(pt));

	for (i = fChange ? 0 : 2; i < 3; i++)
	{
		RotatePoint(ptTemp[i], 5, iAngle[i]);
		Polyline(hdc, ptTemp[i], 5);
	}
}


static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxClient, cyClient;
	static SYSTEMTIME stPrevious;
	BOOL fChange;
	HDC hdc;
	PAINTSTRUCT ps;
	SYSTEMTIME st;

	switch (message)
	{
	case WM_CREATE:
		SetTimer(hwnd, ID_TIMER, 1000, NULL);
		GetLocalTime(&st);
		stPrevious = st;
		return 0;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;
	case WM_TIMER:
		GetLocalTime(&st);
		fChange = st.wHour != stPrevious.wHour || st.wMinute != stPrevious.wMinute;
		hdc = GetDC(hwnd);
		SetIsotropic(hdc, cxClient, cyClient);

		SelectObject(hdc, GetStockObject(WHITE_PEN));
		DrawHands(hdc, &stPrevious, fChange);

		SelectObject(hdc, GetStockObject(BLACK_PEN));
		DrawHands(hdc, &st, TRUE);

		ReleaseDC(hwnd, hdc);

		stPrevious = st;
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		SetIsotropic(hdc, cxClient, cyClient);
		DrawClock(hdc);
		DrawHands(hdc, &stPrevious, TRUE);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		KillTimer(hwnd, ID_TIMER);
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
	wcx.lpfnWndProc = WndProc;     // points to window procedure 
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


void p001_002()
{
	TestWinMain();
}