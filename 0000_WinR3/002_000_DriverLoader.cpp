#include<windows.h>
#include<winsvc.h>
#include<strsafe.h>
#include<winternl.h>
#include<shlwapi.h>
#include<winsvc.h>


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				服务管理器方式    安装驱动服务

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 验证路径
INT32 mVerifyPath(CONST PWCHAR path, PWCHAR nameBuffer, UINT32 nameBufferSize)
{
	PWCHAR pName = NULL;

	if (!path || !nameBuffer || !nameBufferSize || !wcslen(path))
		return -1;

	if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES)
		return -2;	// 文件不存在

	pName = PathFindFileNameW(path);
	if (!pName || !wcslen(pName))
		return -3;

	//wcscpy_s(nameBuffer, nameBufferSize,pName);

	HRESULT hr = StringCchCopyW(nameBuffer, nameBufferSize, pName);
	if (FAILED(hr))
		return -4;  // 缓冲区不足

	PathRemoveExtensionW(nameBuffer);

	return 0;
}

// 安装驱动服务
INT32 mInstallDriver(CONST PWCHAR path, UCHAR mode)
{
	SC_HANDLE schScManager = 0;
	SC_HANDLE schService = 0;
	WCHAR name[MAX_PATH] = { 0 };
	INT32 ret = 0;

	ret = mVerifyPath(path, name, MAX_PATH);
	if (ret < 0)
		return -1;

	// 1. 打开服务管理器
	schScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schScManager)
		return -2;

	// 2.安装建服务
	schService = CreateServiceW(
		schScManager,
		name,                               // 出现在服务面板的名称
		L"",								// 服务面板 描述文本
		SERVICE_ALL_ACCESS,                 // 权限
		mode ? SERVICE_WIN32_OWN_PROCESS : SERVICE_KERNEL_DRIVER,	// 服务类型，驱动与服务不一样
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
		if (GetLastError() != ERROR_SERVICE_EXISTS)
			ret = -3; // 安装失败
	}

	if (schService)
		CloseServiceHandle(schService);
	if (schScManager)
		CloseServiceHandle(schScManager);
	return ret;
}

// 开启驱动服务
INT32 mStartDriver(CONST PWCHAR path)
{
	SC_HANDLE schScManager = 0;
	SC_HANDLE schService = 0;
	DWORD ErrCode = 0;
	WCHAR name[MAX_PATH] = { 0 };
	INT32 ret = 0;

	ret = mVerifyPath(path, name, MAX_PATH);
	if (ret < 0)
		return -1;

	// 1. 打开服务管理器
	schScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schScManager)
		return -2;

	// 2. 打开服务
	schService = OpenServiceW(schScManager, name, SERVICE_ALL_ACCESS);
	if (!schService)
	{
		ret = -3;
		goto end;
	}

	// 3. 开启此项服务  
	if (!StartService(schService, 0, NULL))
		ret = -4;

end:
	if (schService)
		CloseServiceHandle(schService);
	if (schScManager)
		CloseServiceHandle(schScManager);

	return ret;
}

// 停止驱动服务
INT32 mStopDriver(CONST PWCHAR path)
{
	SC_HANDLE schScManager = 0;
	SC_HANDLE schService = 0;
	DWORD ErrCode = 0;
	WCHAR name[MAX_PATH] = { 0 };
	INT32 ret = 0;
	SERVICE_STATUS status = { 0 };

	ret = mVerifyPath(path, name, MAX_PATH);
	if (ret < 0)
		return -1;

	// 1. 打开服务管理器
	schScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schScManager)
		return -2;

	// 2. 打开服务
	schService = OpenServiceW(schScManager, name, SERVICE_ALL_ACCESS);
	if (!schService)
	{
		ret = -3;
		goto end;
	}

	// 3. 查询服务状态
	if (!QueryServiceStatus(schService, &status))
	{
		ret = -4;
		goto end;
	}

	// 已经停止 或者 正在停止，无需操作
	if (status.dwCurrentState == SERVICE_STOPPED || status.dwCurrentState == SERVICE_STOP_PENDING)
		goto end;


	// 4. 停止服务
	if (!ControlService(schService, SERVICE_CONTROL_STOP, &status))
		ret = -5;

end:
	if (schService)
		CloseServiceHandle(schService);
	if (schScManager)
		CloseServiceHandle(schScManager);

	return ret;
}

// 卸载驱动服务
INT32 mUninstallDriver(CONST PWCHAR path)
{
	SC_HANDLE schScManager = 0;
	SC_HANDLE schService = 0;
	DWORD ErrCode = 0;
	WCHAR name[MAX_PATH] = { 0 };
	INT32 ret = 0;
	SERVICE_STATUS status = { 0 };

	ret = mVerifyPath(path, name, MAX_PATH);
	if (ret < 0)
		return -1;

	// 1. 打开服务管理器
	schScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schScManager)
		return -2;

	// 2. 打开服务
	schService = OpenServiceW(schScManager, name, SERVICE_ALL_ACCESS);
	if (!schService)
	{
		ret = -3;
		goto end;
	}

	// 3. 如果服务在运行先停止它
	if (!QueryServiceStatus(schService, &status))
	{
		ret = -4;
		goto end;
	}

	// 4. 如果服务在运行先停止它
	if (status.dwCurrentState != SERVICE_STOPPED)
	{
		if (!ControlService(schService, SERVICE_CONTROL_STOP, &status))
		{
			ret = -5;
			goto end;
		}
	}

	// 5. 卸载服务
	if (!DeleteService(schService))
		ret = -6;

end:
	if (schService)
		CloseServiceHandle(schService);
	if (schScManager)
		CloseServiceHandle(schScManager);

	return ret;
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				NtLoadDriver  / NtUnloadDriver

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// 定义未公开的NTAPI函数
typedef NTSTATUS(NTAPI* _NtLoadDriver)(PUNICODE_STRING DriverServiceName);
typedef NTSTATUS(NTAPI* _NtUnloadDriver)(PUNICODE_STRING DriverServiceName);

// 驱动注册表路径（需全局唯一）
#define DRIVER_REG_KEY L"System\\CurrentControlSet\\Services\\" 

// 创建/删除驱动注册表项
INT32 mConfigDriverRegistry(PWCHAR driverPath, BOOL install, DWORD type = 1, DWORD start = 3)
{
	HKEY hKey;
	DWORD dwType = type;		// 1; SERVICE_KERNEL_DRIVER
	DWORD dwStart = start;		// 3; SERVICE_DEMAND_START
	WCHAR name[MAX_PATH] = { 0 };
	WCHAR regPath[MAX_PATH] = { 0 };
	WCHAR imagePath[MAX_PATH] = { 0 };
	INT32 ret = 0;
	DWORD val = 0;

	ret = mVerifyPath(driverPath, name, MAX_PATH);
	if (ret < 0)
		return -1;

	swprintf(regPath, MAX_PATH, L"%s%s", DRIVER_REG_KEY, name);

	if (install)
	{
		// 创建注册表项并设置键值
		if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, regPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL) != ERROR_SUCCESS)
			return FALSE;

		// 设置驱动.sys文件路径（格式必须为 \??\C:\path\to\driver.sys）
		swprintf(imagePath, MAX_PATH, L"\\??\\%s", driverPath);
		if (RegSetValueExW(hKey, L"ImagePath", 0, REG_EXPAND_SZ, (const BYTE*)imagePath, (DWORD)((wcslen(imagePath) + 1) * sizeof(WCHAR))) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return FALSE;
		}

		// 设置驱动类型和启动方式
		if (RegSetValueExW(hKey, L"Type", 0, REG_DWORD, (const BYTE*)&dwType, sizeof(DWORD)) != ERROR_SUCCESS ||
			RegSetValueExW(hKey, L"Start", 0, REG_DWORD, (const BYTE*)&dwStart, sizeof(DWORD)) != ERROR_SUCCESS ||
			RegSetValueExW(hKey, L"ErrorControl", 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD)) != ERROR_SUCCESS
			)
		{
			RegCloseKey(hKey);
			return FALSE;
		}

		RegCloseKey(hKey);
	}
	else {
		// 删除注册表项
		if (RegDeleteKeyW(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS)
			return FALSE;
	}
	return TRUE;
}

// 想要直接调用NtLoadDriver/NtUnloadDriver，需要启用SeLoadDriverPrivilege权限
BOOL EnableLoadDriverPrivilege() {
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;

	// 1. 获取当前进程的令牌
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		return FALSE;
	}

	// 2. 查找 SeLoadDriverPrivilege 的 LUID
	if (!LookupPrivilegeValue(NULL, SE_LOAD_DRIVER_NAME, &luid)) {
		CloseHandle(hToken);
		return FALSE;
	}

	// 3. 启用权限
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);
	return TRUE;
}

// 调用NtLoadDriver/NtUnloadDriver
NTSTATUS mCallNtLoadUnLoadDriverAPI(PWCHAR driverPath, BOOL load)
{
	WCHAR name[MAX_PATH] = { 0 };
	WCHAR regPath[MAX_PATH] = { 0 };
	WCHAR str[MAX_PATH] = { 0 };
	INT32 ret = 0;

	ret = mVerifyPath(driverPath, name, MAX_PATH);
	if (ret < 0)
		return -1;

	// 创建注册表项并设置键值
	HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
	if (!hNtdll)
		return STATUS_DLL_NOT_FOUND;

	UNICODE_STRING usRegPath;
	swprintf(regPath, MAX_PATH, L"%s%s%s", L"\\Registry\\Machine\\", DRIVER_REG_KEY, name);
	usRegPath.Length = (USHORT)(wcslen(regPath) * sizeof(WCHAR));
	usRegPath.MaximumLength = MAX_PATH * sizeof(WCHAR);
	usRegPath.Buffer = regPath;

	// 在调用 NtLoadDriver 前调用：
	EnableLoadDriverPrivilege();

	NTSTATUS status;
	if (load) {
		_NtLoadDriver pNtLoadDriver = (_NtLoadDriver)GetProcAddress(hNtdll, "NtLoadDriver");
		if (!pNtLoadDriver)
			return STATUS_ENTRYPOINT_NOT_FOUND;
		status = pNtLoadDriver(&usRegPath);
	}
	else {
		_NtUnloadDriver pNtUnloadDriver = (_NtUnloadDriver)GetProcAddress(hNtdll, "NtUnloadDriver");
		if (!pNtUnloadDriver)
			return STATUS_ENTRYPOINT_NOT_FOUND;
		status = pNtUnloadDriver(&usRegPath);
	}

	//swprintf(str, MAX_PATH, L"NtLoadDriver %08X", status);
	//MessageBox(NULL, str, L"NtLoadDriver", MB_OK);

	return status;
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				对外开放  

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


INT32 mTestRunDriver(CONST PWCHAR path)
{
	// ----  服务 方式 ----
	//if(mInstallDriver(path,0) < 0)
	//	return -1;
	//if(mStartDriver(path) < 0)
	//	return -2;

	// ----  NtLoadDriver 方式 ----
	mConfigDriverRegistry(path, 1);
	mCallNtLoadUnLoadDriverAPI(path, 1);

	return 0;
}


INT32 mTestStopDriver(CONST PWCHAR path)
{
	// ---- 服务 方式 ----
	//mUninstallDriver(path);

	// ----  NtLoadDriver 方式 ----
	mCallNtLoadUnLoadDriverAPI(path, 0);
	mConfigDriverRegistry(path, 0);

	return 0;
}

