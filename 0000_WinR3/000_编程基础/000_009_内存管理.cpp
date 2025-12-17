#include"../common.h"
#include<list>
#include<iostream>


//				1. 堆管理
//				2. 虚拟内存管理
//				3. 共享内存
//				4. 内存映射文件
//				5. 跨进程读内存


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				1. 堆管理

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s1()
{
	HANDLE hHeapDefault = GetProcessHeap();
	printf("deafult heap : %p\n", (PVOID)hHeapDefault);

	HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE | HEAP_GROWABLE, 1024, 0);
	if (hHeap == NULL) {
		printf("Failed to create heap: %lu\n", GetLastError());
		return;
	}

	// 使用 hHeap 进行内存分配和释放操作...
	printf("new heap : %p\n", (PVOID)hHeap);

	HANDLE arrHandle[20];
	DWORD num = GetProcessHeaps(20, arrHandle);
	for (size_t i = 0; i < num; i++)
		printf("--- : %p\n", arrHandle[i]);


	LPVOID pMem = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 0x200);
	printf("%p\n", pMem);
	pMem = HeapReAlloc(hHeap, HEAP_ZERO_MEMORY, pMem, 0x400);
	printf("%p\n", pMem);

	HeapFree(hHeap, HEAP_ZERO_MEMORY, pMem);

	// 销毁堆
	if (!HeapDestroy(hHeap)) {
		printf("Failed to destroy heap: %lu\n", GetLastError());
		return;
	}

	SYSTEM_INFO sysInfo;

	// 调用 GetSystemInfo 函数填充 SYSTEM_INFO 结构
	GetSystemInfo(&sysInfo);

	// 输出一些系统信息
	printf("Processor type: %u\n", sysInfo.dwProcessorType);
	printf("Minimum application address: %p\n", sysInfo.lpMinimumApplicationAddress);
	printf("Maximum application address: %p\n", sysInfo.lpMaximumApplicationAddress);
	printf("Active processor mask: %p\n", (PVOID)sysInfo.dwActiveProcessorMask);
	printf("Number of processors: %u\n", sysInfo.dwNumberOfProcessors);
	printf("内存分配颗粒度 dwAllocationGranularity: %x\n", sysInfo.dwAllocationGranularity);
	printf("页大小 Page size: %u\n", sysInfo.dwPageSize);
}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				2. 虚拟内存管理

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s2()
{
	// 1. 私有内存申请
	LPVOID p = VirtualAlloc(NULL, 0X1000 * 2, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!p)
	{
		mDbgPrint(TEXT("VirtualAlloc Error : %d"), GetLastError());
		return;
	}
	*(int*)p = 0x12345678;
	printf("0x%08x\n", *(int*)p);

	DWORD oldProtect;
	VirtualProtect(p, 0x1000 * 2, PAGE_EXECUTE, &oldProtect);


	VirtualFree(p, 0, MEM_RELEASE);

	/*
	CopyMemory();
	FillMemory();
	MoveMemory();
	ZeroMemory();
	IsBadCodePtr();
	IsBadWritePtr();
	IsBadReadPtr();
	*/


}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				3. 共享内存

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s3()
{
	// 1. 创建共享内存
	HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000 * 4, TEXT("???????"));
	if (!hMapFile)
	{
		mDbgPrint(TEXT("CerateFileMapping Error : %d"), GetLastError());
		return;
	}

	// 2. 映射共享内存,让这块共享内存可通过虚拟地址访问。
	LPVOID  pMapFile = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0x1000 * 4);
	if (!pMapFile)
	{
		mDbgPrint(TEXT("MapViewOfFile Error : %d"), GetLastError());
		return;
	}
	*(int*)pMapFile = 0x22222222;
	printf("0x%08x\n", *(int*)pMapFile);

	// 3. 取消映射共享内存
	UnmapViewOfFile(pMapFile);

	// 4. 关闭共享内存
	CloseHandle(hMapFile);
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				4. 内存映射文件

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#include<psapi.h>

static void s4()
{
	HANDLE hFile;
	HANDLE hMapFile;
	// DWORD dwFIleMapSize;
	LPVOID lpAddr;


	//1. 创建文件设备
	hFile = CreateFile(
		TEXT("./a.txt"),					// 文件名
		GENERIC_READ | GENERIC_WRITE,		// 对文件进行操作的方式
		FILE_SHARE_READ,					// 指定文件共享的方式,允许其他进程的操作
		0,									// 文件安全属性, 0 默认
		OPEN_ALWAYS,						// 
		FILE_FLAG_SEQUENTIAL_SCAN,			// 文件的标志和属性.
		0									// 一般不使用. 模板
	);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile: 失败: %d\n", GetLastError());
		return;
	}

	//2. 创建filemapping 这个函数不仅可以申请物理页,还可以在物理页中映射文件.
	hMapFile = CreateFileMapping(
		hFile,					// 句柄
		NULL,					// 安全属性, 0 默认
		PAGE_READWRITE,			// 内存页属性, 只读,可写?
		0,						// 内存大小高32     
		200,					// 内存你大小低32位
		NULL					// 文件映射对象的名称.可以通过这个在多个进程间共享
	);
	if (hMapFile == NULL)
	{
		printf("CreateeileMaping : 失败: %d\n", GetLastError());
		CloseHandle(hFile);
		return;
	}

	//3. 映射物理页到虚拟内存: 这个返回值就是虚拟内存的地址,
	lpAddr = MapViewOfFile(
		hMapFile,			// 句柄
		FILE_MAP_ALL_ACCESS | FILE_MAP_COPY,		// 内存映射文件的访问权限
		0,					// 偏移量高32
		0,					// 偏移量低32
		0					// 指定映射文件的内容大小: 0 全部映射
	);
	// 对这个虚拟内存的操作就相当于 直接操作硬盘上的文件.
	if (!lpAddr)
	{
		printf("映射失败");
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		return;
	}

	// 写
	*(PDWORD)lpAddr = 0x11111111;

	// 读
	DWORD dwText = *(PDWORD)lpAddr;


	// 强制刷新缓存:
		//  为了保证效率, 不是实时写到硬盘上,只是写入缓存,需要刷新,
		//	何时更新:  释放资源时.或手动刷新.
	// 强制更新 :  地址,   几个字节
	FlushViewOfFile((PDWORD)lpAddr, 4);

	// 关闭资源
	UnmapViewOfFile(lpAddr);
	CloseHandle(hMapFile);
	CloseHandle(hFile);


	//HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,TEXT("MyFileMapping"));
	//if (hMapFile == NULL) 
	//{
	//	std::cerr << "打开文件映射失败，错误代码: " << GetLastError() << std::endl;
	//	return;
	//}
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				5. 跨进程读内存

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#ifdef _WIN64
#define ONE_G_SIZE (1024LLU*1024LLU*1024LLU)
#else
#define ONE_G_SIZE (1024*1024*1024)
#endif

#define PAGE_SIZE 1024*4
std::list<PVOID> lAddr;

BYTE page[PAGE_SIZE];

static BOOL compareOnePage(HANDLE hProcess, PCHAR baseAddr, DWORD value)
{
	if (!ReadProcessMemory(hProcess, baseAddr, page, 1024 * 4, NULL))
		return FALSE;

	for (int i = 0; i < PAGE_SIZE - 3; ++i)
	{
		if (*(DWORD*)(page + i) == value)
		{
			printf("get one : 0x%p\n", baseAddr + i);
			lAddr.push_back(baseAddr + i);
		}
	}
	return TRUE;
}

static int a = 0x12345678;
static int b = 0x12345678;

static void findFirst(DWORD value)
{
#ifdef _WIN64
	PCHAR limit = (PCHAR)(1LL << 47);
#else
	PCHAR limit = (PCHAR)(1 << 31);
#endif

	lAddr.clear();
	HANDLE hProcess = GetCurrentProcess();
	for (PCHAR base = 0; base < limit; base += PAGE_SIZE)
		compareOnePage(hProcess, base, value);
}

static void findNext(DWORD value)
{
	for (std::list<PVOID>::iterator it = lAddr.begin(); it != lAddr.end(); )
	{
		std::list<PVOID>::iterator it_t = it++;
		if (*(PDWORD)*it_t != value)
			lAddr.erase(it_t);
	}
}

static void s5()
{
	printf("a addr : %p  b addr : %p\n", &a, &b);

	b = 0x12345678;
	findFirst(0x12345678);
	//for (std::list<PVOID>::iterator it = lAddr.begin(); it != lAddr.end(); ++it)
		//std::cout << *it << std::endl;
	b = 0x11111111;
	findNext(0x11111111);
	std::cout << "next find" << std::endl;
	for (std::list<PVOID>::iterator it = lAddr.begin(); it != lAddr.end(); ++it)
		std::cout << *it << std::endl;

}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				6. 内存函数

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s6()
{
	CHAR a[1024];
	CHAR b[1024];
	CopyMemory(a, b, 1024);
	FillMemory(a, 0x11, 1024);
	MoveMemory(a, b, 1024);
	ZeroMemory(a, 1024);

	//IsBadCodePtr(a);
	//IsBadReadPtr(a, 1024);
	//IsBadWritePtr(a, 1024);
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void p000_009()
{
	s5();
}