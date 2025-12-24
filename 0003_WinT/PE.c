#include "PE.h"



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//	功能函数

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 对齐函数
UINT64 align(UINT64 num, UINT64 standard)
{
	UINT64 remainder = num % standard;
	if (remainder)
		return num + standard - remainder;
	else
		return num;
	// 	return (a + (standard -1)) & (~(standard - 1));
}

// 文件对齐
UINT64 fileAlignment(const char* const pBuffer, UINT64 num)
{
	return align(num, PNT32(pBuffer)->OptionalHeader.FileAlignment);
}

// 节对齐
UINT64 sectionAlignment(const char* const pBuffer, UINT64 num)
{
	return align(num, PNT32(pBuffer)->OptionalHeader.SectionAlignment);
}

// rva到foa
UINT32 rvaToFoa(const char* const pBuffer, UINT32 rva)
{
	USHORT i;
	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pBuffer);

	if (rva < pNt32->OptionalHeader.SizeOfHeaders)
		return rva;

	for (i = 0; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].VirtualAddress <= rva && rva < (pSec[i].VirtualAddress + pSec[i].SizeOfRawData))
			return pSec[i].PointerToRawData + (rva - pSec[i].VirtualAddress);
	}
	return (UINT32)-1;
}

// foa到rva
UINT32 foaToRva(const char* const pBuffer, UINT32 foa)
{
	USHORT i;
	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pBuffer);

	if (foa < pNt32->OptionalHeader.SizeOfHeaders)
		return foa;

	for (i = 0; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].PointerToRawData <= foa && foa < (pSec[i].PointerToRawData + pSec[i].SizeOfRawData))
			return pSec[i].VirtualAddress + (foa - pSec[i].PointerToRawData);
	}
	return (UINT32)-1;
}

// 内存分配
PCHAR mAllocBuffer(UINT32 size)
{
	PCHAR pNew;
#ifdef _KERNEL_MODE
	pNew = ExAllocatePoolWithTag(NonPagedPool, size, 'PE');
#else
	pNew = (PCHAR)malloc(size);
#endif
	if (pNew)
		memset(pNew, 0, size);
	return pNew;
}

// 内存释放
void mFreeBuffer(PCHAR pBuffer)
{
	if (pBuffer)
#ifdef _KERNEL_MODE
		ExFreePool(pBuffer);
#else
		free(pBuffer);
#endif
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//	查 

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 检查PE文件有效性
int IsValidPE(const char* pBuffer, size_t bufferSize)
{
	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pBuffer);

	// 1. 检查DOS头签名"MZ"
	if (pDos->e_magic != IMAGE_DOS_SIGNATURE)
		return -1;

	// 2. 检查PE签名"PE\0\0"
	if (pNt32->Signature != IMAGE_NT_SIGNATURE)
		return -2;
	
	// 3. 检查节大小是否超过文件大小
	USHORT i;
	UINT32 fileSize = 0;
	for (i = 0; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].SizeOfRawData > 0) // 找到最后一个有数据的节，并更新文件大小
			fileSize = pSec[i].PointerToRawData + pSec[i].SizeOfRawData;
	}
	if (fileSize > bufferSize)
		return -3;

	return 0;
}

// 获取文件大小
UINT32 getPEFileSize(const char* const pBuffer)
{
	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pBuffer);
	USHORT i;
	UINT32 fileSize = 0;
	for (i = 0; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].SizeOfRawData > 0) // 找到最后一个有数据的节，并更新文件大小
			fileSize = pSec[i].PointerToRawData + pSec[i].SizeOfRawData;
	}
	return (UINT32)fileAlignment(pBuffer, fileSize);
}

// 获取镜像大小
UINT32 getPEImageSize(const char* const pBuffer)
{
	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pBuffer);
	USHORT i;
	UINT32 imageSize = 0;
	for (i = 0; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].VirtualAddress && (pSec[i].SizeOfRawData > 0 || pSec[i].Misc.VirtualSize > 0)) // 找到最后一个有数据的节，并更新镜像大小
			imageSize = pSec[i].VirtualAddress + max(pSec[i].SizeOfRawData, pSec[i].Misc.VirtualSize);
	}
	return (UINT32)sectionAlignment(pBuffer, imageSize);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//	改 : 已处理异常

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 文件到镜像
char* fileBufferToImageBuffer(const char* const pBuffer)
{
	char* pNew = NULL;
	UINT32 imageSize = 0;
	UINT32 fileSize = 0;
	UINT32 i = 0;

	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pBuffer);

	imageSize = getPEImageSize(p);
	if (!imageSize)
		return NULL;

	fileSize = getPEFileSize(p);
	if (!fileSize)
		return NULL;

	if (imageSize < fileSize)
		return NULL;

	pNew = mAllocBuffer(imageSize);
	if (!pNew)
		return NULL;

	// 头拷贝
	memcpy(pNew, p, pNt32->OptionalHeader.SizeOfHeaders);

	// 节拷贝
	for (i = 0; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].VirtualAddress && pSec[i].PointerToRawData && pSec[i].SizeOfRawData)
			memcpy(pNew + pSec[i].VirtualAddress, p + pSec[i].PointerToRawData, pSec[i].SizeOfRawData);
	}

	return pNew;
}

// 镜像到文件
char* imageBufferToFileBuffer(const char* const pBuffer)
{
	UINT32 imageSize = 0;
	UINT32 fileSize = 0;
	UINT32 i = 0;
	PCHAR pNew = NULL;

	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pBuffer);

	imageSize = getPEImageSize(p);
	if (!imageSize)
		return NULL;

	fileSize = getPEFileSize(p);
	if (!fileSize)
		return NULL;

	pNew = mAllocBuffer(fileSize);
	if (!pNew)
		return NULL;

	// 头拷贝
	memcpy(pNew, p, pNt32->OptionalHeader.SizeOfHeaders);

	// 节拷贝
	for (i = 0; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].VirtualAddress && pSec[i].PointerToRawData && pSec[i].SizeOfRawData)
			memcpy(pNew + pSec[i].PointerToRawData, p + pSec[i].VirtualAddress, pSec[i].SizeOfRawData);
	}

	return pNew;
}
