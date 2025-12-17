#include"../common.h"

#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//			    1. window 窗口界面基础

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 连接器>系统>子系统>窗口    就会使用WinMain作为入口函数,  如果是控制台就是使用main作为入口函数

static HINSTANCE g_hInstance;
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#define MENU_ID_BEGIN 100

// 模拟  win 桌面项目入口函数 
static int WINAPI TestWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;

	// 1. 注册窗口类
	WNDCLASSEX wcx = { 0 };

	TCHAR class_name[] = TEXT("test_window");
	wcx.cbSize = sizeof(wcx);					// size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW;		// redraw if size changes 
	wcx.lpfnWndProc = WindowProc;				// points to window procedure 
	wcx.cbClsExtra = 0;							// no extra class memory 
	wcx.cbWndExtra = 0;							// no extra window memory 
	wcx.hInstance = hInstance;					// handle to instance 
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);              // predefined app. icon 
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);	// predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // white background brush 
	wcx.lpszMenuName = TEXT("MainMenu");		// name of menu resource 
	wcx.lpszClassName = class_name;				// TEXT("MainWClass");  // name of window class 
	wcx.hIconSm = NULL;//LoadImage(hInstance, MAKEINTRESOURCE(5),IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),LR_DEFAULTCOLOR);
	ATOM a = RegisterClassEx(&wcx);
	if (!a)
	{
		mDbgPrint(TEXT("RegisterClassEx error: %d\n"), GetLastError());
		return 0;
	}

	// 2. 创建窗口,WM_CREATE消息在这里处理.
	HWND hwnd = CreateWindowEx(0, class_name, TEXT("窗口程序"), WS_OVERLAPPEDWINDOW, 0, 0, 300, 300, NULL, NULL, hInstance, NULL);
	if (NULL == hwnd)
	{
		mDbgPrint(TEXT("CreateWindowEx error: %d\n"), GetLastError());
		return 0;
	}

	// 3. 显示窗口
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);			    // SendMessage 发送 WM_PAINT消息

	// 4. 接受消息并处理
	MSG msg;
	BOOL bRet;
	//while ((bRet = GetMessage(&msg, hwnd, 0, 0)) != 0)	// 只接受主窗口的消息
	while ((bRet = GetMessage(&msg, NULL , 0, 0)) != 0)		// NULL 表示接受所有窗口的消息, 这样就可以处理子窗口的消息了.
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

// 时钟回调
static void CALLBACK TimerProc(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	printf("timer 2 \n");
}

// 消息处理函数
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;			// 设备上下文句柄
	PAINTSTRUCT ps;		// 绘图结构体
	static int cxClient, cyClient; // 客户区大小

	//mDbgPrint(TEXT("hwnd:%x\tmsg:%x\twParam:%x\tlParam:%x\n"), hwnd, msg, wParam, lParam);	// 调试输出消息信息

	switch (msg)
	{
	case WM_CREATE:		// 窗口创建后、窗口变为可见之前接收此消息,一般用来创建子按钮等初始化工作.  非队列消息
		PlaySound(TEXT("e:\\SOUNDS\\BELLS.WAV"), NULL, SND_SYNC);
		// =====
 
		// 创建控件位置
		CreateWindow(TEXT("BUTTON"), TEXT("按钮1"), WS_CHILD | WS_VISIBLE, 10, 10, 100, 30, hwnd, (HMENU)1001, g_hInstance, NULL);

		// =====
		break;
	case WM_PAINT:		// 窗口出现无效区域,就会收到PAINT消息进行重绘,  简单的理解: 需要重绘的时候重绘
	{
		hdc = BeginPaint(hwnd, &ps); // 使用背景画刷刷新背景
		//=====

		// 绘图位置

		//=====
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_ERASEBKGND: // 窗口将要擦除背景时接收此消息,一般用来绘制背景. 非队列消息
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	case WM_COMMAND:	                // 子窗口消息处理
	{
		switch (LOWORD(wParam))
		{
		case 1001:
			mDbgPrint(TEXT("点击按钮\n"));
			break;
		case MENU_ID_BEGIN + 0:
			printf("Menu %d \n", MENU_ID_BEGIN + 0);
			break;
		case MENU_ID_BEGIN + 1:
			printf("Menu %d \n", MENU_ID_BEGIN + 1);
			break;
		case MENU_ID_BEGIN + 2:
			printf("Menu %d \n", MENU_ID_BEGIN + 2);
			break;
		default:
			break;
		}
		break;
	}
	case WM_KEYDOWN:
	{
		SetTimer(hwnd, 1, 3000, NULL);						// 每3秒发出一个WM_TIMER
		printf("set timer 1\n");

		SetTimer(hwnd, 2, 3000, (TIMERPROC)TimerProc);		// 每3秒调用一次回调
		printf("set timer 2\n");

		// KillTimer(hwnd,1);

		break;
	}
	case WM_CHAR:    // TranslateMessage 的作用就是在得到键盘消息后,生成一个字符消息. 没这个操作就没有WM_CHAR消息.
	{
		IsWindowUnicode(hwnd) ? printf("Unicode\n") : printf("Ansi\n");
		switch (wParam)
		{
		case 'a':
			printf("a\n");
			break;
		default:
			break;
		}
		break;
	}
	case WM_TIMER:
	{
		if (wParam == 1)
		{
			printf("timer 1\n");
		}
		break;
	}

	case WM_SIZE:
	{
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		printf("%d %d\n", LOWORD(lParam), HIWORD(lParam));
		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (wParam & MK_CONTROL)		// 判断ctrl是否按下
			printf("ctrl\n");

		if (GetKeyState(VK_CONTROL) < 0)// 判断ctrl是否按下
			printf("ctrl\n");

		// SHORT state = GetKeyState(VK_SHIFT);	// 键状态查询
		// printf ("VK_SHIFT : %d\n", state);

		POINT pt;
		GetCursorPos(&pt);				// 屏幕坐标
		printf("%d %d\n", pt.x, pt.y);
		SetCursorPos(pt.x + 10, pt.y + 10);

		printf("%d %d\n", LOWORD(lParam), HIWORD(lParam));		// 工作区坐标

		break;
	}
	case WM_RBUTTONDOWN:
	{
		printf("------------");
		// 创建弹出菜单
		HMENU hMenu = CreatePopupMenu();
		AppendMenuA(hMenu, MF_STRING, MENU_ID_BEGIN + 0, "Item 1");
		AppendMenuA(hMenu, MF_STRING, MENU_ID_BEGIN + 1, "Item 2");
		AppendMenuA(hMenu, MF_STRING, MENU_ID_BEGIN + 2, "Item 3");

		// 在鼠标位置显示弹出菜单
		POINT pt;
		GetCursorPos(&pt);
		TrackPopupMenu(hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);

		// 清理弹出菜单资源
		//DestroyMenu(hMenu);
		break;
	}
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCHAR:
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam) == SC_CLOSE)					// 右上角关闭按钮先发送 WM_SYSCOMMAND,默认处理程序发出WM_CLOSE
			printf("close sys command\n");
		return DefWindowProc(hwnd, msg, wParam, lParam);		// 默认消息的默认处理,交给windows来解决
		break;
	case WM_SETFOCUS:		// 当窗口获得焦点时,发送WM_SETFOCUS消息. 失去焦点时,发送WM_KILLFOCUS
		printf("set focus\n");
		break;
	case WM_KILLFOCUS:
		printf("kill focus\n");
		break;
	case WM_CLOSE:
		if (IDYES == MessageBox(hwnd, TEXT("是否关闭窗口"), 0, MB_YESNO))
			DestroyWindow(hwnd); // 销毁窗口,然后发送一个WM_DESTROY
		break;
	case WM_DESTROY:	            // 右上角的×不是关闭进程,而是发出一个WM_DESCTROY消息.
		PostQuitMessage(0);         // 发一个WM_QUIT消息
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);		// 默认消息的默认处理,交给windows来解决
		break;
	}

	return 0;
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_005()
{
	TestWinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_NORMAL);
}