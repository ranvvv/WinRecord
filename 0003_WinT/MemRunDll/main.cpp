#include<windows.h>
#include<stdio.h>
#include<winuser.h>
#include"resource.h"

static void s_template(HINSTANCE base);

static LRESULT CALLBACK mdlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_INITDIALOG:
		printf("WM_INITDIALOG lParma : %08X\n", (int)lParam);
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case 1001:
			break;
		default:
			break;
		}
		break;
	}
	case WM_CLOSE:
		EndDialog(hwnd, 0x1111);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

EXTERN_C __declspec(dllexport)  int EntryFunc(PVOID ImageBase)
{
	MessageBox(NULL, L"test", L"dll", 0);

	INT_PTR result = DialogBoxParam((HINSTANCE)ImageBase, (LPCTSTR)MAKEINTRESOURCEA(IDD_DIALOG_MAIN), 0, (DLGPROC)mdlgProc, 0x12345678);
	char str[1024] = { 0 };
	sprintf_s(str, 1024, "result:%d  %d", (int)result, GetLastError());
	MessageBoxA(NULL, str, "dll", 0);

	//s_template((HINSTANCE)ImageBase);  // x64版本  CreateWindow会出错. 但是DialogParam不会.


	return 10;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


//static HINSTANCE g_hInstance;
static LRESULT CALLBACK WindowProc_template(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static int WINAPI WinMain_template(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//g_hInstance = hInstance;

	// 1. 注册窗口类
	WNDCLASSEX wcx = { 0 };
	TCHAR class_name[] = TEXT("test_window");
	wcx.cbSize = sizeof(wcx);          // size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW; // redraw if size changes 
	wcx.lpfnWndProc = WindowProc_template;     // points to window procedure 
	wcx.cbClsExtra = 0;                // no extra class memory 
	wcx.cbWndExtra = 0;                // no extra window memory 
	wcx.hInstance = hInstance;         // handle to instance 
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);              // predefined app. icon 
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);// predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // white background brush 
	wcx.lpszMenuName = TEXT("MainMenu");    // name of menu resource 
	wcx.lpszClassName = class_name; // TEXT("MainWClass");  // name of window class 
	wcx.hIconSm = NULL;//LoadImage(hInstance, MAKEINTRESOURCE(5),IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),LR_DEFAULTCOLOR);
	ATOM a = RegisterClassEx(&wcx);
	if (!a)
	{
		return 0;
	}

	// 2. 创建窗口
	HWND hwnd = CreateWindowEx(0, class_name, TEXT("窗口程序"), WS_OVERLAPPEDWINDOW, 0, 0, 300, 300, NULL, NULL, hInstance, NULL);
	if (NULL == hwnd)
	{
		return 0;
	}

	// 3. 显示窗口
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);			    // 发送一个WM_PAINT消息

	// 4. 接受消息并处理
	MSG msg;
	BOOL bRet;
	while ((bRet = GetMessage(&msg, hwnd, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
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

static LRESULT CALLBACK WindowProc_template(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int cxClient, cyClient;
	HDC hdc;
	PAINTSTRUCT ps;

	switch (msg)
	{
	case WM_CREATE:
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case 1001:
			break;
		default:
			break;
		}
		break;
	}
	case WM_KEYDOWN:
	{
		break;
	}
	case WM_CHAR:
		break;
	case WM_SIZE:
	{
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		printf("%d %d\n", LOWORD(lParam), HIWORD(lParam));
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	}
	return 0;
}

static void s_template(HINSTANCE base)
{
	LPSTR lpCmdLine = GetCommandLineA();
	WinMain_template(base, NULL, lpCmdLine, 10);
}


