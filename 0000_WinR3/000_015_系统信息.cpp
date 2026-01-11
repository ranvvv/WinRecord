#include"common.h"


//				 1. 宏定义记录
//				 2. 系统版本
//				 3. 系统信息
//				 4. 系统目录



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				 1. 宏定义记录

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


#ifdef _WIN64
#endif

#ifdef _UNICODE
#endif


// kernel32.dll
// user32.dll
// gdi32.dll
// crtdll.dll



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				 2. 系统版本

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#include <VersionHelpers.h>

static void s2()
{
    if (IsWindowsXPOrGreater())
    {
        printf("XPOrGreater\n");
    }

    if (IsWindowsXPSP1OrGreater())
    {
        printf("XPSP1OrGreater\n");
    }

    if (IsWindowsXPSP2OrGreater())
    {
        printf("XPSP2OrGreater\n");
    }

    if (IsWindowsXPSP3OrGreater())
    {
        printf("XPSP3OrGreater\n");
    }

    if (IsWindowsVistaOrGreater())
    {
        printf("VistaOrGreater\n");
    }

    if (IsWindowsVistaSP1OrGreater())
    {
        printf("VistaSP1OrGreater\n");
    }

    if (IsWindowsVistaSP2OrGreater())
    {
        printf("VistaSP2OrGreater\n");
    }

    if (IsWindows7OrGreater())
    {
        printf("Windows7OrGreater\n");
    }

    if (IsWindows7SP1OrGreater())
    {
        printf("Windows7SP1OrGreater\n");
    }

    if (IsWindows8OrGreater())
    {
        printf("Windows8OrGreater\n");
    }

    if (IsWindows8Point1OrGreater())
    {
        printf("Windows8Point1OrGreater\n");
    }

    if (IsWindows10OrGreater())          // 程序必要要限定 win10平台才会返回true, win10下 也可能看不到这个输出
    {
        printf("Windows10OrGreater\n");
    }

    if (IsWindowsServer())
    {
        printf("Server\n");
    }
    else
    {
        printf("Client\n");
    }
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				 3. 系统信息

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

BOOL Is64BitSystem()
{
    SYSTEM_INFO si = { 0 };
    GetNativeSystemInfo(&si); // 注意：用GetNativeSystemInfo而非GetSystemInfo，获取真实系统位数
    return (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);
}

static void s3()
{
    int a = GetSystemMetrics(SM_CXFULLSCREEN);		// 主显示器宽度
    printf("screen width : %d \n", a);


    SYSTEMTIME st;
    GetLocalTime(&st);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				 4. 系统目录

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s4()
{
    TCHAR tzPath[MAX_PATH];

    // 1. 拼接原dll的路径
    BOOL isWow64Process = FALSE;
    if (IsWow64Process((HANDLE)-1, &isWow64Process))	// 判断是否是64位系统下的32位程		
    {
        if (isWow64Process)
            GetSystemWow64Directory(tzPath, MAX_PATH);	// 64位下的32位程序     c:/windows/SysWOW64
        else
            GetSystemDirectory(tzPath, MAX_PATH);		// 64位主机下的64位程序 或 32位主机下的32位程序   c:/windows/System32 
    }
    else
    {
        MessageBox(NULL, TEXT("系统路径获取失败"), TEXT("Error"), MB_ICONSTOP);
        return;
    }
    lstrcat(tzPath, TEXT("\\winspool.drv"));			// 拼接原始dll路径
    _tprintf(TEXT("原dll路径 : %s \n"), tzPath);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$



void p000_015()
{
    s4();

}
