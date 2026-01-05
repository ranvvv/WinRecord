#include<windows.h>
#include<tchar.h>

int flag;

#define SERVICE_NAME TEXT("SplSrv")


SERVICE_STATUS SplSrvServiceStatus;					// 服务状态结构体
SERVICE_STATUS_HANDLE SplSrvServiceStatusHandle;	// 服务状态句柄

VOID SvcDebugOut(PTCHAR String, DWORD Status);
VOID WINAPI SplSrvServiceCtrlHandler(DWORD opcode);
VOID WINAPI SplSrvServiceStart(DWORD argc, LPTSTR* argv);
DWORD SplSrvServiceInitialization(DWORD argc, LPTSTR* argv, DWORD* specificError);


// 服务入口函数
VOID WINAPI SplSrvServiceStart(DWORD argc, LPTSTR* argv) // 参数是StartService传递的参数
{
	SvcDebugOut((PTCHAR)TEXT("SplSrvServiceStart run\n"), NOERROR);

	DWORD status;
	DWORD specificEror;

	SplSrvServiceStatus.dwServiceType = SERVICE_WIN32; 	// 服务类型
	SplSrvServiceStatus.dwCurrentState = SERVICE_START_PENDING; 	// 服务状态
	SplSrvServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
	SplSrvServiceStatus.dwWin32ExitCode = 0;
	SplSrvServiceStatus.dwServiceSpecificExitCode = 0;
	SplSrvServiceStatus.dwCheckPoint = 0;
	SplSrvServiceStatus.dwWaitHint = 0;

	// 注册服务控制处理程序,用于处理SCM对本服务的控制请求,如暂停、继续、停止等。
	SplSrvServiceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, SplSrvServiceCtrlHandler);
	if (SplSrvServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)
	{
		SvcDebugOut((PTCHAR)TEXT("RegisterServiceCtrlHandler failed %d\n"), GetLastError());
		return;
	}

	// 进行本服务的初始化工作,包括读取配置文件等.
	status = SplSrvServiceInitialization(argc, argv, &specificEror);
	if (status != NO_ERROR)
	{
		SplSrvServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SplSrvServiceStatus.dwCheckPoint = 0;
		SplSrvServiceStatus.dwWaitHint = 0;
		SplSrvServiceStatus.dwWin32ExitCode = status;
		SplSrvServiceStatus.dwServiceSpecificExitCode = specificEror;
		SetServiceStatus(SplSrvServiceStatusHandle, &SplSrvServiceStatus); // 设置服务状态.
		return;
	}

	// 初始化完成,设置服务状态为运行中
	SplSrvServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SplSrvServiceStatus.dwCheckPoint = 0;
	SplSrvServiceStatus.dwWaitHint = 0;
	if (!SetServiceStatus(SplSrvServiceStatusHandle, &SplSrvServiceStatus))	  // 设置服务状态.
	{
		SvcDebugOut((PTCHAR)TEXT("SetServiceStatus failed %d\n"), GetLastError());
	}

	// 服务功能

	flag = 1;

	while (1)
	{
		if (flag == 2)
			return;
		if (flag == 1)
			SvcDebugOut((PTCHAR)TEXT("running ----- !\n"), NOERROR);
		Sleep(1000);
	}

}

// 本服务的初始化工作.
DWORD SplSrvServiceInitialization(DWORD argc, LPTSTR* argv, DWORD* specificError)
{
	SvcDebugOut((PTCHAR)TEXT("SplSrvServiceInitialization run\n"), NOERROR);
	return NOERROR;
}

// 服务控制处理程序
VOID WINAPI SplSrvServiceCtrlHandler(DWORD opcode)
{
	SvcDebugOut((PTCHAR)TEXT("SplSrvServiceCtrlHandler run\n"), NOERROR);

	switch (opcode)
	{
	case SERVICE_CONTROL_PAUSE:
		flag = 0;
		SvcDebugOut((PTCHAR)TEXT("SERVICE_CONTROL_PAUSE run!\n"), NOERROR);
		SplSrvServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
	case SERVICE_CONTROL_CONTINUE:
		flag = 1;
		SvcDebugOut((PTCHAR)TEXT("SERVICE_CONTROL_CONTINUE run!\n"), NOERROR);
		SplSrvServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
	case SERVICE_CONTROL_STOP:
		flag = 2;
		SvcDebugOut((PTCHAR)TEXT("SERVICE_CONTROL_STOP run!\n"), NOERROR);
		SplSrvServiceStatus.dwWin32ExitCode = 0;
		SplSrvServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SplSrvServiceStatus.dwCheckPoint = 0;
		SplSrvServiceStatus.dwWaitHint = 0;
		break;
	case SERVICE_CONTROL_INTERROGATE:
		SvcDebugOut((PTCHAR)TEXT("SERVICE_CONTROL_INTERROGATE run!\n"), NOERROR);
		// MessageBeep(MB_OK);
		break;
	default:
		SvcDebugOut((PTCHAR)TEXT("SplSrvServiceCtrlHandler: unknown opcode %d"), opcode);
	}

	if (!SetServiceStatus(SplSrvServiceStatusHandle, &SplSrvServiceStatus)) // 设置服务状态.
	{
		SvcDebugOut((PTCHAR)TEXT("SetServiceStatus failed %d\n"), GetLastError());
	}
}

// 服务主函数
int ServiceMain()
{
	SvcDebugOut((PTCHAR)TEXT("service main run!\n"), NOERROR);

	// 注册服务控制处理程序
	SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{(PTCHAR)SERVICE_NAME, SplSrvServiceStart},
		{NULL, NULL}
	};
	if (!StartServiceCtrlDispatcher(serviceTable))		 // StartServiceCtrlDispatcher不会返回,直到服务停止
	{
		SvcDebugOut((PTCHAR)TEXT("StartServiceCtrlDispatcher failed %d\n"), GetLastError());
	}

	SvcDebugOut((PTCHAR)TEXT("service main end!\n"), NOERROR);

	return 0;
}

// 调试输出函数
VOID SvcDebugOut(PTCHAR String, DWORD Status)
{
	TCHAR buffer[1024];
	if (lstrlen(String) <= 1023)
	{
		_stprintf_s(buffer, 1024, String, Status);
		OutputDebugString(buffer);
	}
	else
		OutputDebugString(TEXT("Debug Oouput Error : mem !"));
}
