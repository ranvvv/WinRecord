#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				 1. TSL回调函数测试

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


#if 0

int MessageBoxPrintf(const TCHAR* const szCaption, const TCHAR* const szFormat, ...);

#ifdef _WIN64
#pragma comment(linker,"/INCLUDE:_tls_used")
#else
#pragma comment(linker,"/INCLUDE:__tls_used")
#endif // _WIN64

//TLS回调函数测试
void NTAPI MY_TLS_CALLBACK1(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
    if (Reason == DLL_PROCESS_ATTACH)
    {
        MessageBoxPrintf(L"TLSTest1!", L"%p", MY_TLS_CALLBACK1);
    }
}
void NTAPI MY_TLS_CALLBACK2(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
    if (Reason == DLL_PROCESS_ATTACH)
    {
        MessageBox(NULL, L"TLSTest2!", NULL, MB_OK);
    }
}

extern "C"
#ifdef _WIN64
#pragma const_seg(".CRT$XLX")
const
#else
#pragma data_seg(".CRT$XLX")
#endif
PIMAGE_TLS_CALLBACK pTLS_CALLBACKs[] = { MY_TLS_CALLBACK1,MY_TLS_CALLBACK2,0 };
#pragma data_seg()
#pragma const_seg()

#endif


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				 2. 动态TLS

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s2()
{
    DWORD i = TlsAlloc();
    TlsSetValue(i, (LPVOID)1);
    LPVOID data = TlsGetValue(i);
    TlsFree(i);
}
