#include<windows.h>
#include<stdio.h>
#include<tchar.h>
#include "../PE.h"



// 借壳执行
int mFakeShellRun(LPTSTR shellPath, PCHAR pBufferExe, UINT32 sizeofBufferExe)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(
		shellPath,
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
		return -1;
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	// 重新打开进程线程,以获取权限
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_OPERATION, FALSE, pi.dwProcessId);
	if (!hProcess)
		return -2;
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, false, pi.dwThreadId);
	if (!hThread)
	{
		CloseHandle(hProcess);
		return -3;
	}


	// 获取  线程初始上下文环境
	CONTEXT context = { 0 };
	context.ContextFlags = CONTEXT_FULL;	// 获取所有值.
	if (!GetThreadContext(hThread, &context))
		return -4;

	// 将fileBuffer 转 imageBuffer
	PCHAR pNewBuffer = NULL;
	UINT32 newBufferSize = 0;
	int result = fileBufferToImageBuffer(pBufferExe, sizeofBufferExe, &pNewBuffer, &newBufferSize);
	if (result < 0)
	{
		CloseHandle(hProcess);
		CloseHandle(hThread);
		return -5;
	}

	// 获取模块基址
	INT_PTR image_base = 0;
#if _WIN64
	ReadProcessMemory(hProcess, LPCVOID(context.Rdx + 0x10), &image_base, 8, NULL);
#else
	ReadProcessMemory(hProcess, LPCVOID(context.Ebx + 8), &image_base, 4, NULL);
#endif

	// 卸载目标进程的旧模块,Win10的安全机制问题. 不让直接写exe内存位置. 先卸载,再申请空间写.
	typedef long NTSTATUS;
	typedef NTSTATUS(__stdcall* pfnZwUnmapViewOfSection)(HANDLE ProcessHandle, PVOID  BaseAddress);
	HMODULE hModule = LoadLibrary(TEXT("ntdll.dll"));
	pfnZwUnmapViewOfSection func = (pfnZwUnmapViewOfSection)GetProcAddress(hModule, "ZwUnmapViewOfSection");
	NTSTATUS status = func(hProcess, (PVOID)image_base);  // 这是ntdll.dll 卸载模块的未导出函数
	if (status < 0)
	{
		CloseHandle(hProcess);
		CloseHandle(hThread);
		free(pNewBuffer);
		return -6;
	}

	// 申请新空间,并复制新模块到进程空间.
	PCHAR pNew = (PCHAR)VirtualAllocEx(hProcess, (LPVOID)image_base, newBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!pNew)
	{
		CloseHandle(hProcess);
		CloseHandle(hThread);
		free(pNewBuffer);
		return -7;
	}

	if (!WriteProcessMemory(hProcess, (LPVOID)pNew, pNewBuffer, newBufferSize, NULL))
	{
		CloseHandle(hThread);
		CloseHandle(hProcess);
		free(pNewBuffer);
		VirtualFreeEx(hProcess, (LPVOID)pNew, 0, MEM_RELEASE);
		return -6;
	}

	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pBufferExe);

#ifdef _WIN64
	context.Rcx = image_base + pNt64->OptionalHeader.AddressOfEntryPoint;
#else
	context.Eax = image_base + pNt32->OptionalHeader.AddressOfEntryPoint;
#endif

	context.ContextFlags = CONTEXT_FULL;	// 获取所有值.
	SetThreadContext(hThread, &context);

	ResumeThread(hThread);

	free(pNewBuffer);

	// 等待子进程结束
	// WaitForSingleObject(hProcess, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);

	return 0;
}


// 内存执行
int mMemRun(PCHAR pBufferExe, UINT32 sizeofBufferExe)
{
	typedef void (*OEP)(PVOID imageBase);

	__try
	{
		char str[200];
		// 将fileBuffer 转 imageBuffer
		PCHAR pNewBuffer = NULL;
		UINT32 newBufferSize = 0;
		int result = fileBufferToImageBuffer(pBufferExe, sizeofBufferExe, &pNewBuffer, &newBufferSize);
		if (result < 0)
		{
			MessageBoxA(0, "Er1", 0, 0);
			return -1;
		}

		// 要给可执行权限
		PCHAR pNew = (PCHAR)VirtualAlloc(NULL, newBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!pNew)
		{
			MessageBoxA(0, "Er2", 0, 0);
			return -2;
		}
		memcpy(pNew, pNewBuffer, newBufferSize);

		// 进行重定位
		result = relocateImageBuffer(pNew, (UINT64)pNew);
		if (result < 0)
		{
			MessageBoxA(0, "Er2", 0, 0);
			return -2;
		}

		// 修正导入表
		result = fixImportIATInImageBuffer(pNew);
		if (result < 0)
		{
			MessageBoxA(0, "Er3", 0, 0);
			return -3;
		}

		OEP oep = (OEP)(pNew+ getExportItemRvaByNameInMemBuffer(pNew, (PCHAR)"EntryFunc"));
		sprintf_s(str, "EntryFunc:%p, pNewBuffer:%p", oep, pNew);
		MessageBoxA(0, str, 0, 0);

		// 调用入口函数
		oep(pNewBuffer);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -4;
	}
	return 0;
}


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	__try
	{
		CHAR str[0x200];
		int result;
		// 解析自己
		PE_VAR_DEFINITION;
		PE_VAR_ASSIGN(GetModuleHandle(0));

		// 将最后一个节的内容解密
		PCHAR pBuffer = (PCHAR)calloc(1, pSec[pNt32->FileHeader.NumberOfSections - 1].SizeOfRawData);
		if (!pBuffer)
			return -1;
		memcpy(pBuffer, p + pSec[pNt32->FileHeader.NumberOfSections - 1].VirtualAddress, pSec[pNt32->FileHeader.NumberOfSections - 1].SizeOfRawData);
		for (size_t i = 0; i < pSec[pNt32->FileHeader.NumberOfSections - 1].SizeOfRawData / 4; i++)
			*(UINT32*)(pBuffer + i * sizeof(int)) ^= 0x23333333;

#if 0
		// 伪装成shell执行
		TCHAR shellPath[MAX_PATH];
		GetModuleFileName(0, shellPath, MAX_PATH);
		result = mFakeShellRun(shellPath,pBuffer, pSec[pNt32->FileHeader.NumberOfSections - 1].SizeOfRawData); // 伪装成shell执行
#else
		// 直接内存执行
		result = mMemRun(pBuffer, pSec[pNt32->FileHeader.NumberOfSections - 1].SizeOfRawData); // 直接内存执行
#endif
		if (result < 0)
		{
			sprintf_s(str, "Error:%d", result);
			MessageBoxA(0, str, 0, 0);
			return -1;
		}
		else
		{
			MessageBox(0, TEXT("Success"), 0, 0);
			return 0;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}

	return 0;
}
