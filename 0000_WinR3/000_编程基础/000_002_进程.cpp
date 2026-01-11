#include"../common.h"

//				1. 进程的创建,打开,关闭,读写内存
//				2. 进程信息
//				3. 进程遍历
//				4. 根据名字找目标进程
//				5. 是否Wow64以及system dll path
//				6. 挂起形式创建进程


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				1. 进程的创建,打开,关闭,读写内存

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s1()
{
	BOOL result;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(TEXT("E:\\a.exe"), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return;
	}
	/*
	BOOL CreateProcess
	(
	1   LPCTSTR lpApplicationName,						// 进程完整路径,如果为NULL则param2的第一个位置必须为路径
	2   LPTSTR lpCommandLine,							// 命令行参数
															游戏的登录器通过CreateProcess运行游戏exe. 通过命令行参数传递数据到游戏进程.
															所以自己手动运行游戏exe可能无法启动.因为进程参数不对.分析出参数来源就可以自己CreateProcess了
	3   LPSECURITY_ATTRIBUTES lpProcessAttributes, 		// 新进程也是个内核对象,这里设置它的属性: 是否可被子进程继承
	4   LPSECURITY_ATTRIBUTES lpThreadAttributes, 		// 新进程的主线程也是内核对象,这里设置它的属性: 是否可被子进程继承
	5   BOOL bInheritHandles, 							//  新进程是否继承父进程句柄表中可继承的句柄
	6   DWORD dwCreationFlags,							//  创建标志 (1) 是否创建新控制台CREATE_NEW_CONSOLE  (2) 是否以挂起的形式创建进程CREATE_SUSPENDED
	7   LPVOID lpEnvironment, 							//  环境变量     NULL 使用父进程环境变量或自定义
	8   LPCTSTR lpCurrentDirectory,      				//  当前目录     NULL 使用父进程目录作为或自定义
	9   LPSTARTUPINFO lpStartupInfo,					//  无用结构体,但还必须填
	10  LPPROCESS_INFORMATION lpProcessInformation 		//  创建成功后的out结构体保存创建的进程和线程的id 和句柄
	);

	// 桌面双击的原理:		桌面图标是shell进程绘制的.双击启动程序最终还是调用CreateProcess()创建制定路径程序文件对应进程
	// 创建进程传递参数 :	给它下断点找出参数, 然后自己CreateProcess就可以跳过游戏更新程序.直接打开游戏进程.
	*/

	// 打开进程
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pi.dwProcessId);
	if (!hProcess)
	{
		printf("OpenProcess failed (%d).\n", GetLastError());
		return;
	}

	// 结束目标进程
	result = TerminateProcess(hProcess, 3);
	if (!result)
	{
		printf("TerminateProcess failed (%d).\n", GetLastError());
		return;
	}

	// 结束当前进程
	ExitProcess(0);

	// 等待子进程结束
	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);


	// 读写进程内存
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)0x401000, (LPVOID)"abc", 3, NULL);
	ReadProcessMemory(GetCurrentProcess(), (LPVOID)0x401000, (LPVOID)"abc", 3, NULL);

	// 进程的终止方式:
	// 1. 主线程main 返回, 进程就会终止.其他线程也会被干掉
	// 2. 进程自己调用		ExitProcess(0);
	// 3. 终止其他进程		TerminateProcess(hProcess,0); 
	// 4. 如果终止进程的所有线程,进程自然会被系统干掉	ExitThread()


	DWORD code;
	GetExitCodeProcess(hProcess,&code); // 获取进程退出码

}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				2. 进程信息

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s2()
{
	// 命令行参数
	PTCHAR pCmdLien = GetCommandLine();

	// 启动信息
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	GetStartupInfo(&si);
	_tprintf(TEXT("%s\n"), si.lpDesktop);
	_tprintf(TEXT("%s\n"), si.lpTitle);
	printf("%d\n", si.dwX);
	printf("%d\n", si.dwY);
	printf("%d\n", si.dwXSize);
	printf("%d\n", si.dwYSize);
	printf("%p\n", si.hStdInput);
	printf("%p\n", si.hStdOutput);
	printf("%p\n", si.hStdError);

	// 反调试实例 :			通过创建进程的命令行参数来查看是否被调试
	// STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	GetStartupInfo(&si);
	printf("%x %x %x %x %x %x %x %x\n", si.dwX, si.dwY, si.dwXCountChars, si.dwYCountChars, si.dwFillAttribute, si.dwXSize, si.dwYSize, si.dwFlags);
	// 当有调试器时: 0 0 77d20034 0 1000000 77d20034 0  , 无调试器时全是0

	// 标准输入输出
	printf("%p %p %p \n", stdin, stdout, stderr);

	// 进程ID和句柄
	HANDLE hProcess = GetCurrentProcess();
	DWORD pid = GetCurrentProcessId();

	// 遍历环境变量
	PTCHAR pEnv = GetEnvironmentStrings();
	for (PTCHAR p = pEnv; *p; )
	{
		_tprintf(TEXT("%s\n"), p);
		while (*p++);
	}
	FreeEnvironmentStrings(pEnv);

	// 获取环境变量
	TCHAR buffer[500];
	DWORD size = sizeof(buffer) / sizeof(buffer[0]);
	DWORD result = GetEnvironmentVariable(TEXT("PATH"), buffer, size);
	if (result == 0) {
		printf("Error: %d\n", GetLastError());
	}
	else {
		buffer[result] = 0;  // Null-terminate the string
		_tprintf(TEXT("PATH = %s\n"), buffer);
	}

	// SetEnvironmentVariable();

	// 模块基址
	HMODULE hModule = GetModuleHandle(NULL);

	// 模块路径
	char text[1024];
	GetModuleFileNameA(NULL, text, 1024);
	printf("module file name %s\n", text);

	// 工作路径
	GetCurrentDirectoryA(1024, text);
	printf("currentDir %s\n", text);
	SetCurrentDirectoryA("d://");
	GetCurrentDirectoryA(1024, text);
	printf("currentDir %s\n", text);

	// GetProcessMemoryInfo(hProcess, &si.WorkingSetSize, sizeof(si.WorkingSetSize));
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				3. 进程遍历

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 遍历进程EnumProcess
static void mEnumProcess()
{
	// ENUM 遍历,但是数组大小是个问题,空间不够是没有错误提示的.
	DWORD result;
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	result = EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded);
	if (!result)
	{
		printf("EnumProcess Error :%d \n", GetLastError());
		return;
	}
	cProcesses = cbNeeded / sizeof(DWORD);
	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
			printf("%d - PID : %d\n", i, aProcesses[i]);
	}
}

// 快照遍历
static void mSnapshotProcess()
{
	PROCESSENTRY32 pe32;
	HANDLE hProcessSnap;
	BOOL b;
	HANDLE hProcess;
	BOOL isWow64Process;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf(" CreateToolhelp32Snapshot error %d\n", GetLastError());
		return;
	}

	b = Process32First(hProcessSnap, &pe32);
	while (b)
	{
		printf("name:%ws  ", pe32.szExeFile);
		printf("process id:%d  ", pe32.th32ProcessID);
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		if (!hProcess)
		{
			printf("OpenProcess failed (%d).\n", GetLastError());
		}
		else
		{
			isWow64Process = FALSE;
			if (IsWow64Process(hProcess, &isWow64Process))	// 判断是否是64位系统下的32位程		系统目录  c:/windows/System32   || c:/windows/SysWOW64
			{
				if (isWow64Process)
					printf("wow64\n");
				else
					printf("x64\n");
			}

		}
		b = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
}


static void s3()
{
	mEnumProcess();
	mSnapshotProcess();
	return;
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				4. 根据名字找目标进程

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static DWORD mFindFirstProcessIDByName(const WCHAR* processName)
{
	PROCESSENTRY32 pe32;
	HANDLE hProcessSnap;
	BOOL b;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf(" CreateToolhelp32Snapshot error %d\n", GetLastError());
		return 0;
	}

	b = Process32First(hProcessSnap, &pe32);
	while (b)
	{
		printf("name:%ws  ", pe32.szExeFile);
		printf("process id:%d  \n", pe32.th32ProcessID);
		if (wcscmp(processName, pe32.szExeFile) == 0)
		{
			wprintf(L"find : %s   PID : %d \n", processName, pe32.th32ProcessID);
			return pe32.th32ProcessID;
		}

		b = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
	return 0;
}

static void s4()
{
	DWORD id = mFindFirstProcessIDByName(L"notepad.exe");
	printf("id:%d\n", id);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				5. 是否Wow64以及system dll path

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s5()
{
	TCHAR tzPath[MAX_PATH];
	// 1. 拼接原dll的路径
	BOOL isWow64Process = FALSE;
	if (IsWow64Process((HANDLE)-1, &isWow64Process))	// 判断是否是64位系统下的32位程		系统目录  c:/windows/System32   || c:/windows/SysWOW64
	{
		if (isWow64Process)
			GetSystemWow64Directory(tzPath, MAX_PATH);	// 64位下的32位程序
		else
			GetSystemDirectory(tzPath, MAX_PATH);		// 64位主机下的64位程序 或 32位主机下的32位程序
	}
	else
	{
		printf("IsWow64Process Error :%d \n", GetLastError());
		return;
	}
	lstrcat(tzPath, TEXT("\\winspool.drv"));			// 拼接原始dll路径
	_tprintf(TEXT("%s\n"), tzPath);
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				6. 挂起形式创建进程

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


static void s6()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(
		TEXT("E:\\WinREx86.exe"),  // module name ( if module name is NULL , use command line)
		NULL,               // Command line
		NULL,               // Process handle not inheritable
		NULL,               // Thread handle not inheritable
		FALSE,              // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE | CREATE_SUSPENDED, //  子进程新建控制台.否则父子就用一个控制台,挂起形式创建
		NULL,               // Use parent's environment block
		NULL,               // Use parent's starting directory 
		&si,                // Pointer to STARTUPINFO structure
		&pi)                // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return;
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	// 打开进程
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pi.dwProcessId);
	if (!hProcess)
	{
		printf("OpenProcess failed (%d).\n", GetLastError());
		return;
	}

	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, false, pi.dwThreadId);
	if (!hThread)
	{
		printf("OpenThread failed (%d).\n", GetLastError());
		return;
	}

	//定义线程信息结构体
	CONTEXT context = { 0 };
	// 设置获取类型:根据分组宏获取一组寄存器,根据这个复制来获取不同组的数据	
	context.ContextFlags = CONTEXT_FULL;	// 获取所有值.
	// 获取  线程上下文环境
	BOOL ok = GetThreadContext(hThread, &context);
	if (!ok)
		printf("error code : %d", GetLastError());

#if _WIN64
	printf("OEP : %I64x\n", context.Rcx);
	printf("PEB: %I64x\n", context.Rdx);

	UINT64 image_base = 0;
	ReadProcessMemory(hProcess, LPCVOID(context.Rdx + 0x10), &image_base, 8, NULL);
	printf("image_base : %016I64X\n", image_base);

	// 设置
	// context.Eip = ;
	// 修改线程上下文环境.可以修改线程的执行流程.     挂起来,改变,再恢复. 线程级别的hook
	// SetThreadContext(hThread, &context);

#else
	// 打印输出Eip
	printf("OEP : %08X\n", context.Eax);
	printf("PEB : %08X\n", context.Ebx);

	DWORD image_base = 0;
	ReadProcessMemory(hProcess, LPCVOID(context.Ebx + 8), &image_base, 4, NULL);
	printf("image_base : %08X\n", image_base);

	// 设置
	// context.Eip = ;
	// 修改线程上下文环境.可以修改线程的执行流程.     挂起来,改变,再恢复. 线程级别的hook
	// SetThreadContext(hThread, &context);
#endif

	ResumeThread(hThread);

	// 等待子进程结束
	WaitForSingleObject(hProcess, INFINITE);

	printf("over\n");

	CloseHandle(hProcess);
	CloseHandle(hThread);

}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_002()
{
	s1();

}
