#define _CRT_SECURE_NO_WARNINGS 1
#include<windows.h>
#include<ShlObj.h>
#include<shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include<stdio.h>

#define MWIN_CLASS_NAME TEXT("Driver_Service_Loader")
#define MWIN_TITLE TEXT("Loader")
#define MODEL_TEXT "*.sys"

#define ID_BUTTON_INSTALL		1001
#define ID_BUTTON_RUN			1002
#define ID_BUTTON_STOP			1003
#define ID_BUTTON_UNINSTALL		1004
#define ID_BUTTON_SELECT_FILE	1005
#define ID_BUTTON_SERVICE		1006
#define ID_BUTTON_DRIVER		1007
#define ID_EDIT_TEXT			2001
#define ID_EDIT_INFO			2002 

#define MARGIN					10
#define BUTTON_WIDTH			100
#define BUTTON_HEIGHT			30
#define EDIT_HEIGHT				30

void viewInit(HWND hwnd);			// 界面初始化

HWND g_hButton_install;
HWND g_hButton_run;
HWND g_hButton_stop;
HWND g_hButton_uninstall;
HWND g_hButton_select_file;
HWND g_hButton_driver;
HWND g_hButton_service;
HWND g_hEdit_text;
HWND g_hEdit_info;

int g_window_width = MARGIN * 5 + BUTTON_WIDTH * 4;
int g_window_height = 300;
int g_type = 0;						// 文件类型: 0 service, 1 driver

void getFile();						// 选择文件
void install();						// 安全驱动/服务
void run();							// 运行
void stop();						// 停止
void uninstall();					// 卸载

void RemoveExtension(char* path);	// 移除扩展名.
void RemoveExtensionW(wchar_t* path);

HINSTANCE g_hInstance;
char text[0x1000];

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;

	// 1. 注册窗口类
	TCHAR class_name[] = MWIN_CLASS_NAME;
	WNDCLASSEX wcx;
	wcx.cbSize = sizeof(wcx);				// size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW;	// redraw if size changes 
	wcx.lpfnWndProc = WindowProc;			// points to window procedure 
	wcx.cbClsExtra = 0;						// no extra class memory 
	wcx.cbWndExtra = 0;						// no extra window memory 
	wcx.hInstance = hInstance;				// handle to instance 
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);  // predefined app. icon 
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);// predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // white background brush 
	wcx.lpszMenuName = TEXT("MainMenu");    // name of menu resource 
	wcx.lpszClassName = class_name; // TEXT("MainWClass");  // name of window class 
	wcx.hIconSm = NULL;

	ATOM a = RegisterClassEx(&wcx);
	if (!a)
		return 0;

	// 2. 创建窗口
	HWND hwnd = CreateWindowEx(WS_EX_ACCEPTFILES, class_name, MWIN_TITLE, WS_OVERLAPPEDWINDOW, 0, 0, g_window_width + 18, g_window_height, NULL, NULL, hInstance, NULL);
	if (NULL == hwnd)
		return 0;

	// 3. 显示窗口
	ShowWindow(hwnd, SW_SHOW);

	// 4. 接受消息并处理
	MSG msg;
	int bRet;
	while (bRet = GetMessage(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		viewInit(hwnd);
		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))

		{
		case ID_BUTTON_SELECT_FILE:
			getFile();
			break;
		case ID_BUTTON_INSTALL:
			install();
			break;
		case ID_BUTTON_RUN:
			run();
			break;
		case ID_BUTTON_STOP:
			stop();
			break;
		case ID_BUTTON_UNINSTALL:
			uninstall();
			break;
		case ID_BUTTON_SERVICE:
			g_type = 0;
			SendMessage(GetDlgItem(hwnd, ID_BUTTON_SERVICE), BM_SETCHECK, 1, 0);
			SendMessage(GetDlgItem(hwnd, ID_BUTTON_DRIVER), BM_SETCHECK, 0, 0);
			break;
		case ID_BUTTON_DRIVER:
			g_type = 1;
			SendMessage(GetDlgItem(hwnd, ID_BUTTON_DRIVER), BM_SETCHECK, 1, 0);
			SendMessage(GetDlgItem(hwnd, ID_BUTTON_SERVICE), BM_SETCHECK, 0, 0);
			break;
		default:
			break;
		}
		break;
	}
	case WM_DROPFILES:
	{
		HDROP hDrop = (HDROP)wParam;
		char filename[MAX_PATH];
		UINT count = DragQueryFileA(hDrop, -1, NULL, 0);
		if (count)
		{
			DragQueryFileA(hDrop, 0, filename, MAX_PATH);
			SetWindowTextA(g_hEdit_text, filename);
			SetWindowTextA(g_hEdit_info, "选择文件成功");
		}
		DragFinish(hDrop);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_QUIT:
	{
		int a = 1;
		break;
	}
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	}
	return 0;
}

void viewInit(HWND hwnd)
{
	WIN32_FIND_DATAA pNextInfo = { 0 };
	int left, top;
	CHAR path[MAX_PATH];
	HANDLE hFile;
	CHAR dest_file[MAX_PATH];

	left = MARGIN;
	top = MARGIN;

	g_hEdit_text = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("edit"), TEXT(""), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		left, top, MARGIN * 2 + BUTTON_WIDTH * 3, EDIT_HEIGHT, hwnd, (HMENU)ID_EDIT_TEXT, g_hInstance, NULL);
	left = MARGIN * 4 + BUTTON_WIDTH * 3;
	g_hButton_select_file = CreateWindow(TEXT("button"), TEXT("select_file"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		left, top, BUTTON_WIDTH, EDIT_HEIGHT, hwnd, (HMENU)ID_BUTTON_SELECT_FILE, g_hInstance, NULL);

	left = MARGIN;
	top = MARGIN * 2 + EDIT_HEIGHT;
	g_hButton_install = CreateWindow(TEXT("button"), TEXT("install"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		left, top, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, (HMENU)ID_BUTTON_INSTALL, g_hInstance, NULL);
	left += (MARGIN + BUTTON_WIDTH * 1);
	g_hButton_run = CreateWindow(TEXT("button"), TEXT("run"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		left, top, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, (HMENU)ID_BUTTON_RUN, g_hInstance, NULL);
	left += (MARGIN + BUTTON_WIDTH * 1);
	g_hButton_stop = CreateWindow(TEXT("button"), TEXT("stop"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		left, top, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, (HMENU)ID_BUTTON_STOP, g_hInstance, NULL);
	left += (MARGIN + BUTTON_WIDTH * 1);
	g_hButton_uninstall = CreateWindow(TEXT("button"), TEXT("uninstall"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		left, top, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, (HMENU)ID_BUTTON_UNINSTALL, g_hInstance, NULL);

	top = MARGIN * 3 + EDIT_HEIGHT + BUTTON_HEIGHT;
	left = MARGIN;
	g_hEdit_info = CreateWindowEx(0, TEXT("edit"), TEXT(""), WS_CHILD | WS_VISIBLE,
		left, top, MARGIN * 3 + BUTTON_WIDTH * 4, EDIT_HEIGHT, hwnd, (HMENU)ID_EDIT_INFO, g_hInstance, NULL);

	top = MARGIN * 3 + EDIT_HEIGHT * 2 + BUTTON_HEIGHT;
	left = MARGIN;
	g_hButton_service = CreateWindow(TEXT("button"), TEXT("service"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP | WS_TABSTOP | BS_RADIOBUTTON,
		left, top, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, (HMENU)ID_BUTTON_SERVICE, g_hInstance, NULL);
	left = MARGIN * 2 + BUTTON_WIDTH;
	g_hButton_driver = CreateWindow(TEXT("button"), TEXT("drvier"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP | WS_TABSTOP | BS_RADIOBUTTON,
		left, top, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, (HMENU)ID_BUTTON_DRIVER, g_hInstance, NULL);


	// 默认是驱动类型
	g_type = 1;
	SendMessage(GetDlgItem(hwnd, ID_BUTTON_DRIVER), BM_SETCHECK, 1, 0);
	SendMessage(GetDlgItem(hwnd, ID_BUTTON_SERVICE), BM_SETCHECK, 0, 0);


	// 尝试打开桌面下的dbgview
	SHGetSpecialFolderPathA(0, path, CSIDL_DESKTOP, 0);
	SetWindowTextA(g_hEdit_info, path);
	sprintf(dest_file, "%s/%s", path, "dbgview.exe");
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	CreateProcessA(
		dest_file,
		NULL,               // Command line
		NULL,               // Process handle not inheritable
		NULL,               // Thread handle not inheritable
		FALSE,              // Set handle inheritance to FALSE
		0,				// No creation flags, 子进程新建控制台.否则父子就用一个控制台
		NULL,               // Use parent's environment block
		NULL,               // Use parent's starting directory 
		&si,                // Pointer to STARTUPINFO structure
		&pi);                // Pointer to PROCESS_INFORMATION structure



	// 设置默认目标文件: ./debug/*.sys
	int result = GetCurrentDirectoryA(MAX_PATH, path);
	if (!result)
	{
		SetWindowTextA(g_hEdit_info, "获取工作路径失败!");
	}
	else
	{
		sprintf(dest_file, "%s/%s", path, MODEL_TEXT);
		hFile = FindFirstFileA(dest_file, &pNextInfo);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			sprintf(dest_file, "FindFirstFileA error : %d", GetLastError());
			SetWindowTextA(g_hEdit_info, dest_file);
		}
		else
		{
			sprintf(dest_file, "%s/%s", path, pNextInfo.cFileName);
			SetWindowTextA(g_hEdit_text, dest_file);
			SetWindowTextA(g_hEdit_info, "查找文件成功");
		}
	}
}


void getFile()
{
	OPENFILENAMEA ofn;       // 文件打开对话框的结构体
	ZeroMemory(&ofn, sizeof(ofn));

	char szFile[260] = { 0 };       // 存储选中文件的路径

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "\0All Files (*.*)\0*.*\0Text Files (*.txt)\0*.txt\0";
	ofn.lpstrInitialDir = "d:\\";
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn))
	{
		SetWindowTextA(g_hEdit_text, szFile);
		SetWindowTextA(g_hEdit_info, "选择文件成功");
	}
}

void install()
{
	SC_HANDLE schScManager = 0;
	SC_HANDLE schService = 0;
	DWORD dwRet = 0;

	wchar_t path[MAX_PATH] = { 0 };
	wchar_t name[MAX_PATH] = { 0 };
	GetWindowTextW(g_hEdit_text, path, MAX_PATH);
	GetWindowTextW(g_hEdit_text, name, MAX_PATH);
	if (!lstrlenW(path))
	{
		SetWindowTextA(g_hEdit_info, "路径为空");
		return;
	}

	RemoveExtensionW(name);
	wchar_t* p = PathFindFileNameW(name);
	if (!lstrlenW(p))
	{
		SetWindowTextA(g_hEdit_info, "服务为空");
		return;
	}

	// 1. 打开服务管理器
	schScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schScManager)
	{
		sprintf(text, "OpenSCManager()失败! ErrorCode :%d", GetLastError());
		SetWindowTextA(g_hEdit_info, text);
		return;
	}

	int m_type;
	if (g_type)
		m_type = SERVICE_KERNEL_DRIVER;
	else
		m_type = SERVICE_WIN32_OWN_PROCESS;

	// 2.安装建服务
	schService = CreateServiceW(
		schScManager,
		p,                                  // 出现在服务面板的名称
		L"",								// 服务面板 描述文本
		SERVICE_ALL_ACCESS,                 // 权限
		m_type,								// win32类型的服务
		SERVICE_DEMAND_START,               // 启动时机
		SERVICE_ERROR_NORMAL,               // 忽略错误
		path,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	);
	//判断服务安装是否失败  
	if (schService == NULL)
	{
		dwRet = GetLastError();
		if (dwRet != ERROR_IO_PENDING && dwRet != ERROR_SERVICE_EXISTS)
		{
			sprintf(text, "CreateService() 失败 ErrorCode :%d", GetLastError());
			SetWindowTextA(g_hEdit_info, text);
		}
		else
		{
			//服务创建失败，是由于服务已经创立过  
			sprintf(text, "CrateService() 失败  Service is ERROR_IO_PENDING or ERROR_SERVICE_EXISTS!\n");
			SetWindowTextA(g_hEdit_info, text);
		}
	}
	else
	{
		SetWindowTextA(g_hEdit_info, "安装 服务/驱动 成功!");
		CloseServiceHandle(schService);
	}

	CloseServiceHandle(schScManager);
}

void run()
{
	SC_HANDLE schScManager = 0;
	SC_HANDLE schService = 0;
	DWORD dwRet = 0;

	wchar_t path[MAX_PATH] = { 0 };
	wchar_t name[MAX_PATH] = { 0 };
	GetWindowTextW(g_hEdit_text, path, MAX_PATH);
	GetWindowTextW(g_hEdit_text, name, MAX_PATH);

	if (!lstrlenW(path))
	{
		SetWindowTextA(g_hEdit_info, "路径为空");
		return;
	}

	RemoveExtensionW(name);
	wchar_t* p = PathFindFileNameW(name);
	if (!lstrlenW(p))
	{
		SetWindowTextA(g_hEdit_info, "服务名为空");
		return;
	}

	// 1. 打开服务管理器
	schScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schScManager)
	{
		sprintf(text, "OpenSCManager()失败! ErrorCode :%d", GetLastError());
		SetWindowTextA(g_hEdit_info, text);
		return;
	}

	// 2. 打开服务
	schService = OpenServiceW(schScManager, p, SERVICE_ALL_ACCESS);
	if (!schService)
	{
		sprintf(text, "OpenService() 失败 ! ErrorCode :%d", GetLastError());
		SetWindowTextA(g_hEdit_info, text);
		CloseServiceHandle(schScManager);
		return;
	}

	// 3. 开启此项服务  
	BOOL bRet = StartService(schService, 0, NULL);
	if (!bRet)
	{
		DWORD dwRet = GetLastError();
		if (dwRet != ERROR_IO_PENDING && dwRet != ERROR_SERVICE_ALREADY_RUNNING)
		{
			sprintf(text, "StartService() 失败1 ! ErrorCode :%d", dwRet);
			SetWindowTextA(g_hEdit_info, text);
		}
		else
		{
			if (dwRet == ERROR_IO_PENDING)
			{
				//设备被挂住  
				sprintf(text, "StartService() 失败2 ERROR_IO_PENDING ! /n!");
				SetWindowTextA(g_hEdit_info, text);
			}
			else
			{
				sprintf(text, "StartService() 失败3 ERROR_SERVICE_ALREADY_RUNNING ");
				SetWindowTextA(g_hEdit_info, text);
			}
		}
	}
	else
	{
		sprintf(text, "服务启动成功!");
		SetWindowTextA(g_hEdit_info, text);
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schScManager);
}

void stop()
{
	SC_HANDLE schScManager = 0;
	SC_HANDLE schService = 0;
	DWORD dwRet = 0;

	wchar_t path[MAX_PATH] = { 0 };
	wchar_t name[MAX_PATH] = { 0 };
	GetWindowTextW(g_hEdit_text, path, MAX_PATH);
	GetWindowTextW(g_hEdit_text, name, MAX_PATH);

	if (!lstrlenW(path))
	{
		SetWindowTextA(g_hEdit_info, "路径为空");
		return;
	}

	RemoveExtensionW(name);
	wchar_t* p = PathFindFileNameW(name);
	if (!lstrlenW(p))
	{
		SetWindowTextA(g_hEdit_info, "服务名为空");
		return;
	}

	// 1. 打开服务管理器
	schScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schScManager)
	{
		sprintf(text, "OpenSCManager()失败! ErrorCode :%d", GetLastError());
		SetWindowTextA(g_hEdit_info, text);
		return;
	}

	// 2. 打开服务
	schService = OpenServiceW(schScManager, p, SERVICE_ALL_ACCESS);
	if (!schService)
	{
		sprintf(text, "OpenService() 失败 ! ErrorCode :%d", GetLastError());
		SetWindowTextA(g_hEdit_info, text);
		CloseServiceHandle(schScManager);
		return;
	}

	// 3. 查询服务状态
	SERVICE_STATUS status;
	BOOL isSuccess;
	isSuccess = QueryServiceStatus(schService, &status);
	if (!isSuccess)
	{
		sprintf(text, "QueryServiceStatus() 失败 ! ErrorCode :%d", GetLastError());
		SetWindowTextA(g_hEdit_info, text);
	}
	else
	{
		if (status.dwCurrentState != SERVICE_STOPPED)
		{
			// 4. 停止驱动程序，如果停止失败，只有重新启动才能，再动态加载。    
			if (!ControlService(schService, SERVICE_CONTROL_STOP, &status))
			{
				sprintf(text, "ControlService() 失败 ! ErrorCode :%d", GetLastError());
				SetWindowTextA(g_hEdit_info, text);
			}
			else
			{
				sprintf(text, "服务停止成功");
				SetWindowTextA(g_hEdit_info, text);
			}
		}
		else
		{
			sprintf(text, "服务未运行");
			SetWindowTextA(g_hEdit_info, text);
		}
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schScManager);
}

void uninstall()
{
	SC_HANDLE schScManager = 0;
	SC_HANDLE schService = 0;
	DWORD dwRet = 0;

	wchar_t path[MAX_PATH] = { 0 };
	wchar_t name[MAX_PATH] = { 0 };
	GetWindowTextW(g_hEdit_text, path, MAX_PATH);
	GetWindowTextW(g_hEdit_text, name, MAX_PATH);

	if (!lstrlenW(path))
	{
		SetWindowTextA(g_hEdit_info, "路径为空");
		return;
	}

	RemoveExtensionW(name);
	wchar_t* p = PathFindFileNameW(name);
	if (!lstrlenW(p))
	{
		SetWindowTextA(g_hEdit_info, "服务名为空");
		return;
	}

	// 1. 打开服务管理器
	schScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schScManager)
	{
		sprintf(text, "OpenSCManager()失败! ErrorCode :%d", GetLastError());
		SetWindowTextA(g_hEdit_info, text);
		return;
	}

	// 2. 打开服务
	schService = OpenServiceW(schScManager, p, SERVICE_ALL_ACCESS);
	if (!schService)
	{
		sprintf(text, "OpenService() 失败 ! ErrorCode :%d", GetLastError());
		SetWindowTextA(g_hEdit_info, text);
		CloseServiceHandle(schScManager);
		return;
	}

	// 3. 如果服务在运行先停止它
	SERVICE_STATUS status;
	BOOL isSuccess;
	isSuccess = QueryServiceStatus(schService, &status);
	if (!isSuccess)
	{
		sprintf(text, "QueryServiceStatus() 失败! ErrorCode :%d", GetLastError());
		SetWindowTextA(g_hEdit_info, text);

		CloseServiceHandle(schService);
		CloseServiceHandle(schScManager);
		return;
	}
	else
	{
		if (status.dwCurrentState != SERVICE_STOPPED)
		{
			// 停止驱动程序，如果停止失败，  
			if (!ControlService(schService, SERVICE_CONTROL_STOP, &status))
			{
				sprintf(text, "ControlService() 失败! ErrorCode :%d", GetLastError());
				SetWindowTextA(g_hEdit_info, text);

				CloseServiceHandle(schService);
				CloseServiceHandle(schScManager);
				return;
			}
		}
	}

	// 5. 卸载服务
	if (!DeleteService(schService))
	{
		sprintf(text, "DeleteService() 失败! ErrorCode :%d", GetLastError());
		SetWindowTextA(g_hEdit_info, text);
	}
	else
	{
		sprintf(text, "服务卸载成功");
		SetWindowTextA(g_hEdit_info, text);
	}
	CloseServiceHandle(schService);
	CloseServiceHandle(schScManager);
}

void RemoveExtension(char* path)
{
	size_t i, len;
	len = strlen(path);
	for (i = 0; i < len; i++) {
		if (path[i] == '.') {
			// 将点号改为字符串结束标记
			path[i] = '\0';
			break;
		}
	}
}

void RemoveExtensionW(wchar_t* path)
{
	size_t i, len;
	len = lstrlenW(path);
	for (i = 0; i < len; i++)
	{
		if (path[i] == L'.')
		{
			path[i] = L'\0';
			break;
		}
	}
}
