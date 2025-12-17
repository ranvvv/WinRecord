#include"../common.h"




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				1. 遍历卷,及卷信息

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 通过 C: D: E: ... 遍历卷
static void s1_1()
{
	// 遍历卷: 通过 C: D: E: ...
	DWORD nChars = GetLogicalDriveStrings(0, NULL);  // 统计所需空间
	if (nChars == 0)
		return;
	nChars += sizeof(TCHAR); // \0
	PTCHAR buffer = (PTCHAR)malloc(nChars * sizeof(TCHAR));
	if (buffer == NULL)
		return;

	// 获取卷表 
	GetLogicalDriveStrings(nChars, buffer);
	PTCHAR drive = buffer;
	while (*drive)
	{
		_tprintf(TEXT("%s"), drive);

		UINT driveType = GetDriveType(drive);  // 设备类型

		switch (driveType) {
		case DRIVE_UNKNOWN:
			printf("The drive type cannot be determined.\n");
			break;
		case DRIVE_NO_ROOT_DIR:
			printf("The root path is invalid. For example, no volume is mounted at the path.\n");
			break;
		case DRIVE_REMOVABLE:
			printf("The drive has removable media; for example, a floppy drive, thumb drive, or USB flash drive.\n");
			break;
		case DRIVE_FIXED:
			printf("The drive has fixed media; for example, a hard drive or flash drive.\n");
			break;
		case DRIVE_REMOTE:
			printf("The drive is a remote (network) drive.\n");
			break;
		case DRIVE_CDROM:
			printf("The drive is a CD-ROM drive.\n");
			break;
		case DRIVE_RAMDISK:
			printf("The drive is a RAM disk.\n");
			break;
		default:
			printf("The drive type cannot be determined.\n");
			break;
		}

		UINT64 size1/*实际可用*/, size2/*总空间*/, size3/*可用空间*/;
		GetDiskFreeSpaceEx(drive, (PULARGE_INTEGER)&size1, (PULARGE_INTEGER)&size2, (PULARGE_INTEGER)&size3);
		printf("实际可用:%I64d 总空间:%I64d 可用空间:%I64d \n", size1 / (1024 * 1024), size2 / (1024 * 1024), size3 / (1024 * 1024));

		TCHAR volumeNameBuffer[MAX_PATH + 1];
		DWORD volumeNameSize = MAX_PATH + 1;

		TCHAR fileSystemNameBuffer[MAX_PATH + 1];
		DWORD fileSystemNameSize = MAX_PATH + 1;

		DWORD volumeSerialNumber;
		DWORD maximumComponentLength;
		DWORD fileSystemFlags;

		// 获取卷信息
		BOOL result = GetVolumeInformation(
			drive,
			volumeNameBuffer,
			volumeNameSize,
			&volumeSerialNumber,
			&maximumComponentLength,
			&fileSystemFlags,
			fileSystemNameBuffer,
			fileSystemNameSize
		);

		if (result)
		{
			_tprintf(TEXT(" %s \n"), drive);
			_tprintf(TEXT("Volume name: %s\n"), volumeNameBuffer);
			_tprintf(TEXT("Volume serial number: %08X\n"), volumeSerialNumber);
			_tprintf(TEXT("Maximum component length: %d\n"), maximumComponentLength);
			_tprintf(TEXT("File system flags: %08X\n"), fileSystemFlags);
			_tprintf(TEXT("File system name: %s\n"), fileSystemNameBuffer);
		}
		else {
			_tprintf(TEXT("GetVolumeInformation error : %d\n"), GetLastError());
		}

		drive += lstrlen(drive) + 1;
	}
	free(buffer);
}

// 通过 \\?\Volume{ffd223b7-28d4-43e7-8240-d8e4d29fb9b3}\ 遍历卷
static void s1_2()
{
	// 卷位标记	:  0x000001FC
	DWORD mark = GetLogicalDrives();
	printf("drives mark : 0x%08X\n", mark);

	// 遍历卷:   \\?\Volume{ffd223b7-28d4-43e7-8240-d8e4d29fb9b3}\ 
	HANDLE hFindVolume;
	WCHAR volumeName[MAX_PATH + 1];
	printf("根据 FindFirstVolume 遍历卷:\n");
	// 查找第一个卷
	hFindVolume = FindFirstVolume(volumeName, MAX_PATH);
	if (hFindVolume == INVALID_HANDLE_VALUE)
	{
		printf("Error finding first volume.\n");
		return;
	}

	// 打印第一个卷的名称
	printf("First volume name: %ws\n", volumeName);

	// 查找其他卷
	while (FindNextVolume(hFindVolume, volumeName, MAX_PATH))
	{
		printf("Next volume name: %ws\n", volumeName);
		UINT64 size1/*实际可用*/, size2/*总空间*/, size3/*可用空间*/;
		GetDiskFreeSpaceEx(volumeName, (PULARGE_INTEGER)&size1, (PULARGE_INTEGER)&size2, (PULARGE_INTEGER)&size3);
		printf("\t实际可用:%I64d 总空间:%I64d 可用空间:%I64d \n", size1 / (1024 * 1024), size2 / (1024 * 1024), size3 / (1024 * 1024));
	}

	// 关闭句柄
	FindVolumeClose(hFindVolume);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				2. 目录操作

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s2()
{
	BOOL result = CreateDirectory(TEXT("E://test"), NULL);
	if (!result)
	{
		switch (GetLastError())
		{
		case ERROR_ALREADY_EXISTS:
			printf("目录已存在\n");
			break;
		case ERROR_PATH_NOT_FOUND:
			printf("路径错误\n");
			break;
		default:
			break;
		}
	}

	TCHAR buf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buf);
	_tprintf(TEXT("%s\n"), buf);
	SetCurrentDirectory(TEXT("e://"));
	_tprintf(TEXT("%s\n"), buf);
	GetCurrentDirectory(MAX_PATH, buf);
	_tprintf(TEXT("%s\n"), buf);
	GetModuleFileName(GetModuleHandle(NULL), buf, MAX_PATH);
	_tprintf(TEXT("%s\n"), buf);


}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				3. 文件操作

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s3()
{
	// 定义文件路径和打开模式
	PTCHAR filePath = (PTCHAR)TEXT("E:\\file.txt");
	DWORD desiredAccess = GENERIC_READ | GENERIC_WRITE; // 可读写
	DWORD shareMode = FILE_SHARE_READ;					// 共享模式
	LPSECURITY_ATTRIBUTES securityAttributes = NULL;
	DWORD creationDisposition = OPEN_ALWAYS;				// OPEN_ALWAYS;有则打开,无则创建  //  OPEN_EXISTING : 打开已存在的文件LPCSTR
	DWORD flagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	// 打开文件
	hFile = CreateFile(filePath, desiredAccess, shareMode, securityAttributes, creationDisposition, flagsAndAttributes, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed with error %d\n", GetLastError());
		return;
	}

	// 文件打开成功
	printf("File opened successfully\n");

	DWORD dwWrite;
	char text[] = "这是写入的文本内容11111";
	if (!WriteFile(hFile, text, (int)strlen(text), &dwWrite, NULL))
	{
		printf("WriteFile failed with error %d\n", GetLastError());
		CloseHandle(hFile);
		return;
	}


	// 获取文件大小
	DWORD highSize;
	DWORD fileSize = GetFileSize(hFile, &highSize);
	if (fileSize == -1 && GetLastError() != NO_ERROR) {
		printf("无法获取文件大小\n");
		CloseHandle(hFile);
		return;
	}

	// 文件大小可能大于4GB，需要处理高位
	__int64 size = (__int64)highSize << 32 | fileSize;

	printf("文件大小: %I64d 字节\n", size);

	// 上边的写入操作会将文件指针拉到最后,所以直接读会读不出数据, 修复文件指针位置
	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		printf("SetFilePointer failed with error %d\n", GetLastError());
		CloseHandle(hFile);
		return;
	}

	char buf[1024];
	memset(buf, 0, 1024);
	DWORD outSize;
	if (!ReadFile(hFile, buf, (DWORD)1024, &outSize, NULL))
	{
		printf("ReadFile failed with error %d\n", GetLastError());
		CloseHandle(hFile);
		return;
	}
	printf(buf);


	// 关闭文件
	if (!CloseHandle(hFile)) {
		printf("CloseHandle failed with error %d\n", GetLastError());
	}


	// DeleteFile();
	// CopyFile();
	// MoveFile()
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//			    4. 查找目录下的文件

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s4()
{
	WIN32_FIND_DATA NextInfo = { 0 };
	HANDLE hFile;
	TCHAR path[] = TEXT("E:\\test\\*.sys");

	hFile = FindFirstFile(path, &NextInfo);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("FindFirstFile Error : %d\n", GetLastError());
		return;
	}

	do {

		if (lstrcmp(NextInfo.cFileName, TEXT(".")) == 0 || lstrcmp(NextInfo.cFileName, TEXT("..")) == 0)
			continue;

		_tprintf(TEXT("%s\n"), NextInfo.cFileName);


		// 加了密的
		if (NextInfo.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
		{

		}

		// 隐藏文件
		if (NextInfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
		{

		}

		// 目录
		if (NextInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{

		}

	} while (FindNextFile(hFile, &NextInfo));

}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//			    5. 列出所有文件,递归

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


static void listFiles(PTCHAR dirPath, int deep = 0)
{
	WIN32_FIND_DATA NextInfo = { 0 };
	HANDLE hFile;
	TCHAR fullPath[MAX_PATH] = { 0 };
	lstrcpy(fullPath, dirPath);
	lstrcat(fullPath, TEXT("\\*"));

	hFile = FindFirstFile(fullPath, &NextInfo);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("FindFirstFile Error : %d\n", GetLastError());
		return;
	}

	do {

		if (lstrcmp(NextInfo.cFileName, TEXT(".")) == 0 || lstrcmp(NextInfo.cFileName, TEXT("..")) == 0)
			continue;

		int i = 0;
		while (i++ < deep)
			printf("\t");
		_tprintf(TEXT("%s\n"), NextInfo.cFileName);

		// 目录
		if (NextInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			memset(fullPath, 0, MAX_PATH * sizeof(TCHAR));
			lstrcpy(fullPath, dirPath);
			lstrcat(fullPath, TEXT("\\"));
			lstrcat(fullPath, NextInfo.cFileName);
			listFiles(fullPath, deep + 1);
		}
	} while (FindNextFile(hFile, &NextInfo));
}


static void s5()
{
	listFiles((PTCHAR)TEXT("e:\\test"));
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//			    6. 文件属性

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


static void s6()
{
	// 文件或目录的路径
	TCHAR filePath[] = TEXT("e:\\a.sys");

	// 获取文件属性
	DWORD attributes = GetFileAttributes(filePath);

	if (attributes == INVALID_FILE_ATTRIBUTES)
	{
		// 文件或目录不存在或者获取属性失败
		printf("文件或目录不存在或者获取属性失败。错误代码：%d\n", GetLastError());
		return;
	}

	printf("attributes : 0x%x\n", attributes);

	// 检查文件属性
	if (attributes & FILE_ATTRIBUTE_READONLY)
	{
		printf("文件是只读的。\n");
	}
	if (attributes & FILE_ATTRIBUTE_HIDDEN)
	{
		printf("文件是隐藏的。\n");
	}

	WIN32_FILE_ATTRIBUTE_DATA fileInfo;
	if (GetFileAttributesEx(filePath, GetFileExInfoStandard, &fileInfo))
	{
		printf("文件大小: %d 字节\n", (fileInfo.nFileSizeHigh * (MAXDWORD + 1)) + fileInfo.nFileSizeLow);
		printf("创建时间: %d\n", fileInfo.ftCreationTime.dwLowDateTime);
	}
	else {
		printf("获取文件属性失败，错误代码: %d\n", GetLastError());
	}

}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_010()
{
}
