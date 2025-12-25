#include "PE.h"

static void test()
{
	IMAGE_NT_HEADERS;
	IMAGE_NT_HEADERS32;
	IMAGE_NT_HEADERS64;
}


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

//	改

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

// 文件转换测试
#if 0
文件到镜像，再从镜像到文件的测试
char* pImageBuffer = fileBufferToImageBuffer(m_pBuffer);
char* pFileBuffer = imageBufferToFileBuffer(pImageBuffer);

FILE* pf;
fopen_s(&pf, "f:\\a.exe", "wb");
if (!pf)
	return;
fwrite(pFileBuffer, 1,getPEFileSize(pFileBuffer), pf);
fclose(pf);
#endif


// 添加节
PCHAR addSection(PCHAR pBuffer, PCHAR name, UINT32 size, UINT32 charac)
{
	UINT32 fileSize = 0;
	UINT32 i = 0;
	UINT32 newFileSize;
	UINT32 headerFreeSize = 0;
	UINT32 moveNtHeader = 0;
	PCHAR pBegin = NULL;
	PCHAR pEnd = NULL;
	PCHAR pCur = NULL;
	PE_VAR_DEFINITION;
	NEW_PE_VAR_DEFINITION;


	PE_VAR_ASSIGN(pBuffer);

	// 检测头部是否有足够空间写节表
	pBegin = (PCHAR)(pSec + pNt32->FileHeader.NumberOfSections);
	pEnd = (PCHAR)(p + pNt32->OptionalHeader.SizeOfHeaders);
	pCur = pBegin;
	while (pCur < pEnd)
	{
		if (*pCur)
			break;
		pCur++;
		headerFreeSize++;
	}

	// 空间不足，移动NT头
	if (headerFreeSize < sizeof(IMAGE_SECTION_HEADER))
	{
		moveNtHeader = 1;
		headerFreeSize += pDos->e_lfanew - sizeof(IMAGE_DOS_HEADER);
		// 移动之后还是不够,那就没办法了.
		if (headerFreeSize < sizeof(IMAGE_SECTION_HEADER))
			return NULL;
	}

	// 获取文件大小
	fileSize = getPEFileSize(p);
	if (!fileSize)
		return NULL;

	// 新文件大小，对齐处理
	newFileSize = (UINT32)fileAlignment(p, fileSize + size);

	pNew = mAllocBuffer(newFileSize);
	if (!pNew)
		return NULL;

	// 头拷贝
	memcpy(pNew, p, pNt32->OptionalHeader.SizeOfHeaders);

	// 节拷贝
	for (i = 0; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].PointerToRawData)
			memcpy(pNew + pSec[i].PointerToRawData, p + pSec[i].PointerToRawData, pSec[i].SizeOfRawData);
	}

	// 移动NT头紧邻DOS头,并清空原来的空间
	if (moveNtHeader)
	{
		NEW_PE_VAR_ASSIGN(pNew);

		pBegin = (PCHAR)pNt32New;
		pEnd = (PCHAR)(pSecNew + pNt32New->FileHeader.NumberOfSections);
		pCur = pNew + sizeof(IMAGE_DOS_HEADER);
		while (pBegin < pEnd)
		{
			*pCur++ = *pBegin;
			*pBegin++ = 0;
		}
		pDosNew->e_lfanew = sizeof(IMAGE_DOS_HEADER);
	}

	NEW_PE_VAR_ASSIGN(pNew);

	// 新节表
	pSecNew[pNt32New->FileHeader.NumberOfSections].PointerToRawData = getPEFileSize(p);;
	pSecNew[pNt32New->FileHeader.NumberOfSections].SizeOfRawData = (UINT32)fileAlignment(pNew, size);
	pSecNew[pNt32New->FileHeader.NumberOfSections].VirtualAddress = getPEImageSize(p);
	pSecNew[pNt32New->FileHeader.NumberOfSections].Misc.VirtualSize = (UINT32)sectionAlignment(pNew, size);
	pSecNew[pNt32New->FileHeader.NumberOfSections].Characteristics = charac;
	strncpy_s((PCHAR)pSecNew[pNt32New->FileHeader.NumberOfSections].Name, 8, name, _TRUNCATE);

	// 节计数++
	pNt32New->FileHeader.NumberOfSections++;

	// 镜像大小修正
	pNt32New->OptionalHeader.SizeOfImage = getPEImageSize(pNew);

	return pNew;
}

// 修改节
PCHAR modifySection(PCHAR pBuffer, UINT32 index, PCHAR name, UINT32 size, UINT32 charac)
{
	PE_VAR_DEFINITION;
	NEW_PE_VAR_DEFINITION;

	UINT32 fileSize = 0;
	UINT32 newFileSize;
	UINT32 i = 0;
	UINT32 modifySecSize = 0;

	PE_VAR_ASSIGN(pBuffer);

	fileSize = getPEFileSize(p);
	if (!fileSize)
		return NULL;

	if (size && pSec[index].SizeOfRawData != size)
	{
		// 只有最后一个节可以修改大小,且大小不能小于当前
		if (index != (UINT32)pNt32->FileHeader.NumberOfSections - 1 || pSec[index].SizeOfRawData > size)
			return NULL;
		newFileSize = (UINT32)fileAlignment(p, pSec[index].PointerToRawData + size);
		modifySecSize = 1;
	}
	else
		newFileSize = fileSize;

	pNew = mAllocBuffer(newFileSize);
	if (!pNew)
		return NULL;

	// 头拷贝
	memcpy(pNew, p, pNt32->OptionalHeader.SizeOfHeaders);

	// 节拷贝
	for (i = 0; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].PointerToRawData)
			memcpy(pNew + pSec[i].PointerToRawData, p + pSec[i].PointerToRawData, pSec[i].SizeOfRawData);
	}

	NEW_PE_VAR_ASSIGN(pNew);

	if (modifySecSize)
	{
		pSecNew[index].SizeOfRawData = (UINT32)fileAlignment(pNew, size);
		pSecNew[index].Misc.VirtualSize = (UINT32)sectionAlignment(pNew, size);
		pNt32New->OptionalHeader.SizeOfImage = getPEImageSize(pNew);
	}
	strncpy_s((PCHAR)pSecNew[index].Name, 8, name, _TRUNCATE);
	pSecNew[index].Characteristics = charac;

	return pNew;
}

// 合并节
PCHAR mergeSection(PCHAR pBuffer, UINT16 index)
{
	PE_VAR_DEFINITION;
	NEW_PE_VAR_DEFINITION;
	WORD i = 0;
	UINT32 pointerToRawData = 0;	// 合并后节的起始偏移
	UINT32 virtualAddress = 0;		// 合并后节的起始虚拟地址
	UINT32 lastVirtualAddress = 0;	// 最后一个有虚拟地址的节
	UINT32 lastSizeOfRawData = 0;	// 最后一个有虚拟地址的节

	UINT32 sizeOfNewSection = 0;	// 新节的大小

	PE_VAR_ASSIGN(pBuffer);

	pointerToRawData = pSec[index].PointerToRawData;
	if (pSec[index].PointerToRawData == 0)
	{
		for (i = index; i < pNt32->FileHeader.NumberOfSections; i++)
		{
			if (pSec[i].PointerToRawData)
			{
				pointerToRawData = pSec[i].PointerToRawData;
				break;
			}
		}
		if(pointerToRawData == 0)
			return NULL;
	}

	virtualAddress = pSec[index].VirtualAddress;
	if (pSec[index].VirtualAddress == 0)
	{
		for (i = index; i < pNt32->FileHeader.NumberOfSections; i++)
		{
			if (pSec[i].VirtualAddress)
			{
				virtualAddress = pSec[i].VirtualAddress;
				break;
			}
		}
		if(virtualAddress == 0)
			return NULL;
	}


	for (i = index; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].VirtualAddress)
		{
			lastVirtualAddress = pSec[i].VirtualAddress;
			lastSizeOfRawData = pSec[i].SizeOfRawData;
		}
	}
	if (lastVirtualAddress == 0)
		return NULL;

	sizeOfNewSection = (UINT32)fileAlignment(p, lastVirtualAddress - virtualAddress + lastSizeOfRawData);

	pNew = mAllocBuffer(pointerToRawData + sizeOfNewSection);
	if (!pNew)
		return NULL;

	// 拷贝第一个节之前的数据
	memcpy(pNew,p,pointerToRawData);

	// 拷贝后续
	for (i = index; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		// 这个节在文件中存在
		if (pSec[i].PointerToRawData && pSec[i].SizeOfRawData)
		{
			// 有虚拟地址,就要往拉伸后的虚拟地址位置拷贝
			if (pSec[i].VirtualAddress)
				memcpy(pNew + pointerToRawData + pSec[i].VirtualAddress - virtualAddress, p + pSec[i].PointerToRawData, pSec[i].SizeOfRawData);
			else // 没有虚拟地址,那就是文件数据,就要拷到原来的位置去
				memcpy(pNew + pSec[i].PointerToRawData , p + pSec[i].PointerToRawData, pSec[i].SizeOfRawData);
		}
	}
	
	NEW_PE_VAR_ASSIGN(pNew);


	pSecNew[index].PointerToRawData = pointerToRawData;
	pSecNew[index].SizeOfRawData = (UINT32)fileAlignment(p, sizeOfNewSection);

	pSecNew[index].VirtualAddress = virtualAddress;
	pSecNew[index].Misc.VirtualSize = (UINT32)sectionAlignment(p, sizeOfNewSection);
	pSecNew[index].Characteristics = 0xE00000E0;
	strncpy_s((PCHAR)pSecNew[index].Name, 8, ".mergeSec", _TRUNCATE);

	for (i = index + 1; i < pNt32->FileHeader.NumberOfSections; i++)
		memset(pSecNew + i, 0, sizeof(IMAGE_SECTION_HEADER));

	pNt32New->FileHeader.NumberOfSections = index + 1;
	pNt32New->OptionalHeader.SizeOfImage = getPEImageSize(pNew);

	return pNew;
}

// 节修改测试
#if 0
char* fileBuffer;
// fileBuffer = addSection(m_pBuffer, "ttt", 0x2220, 0xE00000E0);

// fileBuffer = modifySection(m_pBuffer, 9,"zzzz",0x3000, 0xE00000E0);
// fileBuffer = modifySection(m_pBuffer, 0,"zzzz",0, 0xE00000E0);

fileBuffer = mergeSection(m_pBuffer, 0);

FILE* pf;
fopen_s(&pf, "f:\\a.exe", "wb");
if (!pf)
return;
fwrite(fileBuffer, 1, getPEFileSize(fileBuffer), pf);
fclose(pf);
#endif


