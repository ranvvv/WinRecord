#include"common.h"
#include<Psapi.h>	
#include<TlHelp32.h>
#include<iostream>
#include<shlwapi.h>

//				1. 遍历进程模块
//				2. 引入模块
//				3. 根据名字得到模块基址


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				1. 遍历进程模块

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s1()
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;

	DWORD pid = GetCurrentProcessId();

	printf("\nProcess ID: %u\n", pid);

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (NULL == hProcess)
	{
		printf("OpenProcess Error : %d\n", GetLastError());
		return;
	}

	if (EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_ALL))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			char szModName[MAX_PATH];
			if (GetModuleFileNameExA(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(char)))
			{
				printf("\t%s (0x%08X)\n", szModName, (UINT32)(UINT64)(hMods[i]));
			}
		}
	}

	CloseHandle(hProcess);


	// 进程中拍一个所有模块的快照
	HANDLE hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
		return;

	// 遍历快照中记录的模块
	MODULEENTRY32 me32 = { 0 };
	me32.dwSize = sizeof(MODULEENTRY32);
	if (::Module32First(hModuleSnap, &me32))
	{
		do
		{
			std::cout << me32.szExePath << "\n";
			std::cout << " 模块在本进程中的地址：" << me32.hModule << "\n";
		} while (::Module32Next(hModuleSnap, &me32));
	}
	::CloseHandle(hModuleSnap);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				2. 引入模块

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// #pragma comment(lib,"./x64/Debug/TestDll.lib")
// __declspec(dllimport) void __stdcall  Test1();

typedef void (*FUNC)();

static void s2()
{
	//	Test1();

	HMODULE hMod = LoadLibraryA("TestDll.dll");
	if (NULL == hMod)
	{
		printf("LoadLibrary Error : %d\n", GetLastError());
		return;
	}

	PVOID t1 = GetProcAddress(hMod, "Test1");
	PVOID t2 = GetProcAddress(hMod, "Test2");
	PVOID t3 = GetProcAddress(hMod, "Test3");

	((FUNC)t2)();
	((FUNC)t3)();

	printf("%p %p %p\n", t1, t2, t3);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				3. 根据名字得到模块基址

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 获取模块的基址根据进程ID和模块名称
static HMODULE getMoudleImageBaseByName(DWORD pid, const TCHAR* name)
{
	// 在本进程中拍一个所有模块的快照
	HANDLE hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
		return NULL;

	// 遍历快照中记录的模块
	MODULEENTRY32 me32 = { 0 };
	me32.dwSize = sizeof(MODULEENTRY32);
	if (::Module32First(hModuleSnap, &me32))
	{
		do
		{
			PTCHAR pFileName = PathFindFileName(me32.szExePath);
			_tprintf(TEXT("fileName : %s\n"), pFileName);
			if (lstrcmpiW(pFileName, name) == 0)
			{
				CloseHandle(hModuleSnap);
				return me32.hModule;
			}
		} while (::Module32Next(hModuleSnap, &me32));
	}
	::CloseHandle(hModuleSnap);
	return NULL;
}

static void s3()
{
	HMODULE hModule = getMoudleImageBaseByName(0, TEXT("KERNEL32.DLL"));
	printf(" hModule : %p\n", hModule);
	hModule = getMoudleImageBaseByName(0, TEXT("kernel32.dll"));
	printf(" hModule : %p\n", hModule);
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				4. 模块隐藏

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
#ifndef _WIN64

#include <winternl.h>

// 定义未导出的结构体和函数
//typedef struct _UNICODE_STRING {
//	USHORT Length;
//	USHORT MaximumLength;
//	PWSTR  Buffer;
//} UNICODE_STRING, * PUNICODE_STRING;
//
typedef struct MD_PEB_LDR_DATA {
	ULONG Length;
	BOOLEAN Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
} MD_PEB_LDR_DATA, * PMD_PEB_LDR_DATA;

typedef struct _LDR_MODULE {
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID BaseAddress;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	SHORT LoadCount;
	SHORT TlsIndex;
	LIST_ENTRY HashTableEntry;
	ULONG TimeDateStamp;
} LDR_MODULE, * PLDR_MODULE;


// 获取PEB的函数声明
PPEB __cdecl NtCurrentPeb(VOID);

// 隐藏指定模块
BOOL HideModule(HMODULE hModule) {

	PPEB pPeb;

	__asm {
		mov eax, fs: [0x30]  // FS段寄存器指向TEB，偏移0x18处是PEB指针
		mov pPeb, eax
	}

	PMD_PEB_LDR_DATA pLdr = (PMD_PEB_LDR_DATA)pPeb->Ldr;
	PLIST_ENTRY pListEntry = pLdr->InLoadOrderModuleList.Flink;

	while (pListEntry != &pLdr->InLoadOrderModuleList) {
		PLDR_MODULE pLdrModule = CONTAINING_RECORD(pListEntry, LDR_MODULE, InLoadOrderModuleList);

		if (pLdrModule->BaseAddress == hModule) {
			// 从InLoadOrderModuleList中移除
			pLdrModule->InLoadOrderModuleList.Blink->Flink = pLdrModule->InLoadOrderModuleList.Flink;
			pLdrModule->InLoadOrderModuleList.Flink->Blink = pLdrModule->InLoadOrderModuleList.Blink;

			// 从InMemoryOrderModuleList中移除
			pLdrModule->InMemoryOrderModuleList.Blink->Flink = pLdrModule->InMemoryOrderModuleList.Flink;
			pLdrModule->InMemoryOrderModuleList.Flink->Blink = pLdrModule->InMemoryOrderModuleList.Blink;

			// 从InInitializationOrderModuleList中移除
			pLdrModule->InInitializationOrderModuleList.Blink->Flink = pLdrModule->InInitializationOrderModuleList.Flink;
			pLdrModule->InInitializationOrderModuleList.Flink->Blink = pLdrModule->InInitializationOrderModuleList.Blink;

			return TRUE;
		}

		pListEntry = pListEntry->Flink;
	}

	return FALSE;
}

void s4() {
	HMODULE hModule = GetModuleHandleA("kernel32.dll");
	if (hModule == NULL) {
		printf("Failed to get module handle\n");
		return;
	}

	printf("Before hiding:\n");
	//system("tasklist /m kernel32.dll");

	if (HideModule(hModule)) {
		printf("\nModule hidden successfully!\n");
		printf("After hiding:\n");
		system("tasklist /m kernel32.dll");
	}
	else {
		printf("Failed to hide module\n");
	}
}

#endif

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void p000_016()
{

}