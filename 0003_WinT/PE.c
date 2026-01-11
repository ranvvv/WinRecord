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
UINT64 fileAlignment(PCHAR pBuffer, UINT64 num)
{
	return align(num, PNT32(pBuffer)->OptionalHeader.FileAlignment);
}

// 节对齐
UINT64 sectionAlignment(PCHAR pBuffer, UINT64 num)
{
	return align(num, PNT32(pBuffer)->OptionalHeader.SectionAlignment);
}

// rva到foa
UINT32 rvaToFoa(PCHAR pBuffer, UINT32 rva)
{
	PE_VAR_DEFINITION;
	WORD i = 0;

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
UINT32 foaToRva(PCHAR pBuffer, UINT32 foa)
{
	PE_VAR_DEFINITION;
	WORD i = 0;

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
	PCHAR pNew = NULL;
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
VOID mFreeBuffer(PCHAR pBuffer)
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
int IsValidPE(PCHAR pBuffer, UINT32 bufferSize,UINT32 isMemImage)
{
	/**
	 * @brief				检查PE文件有效性
	 * @param pBuffer		PE文件缓冲区地址
	 * @param bufferSize	PE文件缓冲区大小
	 * @return				0:成功, <0:失败:-1:文件大小不足, -2:DOS头签名不正确, -3:PE头签名不正确
	*/

	PE_VAR_DEFINITION;
	IMAGE_NT_HEADERS32* pNt32Tmp = NULL;

	// 文件大小不能小于DOS头和PE头的最小长度
	if (bufferSize < sizeof(IMAGE_DOS_HEADER) || 
		bufferSize < (UINT32)((IMAGE_DOS_HEADER*)pBuffer)->e_lfanew ||
		bufferSize < (UINT32)((IMAGE_DOS_HEADER*)pBuffer)->e_lfanew + sizeof(IMAGE_NT_HEADERS32))
		return -1;

	// 文件大小不能小于节头大小
	pNt32Tmp = (IMAGE_NT_HEADERS32*)(pBuffer + ((IMAGE_DOS_HEADER*)pBuffer)->e_lfanew);
	if(bufferSize < pNt32Tmp->OptionalHeader.SizeOfHeaders)
		return -1;

	// 头部大小足够,进行详细配置
	PE_VAR_ASSIGN(pBuffer);
		
	// 检查DOS头签名"MZ"
	if (pDos->e_magic != IMAGE_DOS_SIGNATURE)
		return -2;

	// 检查PE签名"PE\0\0"
	if (pNt32->Signature != IMAGE_NT_SIGNATURE)
		return -3;
	
	// 检查节大小是否超过文件大小
	if (isMemImage)
	{
		if (bufferSize < getPEImageSize(pBuffer))
			return -1;
	}
	else
	{
		if (bufferSize < getPEFileSize(pBuffer))
			return -1;
	}

	return 0;
}

// 获取文件大小
UINT32 getPEFileSize(PCHAR pBuffer)
{
	PE_VAR_DEFINITION;
	USHORT i = 0;
	UINT32 fileSize = 0;

	PE_VAR_ASSIGN(pBuffer);

	for (i = 0; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].PointerToRawData  && pSec[i].SizeOfRawData > 0)			// 找到最后一个有数据的节，并更新文件大小
			fileSize = pSec[i].PointerToRawData + pSec[i].SizeOfRawData;
	}

	return (UINT32)fileAlignment(pBuffer, fileSize);
}

// 获取镜像大小
UINT32 getPEImageSize(PCHAR pBuffer)
{
	PE_VAR_DEFINITION;
	USHORT i = 0;
	UINT32 imageSize = 0;

	PE_VAR_ASSIGN(pBuffer);

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
int fileBufferToImageBuffer(PCHAR pBuffer,UINT32 bufferSize,PCHAR* ppNewBufferOut,UINT32* pSizeOut)
{
	PE_VAR_DEFINITION;
	PCHAR pNew = NULL;
	UINT32 imageSize = 0;
	UINT32 fileSize = 0;
	UINT32 i = 0;

	if (!pBuffer || !bufferSize || !ppNewBufferOut || !pSizeOut)
		return -1;

	PE_VAR_ASSIGN(pBuffer);

	imageSize = getPEImageSize(p);
	if (!imageSize)
		return -2;

	fileSize = getPEFileSize(p);
	if (!fileSize)
		return -3;

	// 新建ImageBuffer缓冲区
	pNew = mAllocBuffer(imageSize);
	if (!pNew)
		return -4;

	// 头拷贝
	memcpy(pNew, p, pNt32->OptionalHeader.SizeOfHeaders);

	// 节拷贝
	for (i = 0; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].VirtualAddress && pSec[i].PointerToRawData && pSec[i].SizeOfRawData)
			memcpy(pNew + pSec[i].VirtualAddress, p + pSec[i].PointerToRawData, pSec[i].SizeOfRawData);
	}

	*ppNewBufferOut = pNew;
	*pSizeOut = imageSize;

	return 0;
}

// 镜像到文件
int imageBufferToFileBuffer(PCHAR pBuffer,UINT32 bufferSize,PCHAR* ppNewBufferOut,UINT32* pSizeOut)
{
	PE_VAR_DEFINITION;
	UINT32 imageSize = 0;
	UINT32 fileSize = 0;
	UINT32 i = 0;
	PCHAR pNew = NULL;

	if (!pBuffer || !bufferSize || !ppNewBufferOut || !pSizeOut)
		return -1;

	PE_VAR_ASSIGN(pBuffer);

	imageSize = getPEImageSize(p);
	if (!imageSize)
		return -2;

	fileSize = getPEFileSize(p);
	if (!fileSize)
		return -3;

	pNew = mAllocBuffer(fileSize);
	if (!pNew)
		return -4;

	// 头拷贝
	memcpy(pNew, p, pNt32->OptionalHeader.SizeOfHeaders);

	// 节拷贝
	for (i = 0; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].VirtualAddress && pSec[i].PointerToRawData && pSec[i].SizeOfRawData)
			memcpy(pNew + pSec[i].PointerToRawData, p + pSec[i].VirtualAddress, pSec[i].SizeOfRawData);
	}

	*ppNewBufferOut = pNew;
	*pSizeOut = fileSize;

	return 0;
}


#if 0
int result;

PCHAR pBufferImage;
UINT32 sizeBufferImage;

result = fileBufferToImageBuffer(m_pBuffer, m_bufferSize, &pBufferImage, &sizeBufferImage);
if (result < 0)
	return;

PCHAR pBufferFile;
UINT32 sizeBufferFile;
result = imageBufferToFileBuffer(pBufferImage, sizeBufferImage, &pBufferFile, &sizeBufferFile);
if (result < 0)
	return;

free(pBufferImage);

#endif

// 添加节
int addSectionInFileBuffer(PCHAR pBuffer, UINT32 bufferSize, PCHAR name, UINT32 size, UINT32 charac, PCHAR* ppNewBufferOut, UINT32* pSizeOut)
{
	PE_VAR_DEFINITION;
	NEW_PE_VAR_DEFINITION;
	UINT32 fileSize = 0;
	UINT32 i = 0;
	UINT32 newFileSize;
	UINT32 headerFreeSize = 0;
	UINT32 moveNtHeader = 0;
	PCHAR pBegin = NULL;
	PCHAR pEnd = NULL;
	PCHAR pCur = NULL;

	if (!pBuffer || !name || !ppNewBufferOut || !pSizeOut)
		return -1;

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
			return -2;
	}

	// 获取文件大小
	fileSize = getPEFileSize(p);
	if (!fileSize)
		return -3;

	// 新文件大小，对齐处理
	newFileSize = (UINT32)fileAlignment(p, (UINT64)(fileSize + size));

	pNew = mAllocBuffer(newFileSize);
	if (!pNew)
		return -4;

	// 头拷贝
	memcpy(pNew, p, pNt32->OptionalHeader.SizeOfHeaders);

	// 节拷贝
	for (i = 0; i < pNt32->FileHeader.NumberOfSections; i++)
	{
		if (pSec[i].PointerToRawData && pSec[i].SizeOfRawData)
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

	*ppNewBufferOut = pNew;
	*pSizeOut = newFileSize;

	return 0;
}

// 修改节
int modifySectionInFileBuffer(PCHAR pBuffer, UINT32 bufferSize, UINT32 index, PCHAR name, UINT32 size, UINT32 charac, PCHAR* ppNewBufferOut, UINT32* pSizeOut)
{
	PE_VAR_DEFINITION;
	NEW_PE_VAR_DEFINITION;

	UINT32 fileSize = 0;
	UINT32 newFileSize;
	UINT32 i = 0;
	UINT32 modifySecSize = 0;
	
	if (!pBuffer || !bufferSize || !name || !ppNewBufferOut || !pSizeOut)
		return -1;

	if (IsValidPE(pBuffer, bufferSize, 0) < 0)
		return -2;

	PE_VAR_ASSIGN(pBuffer);

	fileSize = max(getPEFileSize(p), (UINT32)bufferSize);

	if (size && pSec[index].SizeOfRawData != size)
	{
		// 只有最后一个节可以修改大小,且大小不能小于当前
		if (index != (UINT32)pNt32->FileHeader.NumberOfSections - 1 || pSec[index].SizeOfRawData > size)
			return -3;
		newFileSize = (UINT32)fileAlignment(p, (UINT64)pSec[index].PointerToRawData + (UINT64)size);
		modifySecSize = 1;
	}
	else
		newFileSize = fileSize;

	pNew = mAllocBuffer(newFileSize);
	if (!pNew)
		return -4;

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

	*ppNewBufferOut = pNew;
	*pSizeOut = newFileSize;

	return 0;
}

// 合并节
int mergeSectionInFileBuffer(PCHAR pBuffer,UINT32 bufferSize, UINT16 index, PCHAR* ppNewBufferOut, UINT32* pSizeOut)
{
	PE_VAR_DEFINITION;
	NEW_PE_VAR_DEFINITION;
	WORD i = 0;
	UINT32 pointerToRawData = 0;	// 合并后节的起始偏移
	UINT32 virtualAddress = 0;		// 合并后节的起始虚拟地址
	UINT32 lastVirtualAddress = 0;	// 最后一个有虚拟地址的节
	UINT32 lastSizeOfRawData = 0;	// 最后一个有虚拟地址的节
	UINT32 sizeOfNewSection = 0;	// 新节的大小

	if (!pBuffer || !ppNewBufferOut || !pSizeOut)
		return -1;

	if (IsValidPE(pBuffer, bufferSize, 0) < 0)
		return -2;

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
			return -3;
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
			return -4;
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
		return -5;

	sizeOfNewSection = (UINT32)fileAlignment(p, lastVirtualAddress - virtualAddress + lastSizeOfRawData);

	pNew = mAllocBuffer(pointerToRawData + sizeOfNewSection);
	if (!pNew)
		return -6;

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

	*ppNewBufferOut = pNew;
	*pSizeOut = pointerToRawData + sizeOfNewSection;

	return 0;
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//	导出表操作

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 获取导出项RVA by name  文件缓冲区版本
UINT32 getExportItemRvaByNameInFileBuffer(PCHAR pBuffer, PCHAR name)
{
	PE_VAR_DEFINITION
	IMAGE_EXPORT_DIRECTORY* pTable = NULL;
	UINT32* pFuncRvaTable = NULL;
	UINT32* pNameRvaTable = NULL;
	UINT16* pNameOrdinalTable = NULL;
	UINT32 j = 0;

	PE_VAR_ASSIGN(pBuffer);

	pTable = (IMAGE_EXPORT_DIRECTORY*)RVA_TO_FILE_BUFFER_VA(p, pDir[0].VirtualAddress);
	pFuncRvaTable = (UINT32*)RVA_TO_FILE_BUFFER_VA(p, pTable->AddressOfFunctions);
	pNameRvaTable = (UINT32*)RVA_TO_FILE_BUFFER_VA(p, pTable->AddressOfNames);
	pNameOrdinalTable = (UINT16*)RVA_TO_FILE_BUFFER_VA(p, pTable->AddressOfNameOrdinals);

	for (j = 0; j < pTable->NumberOfNames; j++)
	{
		if (strcmp((PCHAR)RVA_TO_FILE_BUFFER_VA(p, pNameRvaTable[j]), name) == 0)
			return pFuncRvaTable[pNameOrdinalTable[j]];
	}

	return 0;
}

// 获取导出项RVA by name 内存缓冲区版本
UINT32 getExportItemRvaByNameInMemBuffer(PCHAR pBuffer, PCHAR name)
{
	PE_VAR_DEFINITION
	IMAGE_EXPORT_DIRECTORY* pTable = NULL;
	UINT32* pFuncRvaTable = NULL;
	UINT32* pNameRvaTable = NULL;
	UINT16* pNameOrdinalTable = NULL;
	UINT32 j = 0;

	PE_VAR_ASSIGN(pBuffer);

	pTable = (IMAGE_EXPORT_DIRECTORY*)(p + pDir[0].VirtualAddress);
	pFuncRvaTable = (UINT32*)(p + pTable->AddressOfFunctions);
	pNameRvaTable = (UINT32*)(p + pTable->AddressOfNames);
	pNameOrdinalTable = (UINT16*)(p + pTable->AddressOfNameOrdinals);

	for (j = 0; j < pTable->NumberOfNames; j++)
	{
		if (strcmp((PCHAR)(p + pNameRvaTable[j]), name) == 0)
			return pFuncRvaTable[pNameOrdinalTable[j]];
	}

	return 0;
}

// 获取导出项RVA by number    文件缓冲区版本
UINT32 getExportItemRvaByNumberInFileBuffer(PCHAR pBuffer, UINT32 number)
{
	PE_VAR_DEFINITION
	IMAGE_EXPORT_DIRECTORY* pTable = NULL;
	UINT32* pFuncRvaTable = NULL;
	PE_VAR_ASSIGN(pBuffer);
	pTable = (IMAGE_EXPORT_DIRECTORY*)RVA_TO_FILE_BUFFER_VA(p, pDir[0].VirtualAddress);
	pFuncRvaTable = (UINT32*)RVA_TO_FILE_BUFFER_VA(p, pTable->AddressOfFunctions);
	return pFuncRvaTable[number - pTable->Base];
}

// 获取导出项RVA by number    内存缓冲区版本
UINT32 getExportItemRvaByNumberInMemBuffer(PCHAR pBuffer, UINT32 number)
{
	PE_VAR_DEFINITION;
	IMAGE_EXPORT_DIRECTORY* pTable = NULL;
	UINT32* pFuncRvaTable = NULL;
	PE_VAR_ASSIGN(pBuffer);
	pTable = (IMAGE_EXPORT_DIRECTORY*)(p + pDir[0].VirtualAddress);
	pFuncRvaTable = (UINT32*)(p + pTable->AddressOfFunctions);
	return pFuncRvaTable[number - pTable->Base];
}

// 获取导出表大小
UINT32 getExportTableSizeInFileBuffer(PCHAR pBuffer, EXPORT_TABLE_SIZE* pExportTableSize)
{
	/**
	* @brief						获取导出表大小
	* @param pBuffer				文件缓冲区
	* @param pExportTableSize		导出表大小结构体
	* @return						总大小(字节) : 0 无导出表
	*/
	PE_VAR_DEFINITION;
	UINT32* pFuncRvaTable = NULL;
	UINT32* pNameRvaTable = NULL;
	UINT16* pNameOrdinalTable = NULL;
	IMAGE_EXPORT_DIRECTORY* pTable = NULL;
	UINT32 j = 0;

	if(!pBuffer || !pExportTableSize)
		return 0;

	PE_VAR_ASSIGN(pBuffer);

	memset(pExportTableSize, 0, sizeof(EXPORT_TABLE_SIZE));

	if (pDir[0].VirtualAddress == 0)
		return 0;

	pTable = (IMAGE_EXPORT_DIRECTORY*)RVA_TO_FILE_BUFFER_VA(p, pDir[0].VirtualAddress);
	pFuncRvaTable = (UINT32*)RVA_TO_FILE_BUFFER_VA(p, pTable->AddressOfFunctions);
	pNameRvaTable = (UINT32*)RVA_TO_FILE_BUFFER_VA(p, pTable->AddressOfNames);
	pNameOrdinalTable = (UINT16*)RVA_TO_FILE_BUFFER_VA(p, pTable->AddressOfNameOrdinals);

	pExportTableSize->nameSize = (UINT32)strlen((PCHAR)RVA_TO_FILE_BUFFER_VA(p, pTable->Name)) + 1;			// 模块名称
	pExportTableSize->tableSize = (UINT32)sizeof(IMAGE_EXPORT_DIRECTORY);						// 导出表大小
	pExportTableSize->arrFuncRvaSize = pTable->NumberOfFunctions * sizeof(UINT32);				// 函数表大小
	pExportTableSize->arrNameRvaSize = pTable->NumberOfNames * sizeof(UINT32);					// 名称表大小
	pExportTableSize->arrOrdinalSize = pTable->NumberOfNames * sizeof(UINT16);					// 序号表大小

	for (j = 0; j < pTable->NumberOfNames; j++)
		pExportTableSize->nameSize += (UINT32)strlen((char*)RVA_TO_FILE_BUFFER_VA(p, pNameRvaTable[j])) + 1;

	return pExportTableSize->nameSize + pExportTableSize->tableSize + pExportTableSize->arrFuncRvaSize + pExportTableSize->arrNameRvaSize + pExportTableSize->arrOrdinalSize;
}

// 移动导出表
int moveExportTableInFileBuffer(PCHAR pBuffer, UINT32 begin)
{
	/**
	 * @param pBuffer				PE文件
	 * @param begin					新导出表起始位置
	 * return
	 */

	UINT32 i = 0;
	IMAGE_EXPORT_DIRECTORY* pTable = NULL;
	UINT32* pFuncRvaTable = NULL;
	UINT32* pNameRvaTable = NULL;
	UINT16* pNameOrdinalTable = NULL;
	UINT32 len = 0;

	IMAGE_EXPORT_DIRECTORY* pTableNew = NULL;
	UINT32* pFuncRvaTableNew = NULL;
	UINT32* pNameRvaTableNew = NULL;
	UINT16* pNameOrdinalTableNew = NULL;
	PCHAR pNameNew = NULL;

	// 获取大小
	EXPORT_TABLE_SIZE exportTableSize = { 0 };
	getExportTableSizeInFileBuffer(pBuffer, &exportTableSize);

	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pBuffer);

	// 旧表定位
	pTable = (IMAGE_EXPORT_DIRECTORY*)RVA_TO_FILE_BUFFER_VA(p, pDir[0].VirtualAddress);
	pFuncRvaTable = (UINT32*)RVA_TO_FILE_BUFFER_VA(p, pTable->AddressOfFunctions);
	pNameRvaTable = (UINT32*)RVA_TO_FILE_BUFFER_VA(p, pTable->AddressOfNames);
	pNameOrdinalTable = (UINT16*)RVA_TO_FILE_BUFFER_VA(p, pTable->AddressOfNameOrdinals);

	// 新表定位
	pTableNew = (IMAGE_EXPORT_DIRECTORY*)(p + begin);
	pFuncRvaTableNew = (UINT32*)(p + begin + exportTableSize.tableSize);
	pNameRvaTableNew = (UINT32*)(p + begin + exportTableSize.tableSize + exportTableSize.arrFuncRvaSize);
	pNameOrdinalTableNew = (UINT16*)(p + begin + exportTableSize.tableSize + exportTableSize.arrFuncRvaSize + exportTableSize.arrNameRvaSize);
	pNameNew = (PCHAR)(p + begin + exportTableSize.tableSize + exportTableSize.arrFuncRvaSize + exportTableSize.arrNameRvaSize + exportTableSize.arrOrdinalSize);

	// 新表设置
	memcpy(pTableNew, pTable, sizeof(IMAGE_EXPORT_DIRECTORY));
	pTableNew->Name = foaToRva(p, (UINT32)(pNameNew - p));
	pTableNew->AddressOfFunctions = foaToRva(p, (UINT32)((PCHAR)pFuncRvaTableNew - p));
	pTableNew->AddressOfNameOrdinals = foaToRva(p, (UINT32)((PCHAR)pNameOrdinalTableNew - p));
	pTableNew->AddressOfNames = foaToRva(p, (UINT32)((PCHAR)pNameRvaTableNew - p));

	// 模块名转移
	len = (UINT32)strlen((PCHAR)RVA_TO_FILE_BUFFER_VA(p, pTable->Name)) + 1;
	strcpy_s(pNameNew, len, (PCHAR)RVA_TO_FILE_BUFFER_VA(p, pTable->Name));
	memset((PCHAR)RVA_TO_FILE_BUFFER_VA(p, pTable->Name), 0, len);
	pNameNew += len;

	// 函数地址表转移
	memcpy(pFuncRvaTableNew, pFuncRvaTable, exportTableSize.arrFuncRvaSize);
	// 函数序号表转移
	memcpy(pNameOrdinalTableNew, pNameOrdinalTable, exportTableSize.arrOrdinalSize);
	// 名字地址表转移
	for (i = 0; i < pTable->NumberOfNames; i++)
	{
		len = (UINT32)strlen((PCHAR)RVA_TO_FILE_BUFFER_VA(p, pNameRvaTable[i])) + 1;
		strcpy_s(pNameNew, len, (PCHAR)RVA_TO_FILE_BUFFER_VA(p, pNameRvaTable[i]));
		memset((PCHAR)RVA_TO_FILE_BUFFER_VA(p, pNameRvaTable[i]), 0, len);
		pNameRvaTableNew[i] = foaToRva(p, (UINT32)(pNameNew - p));
		pNameNew += len;
	}
	// 清理旧表
	memset(pNameOrdinalTable, 0, exportTableSize.arrOrdinalSize);	// 序号表清零
	memset(pFuncRvaTable, 0, exportTableSize.arrFuncRvaSize);		// 函数表清零
	memset(pNameRvaTable, 0, exportTableSize.arrNameRvaSize);		// 名称表清零
	memset(pTable, 0, sizeof(IMAGE_EXPORT_DIRECTORY));				// 导出表清零

	// 更新目录表
	pDir[0].VirtualAddress = foaToRva(p, (UINT32)((PCHAR)pTableNew - p));

	return 0;
}

#if 0
// 移动导出表测试
EXPORT_TABLE_SIZE exportTableSize = { 0 };
UINT32 totalSize = getExportTableSizeInFileBuffer(m_pBuffer, &exportTableSize);
if (totalSize != 0)
{
	PCHAR pNewBuffer;
	UINT32 newBufferSize = 0;
	int result = addSectionInFileBuffer(m_pBuffer, m_bufferSize, "newsec", totalSize, 0xE00000E0, &pNewBuffer, &newBufferSize);
	if (result < 0)
	{
		AfxMessageBox(TEXT("移动导出表失败"));
		return;
		}

	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pNewBuffer);
	moveExportTableInFileBuffer(pNewBuffer, pSec[pNt32->FileHeader.NumberOfSections - 1].PointerToRawData);
	mRefreshPage(pNewBuffer, newBufferSize);
	}
#endif

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//	导入表操作

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// 获取导入表大小
UINT32 getImportTableSizeInFileBuffer(PCHAR pBuffer, IMPORT_TABLE_SIZE* pImportTableSize)
{
	IMAGE_IMPORT_BY_NAME* pImportByName = NULL;
	UINT32* pThunk32 = NULL;
	UINT64* pThunk64 = NULL;
	IMAGE_IMPORT_DESCRIPTOR* pTable = NULL;
	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pBuffer);

	pTable = (IMAGE_IMPORT_DESCRIPTOR*)RVA_TO_FILE_BUFFER_VA(p, pDir[1].VirtualAddress);
	memset(pImportTableSize, 0, sizeof(IMPORT_TABLE_SIZE));

	while (pTable->Name)
	{
		pImportTableSize->importTableSize += (UINT32)sizeof(IMAGE_IMPORT_DESCRIPTOR);
		pImportTableSize->nameSize += (UINT32)strlen((char*)RVA_TO_FILE_BUFFER_VA(p, pTable->Name)) + 1;

		if (isX64(p))
		{
			pThunk64 = (PUINT64)RVA_TO_FILE_BUFFER_VA(p, pTable->OriginalFirstThunk);
			while (*pThunk64)
			{
				if (!(*pThunk64 >> 63))
				{
					pImportByName = (IMAGE_IMPORT_BY_NAME*)RVA_TO_FILE_BUFFER_VA(p, (UINT32)*pThunk64);
					pImportTableSize->nameSize += (UINT32)strlen((char*)pImportByName->Name) + 1 + sizeof(pImportByName->Hint);
				}
				pThunk64++;
				pImportTableSize->INTSize += sizeof(UINT64);
			}
			pImportTableSize->INTSize += sizeof(UINT64);	// 0结尾
		}
		else
		{
			pThunk32 = (PUINT32)RVA_TO_FILE_BUFFER_VA(p, pTable->OriginalFirstThunk);
			while (*pThunk32)
			{
				if (!(*pThunk32 >> 31))
				{
					pImportByName = (IMAGE_IMPORT_BY_NAME*)RVA_TO_FILE_BUFFER_VA(p, (UINT32)*pThunk32);
					pImportTableSize->nameSize += (UINT32)strlen((char*)pImportByName->Name) + 1 + sizeof(pImportByName->Hint);
				}
				pThunk32++;
				pImportTableSize->INTSize += sizeof(UINT32);
			}
			pImportTableSize->INTSize += sizeof(UINT32);	// 0结尾
		}
		pTable++;
	}
	pImportTableSize->importTableSize += (UINT32)sizeof(IMAGE_IMPORT_DESCRIPTOR);	// 0结尾

	return pImportTableSize->importTableSize + pImportTableSize->nameSize + pImportTableSize->INTSize;
}

// 移动导入表
int moveImportTableInFileBuffer(PCHAR pBuffer, UINT32 begin)
{
	IMAGE_IMPORT_DESCRIPTOR* pTableNew = NULL;
	UINT64* pThunk64New = NULL;
	UINT32* pThunk32New = NULL;
	PCHAR pNameNew = NULL;
	int len = 0;
	UINT64* pThunk64 = NULL;
	UINT32* pThunk32 = NULL;
	IMAGE_IMPORT_DESCRIPTOR* pTable = NULL;
	IMAGE_IMPORT_BY_NAME* pImportByName = NULL;
	IMAGE_IMPORT_BY_NAME* pImportByNameNew = NULL;

	IMPORT_TABLE_SIZE importTableSize = { 0 };
	getImportTableSizeInFileBuffer(pBuffer, &importTableSize);
	IMPORT_TABLE_SIZE* pImportTableSize = &importTableSize;

	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pBuffer);

	pTable = (IMAGE_IMPORT_DESCRIPTOR*)RVA_TO_FILE_BUFFER_VA(p, pDir[1].VirtualAddress);

	pTableNew = (IMAGE_IMPORT_DESCRIPTOR*)(p + begin);
	pThunk64New = (UINT64*)((PCHAR)pTableNew + pImportTableSize->importTableSize);
	pThunk32New = (UINT32*)((PCHAR)pTableNew + pImportTableSize->importTableSize);
	pNameNew = (PCHAR)pThunk64New + pImportTableSize->INTSize;

	while (pTable->Name)
	{
		// 导入模块名转移
		memcpy(pTableNew, pTable, sizeof(IMAGE_IMPORT_DESCRIPTOR));
		pTableNew->Name = (UINT32)foaToRva(p, (UINT32)(pNameNew - p));
		len = (UINT32)strlen((char*)RVA_TO_FILE_BUFFER_VA(p, pTable->Name)) + 1;
		strcpy_s(pNameNew, len, (char*)RVA_TO_FILE_BUFFER_VA(p, pTable->Name));
		memset((PCHAR)RVA_TO_FILE_BUFFER_VA(p, pTable->Name), 0, len);
		pNameNew += len;

		if (isX64(p))
		{
			// INT 转移
			pTableNew->OriginalFirstThunk = foaToRva(p, (UINT32)((PCHAR)pThunk64New - p));
			pThunk64 = (PUINT64)RVA_TO_FILE_BUFFER_VA(p, pTable->OriginalFirstThunk);
			while (*pThunk64)
			{
				if (*pThunk64 >> 63)
					*pThunk64New = *pThunk64;	// 序号导入转移
				else
				{
					// 名称导入的名称转移
					pImportByName = (IMAGE_IMPORT_BY_NAME*)RVA_TO_FILE_BUFFER_VA(p, (UINT32)*pThunk64);
					pImportByNameNew = (IMAGE_IMPORT_BY_NAME*)pNameNew;
					pImportByNameNew->Hint = pImportByName->Hint;
					len = (UINT32)strlen((char*)pImportByName->Name) + 1;
					strcpy_s(pNameNew + sizeof(pImportByName->Hint), len, pImportByName->Name);
					memset(pImportByName, 0, len + sizeof(pImportByName->Hint));
					*pThunk64New = foaToRva(p, (UINT32)(pNameNew - p));
					pNameNew += len + sizeof(pImportByName->Hint);
				}
				*pThunk64 = 0;
				pThunk64++;
				pThunk64New++;
			}
			pThunk64New++;	// 0 结尾
		}
		else
		{
			pTableNew->OriginalFirstThunk = foaToRva(p, (UINT32)((PCHAR)pThunk32New - p));
			pThunk32 = (PUINT32)RVA_TO_FILE_BUFFER_VA(p, pTable->OriginalFirstThunk);
			while (*pThunk32)
			{
				if (*pThunk32 >> 31)
					*pThunk32New = *pThunk32;
				else
				{
					pImportByName = (IMAGE_IMPORT_BY_NAME*)RVA_TO_FILE_BUFFER_VA(p, (UINT32)*pThunk32);
					pImportByNameNew = (IMAGE_IMPORT_BY_NAME*)pNameNew;
					pImportByNameNew->Hint = pImportByName->Hint;
					len = (UINT32)strlen((char*)pImportByName->Name) + 1;
					strcpy_s(pNameNew + sizeof(pImportByName->Hint), len, pImportByName->Name);
					memset(pImportByName, 0, len + sizeof(pImportByName->Hint));
					*pThunk32New = foaToRva(p, (UINT32)(pNameNew - p));
					pNameNew += len + sizeof(pImportByName->Hint);
				}
				*pThunk32 = 0;
				pThunk32++;
				pThunk32New++;
			}
			pThunk32New++;
		}
		memset(pTable, 0, sizeof(IMAGE_IMPORT_DESCRIPTOR));
		pTable++;
		pTableNew++;
	}

	pDir[1].VirtualAddress = foaToRva(p, begin);

	return 0;
}

#if 0
// 移动导入表测试
IMPORT_TABLE_SIZE importTableSize = { 0 };
UINT32 totalSize = getImportTableSizeInFileBuffer(m_pBuffer, &importTableSize);
if (totalSize != 0)
{
	PCHAR pNewBuffer;
	UINT32 newBfferSize;
	int result = addSectionInFileBuffer(m_pBuffer, m_bufferSize, "newsec", totalSize, 0xE00000E0, &pNewBuffer, &newBfferSize);
	if (result < 0)
	{
		AfxMessageBox(TEXT("移动导入表失败"));
		return;
		}

	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pNewBuffer);
	moveImportTableInFileBuffer(pNewBuffer, pSec[pNt32->FileHeader.NumberOfSections - 1].PointerToRawData);
	mRefreshPage(pNewBuffer, newBfferSize);
	}

#endif


// 导入表注入 by name
int importTableInjectionByNameInFileBuffer(PCHAR pBuffer,UINT32 bufferSize, PCHAR dllName, PCHAR funcName,PCHAR* ppNewBuffer,UINT32* pSizeOut)
{
	PCHAR pNewBuffer = NULL;
	UINT32 newBufferSize = 0;

	// 添加个节
	IMPORT_TABLE_SIZE importTableSize = { 0 };
	UINT32 totalSize = 0;
	UINT32 begin = 0;
	IMAGE_IMPORT_DESCRIPTOR* pTable = NULL;
	IMAGE_IMPORT_DESCRIPTOR* pTableNew = NULL;
	UINT64* pThunk64New = NULL;
	UINT32* pThunk32New = NULL;
	PCHAR pNameNew = NULL;
	int len = 0;
	int result = 0;

	PE_VAR_DEFINITION;

	// 添加一个新节,用来存放新导入表数据
	getImportTableSizeInFileBuffer(pBuffer, &importTableSize);
	importTableSize.importTableSize += (UINT32)sizeof(IMAGE_IMPORT_DESCRIPTOR);	// 添加一个空间
	totalSize = importTableSize.importTableSize + sizeof(UINT64) * 4 + (UINT32)strlen(dllName) + 1 + (UINT32)strlen(funcName) + 1;
	result = addSectionInFileBuffer(pBuffer, bufferSize, (PCHAR)".add", totalSize, 0xE00000E0,&pNewBuffer,&newBufferSize);
	if (result<0)
		return -1;

	PE_VAR_ASSIGN(pNewBuffer);

	// 定位新表位置
	begin = pSec[pNt32->FileHeader.NumberOfSections - 1].PointerToRawData;

	// 移动导入表
	pTable = (IMAGE_IMPORT_DESCRIPTOR*)RVA_TO_FILE_BUFFER_VA(p, pDir[1].VirtualAddress);

	pTableNew = (IMAGE_IMPORT_DESCRIPTOR*)(p + begin);
	pThunk64New = (UINT64*)((PCHAR)pTableNew + importTableSize.importTableSize);
	pThunk32New = (UINT32*)((PCHAR)pTableNew + importTableSize.importTableSize);
	pNameNew = (PCHAR)pThunk64New + sizeof(UINT64) * 4;

	// 导入表需要移动,因为导入表是连续的,不知道后面有没有足够空间.
	while (pTable->Name)
	{
		memcpy(pTableNew, pTable, sizeof(IMAGE_IMPORT_DESCRIPTOR));
		pTableNew->Name = (UINT32)foaToRva(p, (UINT32)(pNameNew - p));
		len = (UINT32)strlen((char*)RVA_TO_FILE_BUFFER_VA(p, pTable->Name)) + 1;
		strcpy_s(pNameNew, len, (char*)RVA_TO_FILE_BUFFER_VA(p, pTable->Name));
		memset((PCHAR)RVA_TO_FILE_BUFFER_VA(p, pTable->Name), 0, len);
		pNameNew += len;
		memset(pTable, 0, sizeof(IMAGE_IMPORT_DESCRIPTOR));
		pTable++;
		pTableNew++;
	}

	// 设置新导入表.
	pTableNew->Name = foaToRva(p, (UINT32)(pNameNew - p));
	strcpy_s(pNameNew, (UINT32)strlen(dllName) + 1, dllName);
	pNameNew += (UINT32)strlen(dllName) + 1;
	pTableNew->OriginalFirstThunk = foaToRva(p, (UINT32)((PCHAR)pThunk64New - p));				// 新INT
	pTableNew->FirstThunk = foaToRva(p, (UINT32)((PCHAR)pThunk64New - p + sizeof(UINT64) * 2)); // 新IAT
	//pTableNew->TimeDateStamp = pTable->TimeDateStamp;
	//pTableNew->ForwarderChain = pTable->ForwarderChain;

	if (isX64(p))
	{
		*pThunk64New = foaToRva(p, (UINT32)(pNameNew - p));	// 新INT
		*(pThunk64New + 2) = foaToRva(p, (UINT32)(pNameNew - p)); // 新IAT
	}
	else
	{
		*pThunk32New = foaToRva(p, (UINT32)(pNameNew - p));
		*(pThunk32New + 4) = foaToRva(p, (UINT32)(pNameNew - p));
	}
	*pNameNew = 1;
	pNameNew += 2;
	strcpy_s(pNameNew, (UINT32)strlen(funcName) + 1, funcName);

	pDir[1].VirtualAddress = foaToRva(p, begin);

	*ppNewBuffer = pNewBuffer;
	*pSizeOut = newBufferSize;

	return 0;
}

// 导入表注入 by number
int importTableInjectionByNumberInFileBuffer(PCHAR pBuffer, UINT32 bufferSize, PCHAR dllName, UINT32 number ,PCHAR* ppNewBuffer, UINT32* pSizeOut)
{
	PCHAR pNewBuffer = NULL;
	UINT32 newBufferSize = 0;

	// 添加个节
	IMPORT_TABLE_SIZE importTableSize = { 0 };
	UINT32 totalSize = 0;
	UINT32 begin = 0;
	IMAGE_IMPORT_DESCRIPTOR* pTable = NULL;
	IMAGE_IMPORT_DESCRIPTOR* pTableNew = NULL;
	UINT64* pThunk64New = NULL;
	UINT32* pThunk32New = NULL;
	PCHAR pNameNew = NULL;
	int len = 0;
	int result = 0;
	PE_VAR_DEFINITION;

	// 添加一个新节,用来存放新导入表数据
	getImportTableSizeInFileBuffer(pBuffer, &importTableSize);
	importTableSize.importTableSize += (UINT32)sizeof(IMAGE_IMPORT_DESCRIPTOR);	// 添加一个空间
	totalSize = importTableSize.importTableSize + sizeof(UINT64) * 4 + (UINT32)strlen(dllName) + 1;
	result = addSectionInFileBuffer(pBuffer, bufferSize, (PCHAR)".add", totalSize, 0xE00000E0, &pNewBuffer, &newBufferSize);
	if (result < 0)
		return -1;


	PE_VAR_ASSIGN(pNewBuffer);

	begin = pSec[pNt32->FileHeader.NumberOfSections - 1].PointerToRawData;

	pTable = (IMAGE_IMPORT_DESCRIPTOR*)RVA_TO_FILE_BUFFER_VA(p, pDir[1].VirtualAddress);

	pTableNew = (IMAGE_IMPORT_DESCRIPTOR*)(p + begin);
	pThunk64New = (UINT64*)((PCHAR)pTableNew + importTableSize.importTableSize);
	pThunk32New = (UINT32*)((PCHAR)pTableNew + importTableSize.importTableSize);
	pNameNew = (PCHAR)pThunk64New + sizeof(UINT64) * 4;

	while (pTable->Name)
	{
		// 移动导入表
		memcpy(pTableNew, pTable, sizeof(IMAGE_IMPORT_DESCRIPTOR));
		pTableNew->Name = (UINT32)foaToRva(p, (UINT32)(pNameNew - p));
		len = (UINT32)strlen((char*)RVA_TO_FILE_BUFFER_VA(p, pTable->Name)) + 1;
		strcpy_s(pNameNew, len, (char*)RVA_TO_FILE_BUFFER_VA(p, pTable->Name));
		memset((PCHAR)RVA_TO_FILE_BUFFER_VA(p, pTable->Name), 0, len);
		pNameNew += len;
		memset(pTable, 0, sizeof(IMAGE_IMPORT_DESCRIPTOR));
		pTable++;
		pTableNew++;
	}

	// 设置新导入表.
	pTableNew->Name = foaToRva(p, (UINT32)(pNameNew - p));
	strcpy_s(pNameNew, (UINT32)strlen(dllName) + 1, dllName);
	pNameNew += (UINT32)strlen(dllName) + 1;
	pTableNew->OriginalFirstThunk = foaToRva(p, (UINT32)((PCHAR)pThunk64New - p));
	pTableNew->FirstThunk = foaToRva(p, (UINT32)((PCHAR)pThunk64New - p + sizeof(UINT64) * 2));
	pTableNew->TimeDateStamp = pTable->TimeDateStamp;
	pTableNew->ForwarderChain = pTable->ForwarderChain;

	if (isX64(p))
	{
		*pThunk64New = (UINT64)number | (1LL << 63);
		*(pThunk64New + 2) = *pThunk64New;
	}
	else
	{
		*pThunk32New = number | 1 << 31;
		*(pThunk32New + 4) = *pThunk32New;
	}

	pDir[1].VirtualAddress = foaToRva(p, begin);

	*ppNewBuffer = pNewBuffer;
	*pSizeOut = newBufferSize;

	return 0;
}

#if 0
PCHAR pNewBuffer = NULL;
UINT32 newBufferSize = 0;
//int result = importTableInjectionByNameInFileBuffer(m_pBuffer, m_bufferSize,"ttt.dll", "ceshi",&pNewBuffer, &newBufferSize);
int result = importTableInjectionByNumberInFileBuffer(m_pBuffer, m_bufferSize, "ttt.dll", 11, &pNewBuffer, &newBufferSize);
#endif

// 根据导入表修复IAT
int fixImportIATInImageBuffer(PCHAR pBuffer)
{
#ifndef _KERNEL_MODE
	UINT32* pThunk32 = NULL;
	UINT64* pThunk64 = NULL;
	UINT32* pThunk32IAT = NULL;
	UINT64* pThunk64IAT = NULL;
	PCHAR hModule = NULL;
	IMAGE_IMPORT_DESCRIPTOR* pTable = NULL;
	IMAGE_IMPORT_BY_NAME* pImportByName = NULL;
	UINT32 offset = 0;
	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pBuffer);

	if (pDir[1].VirtualAddress == 0)
		return -1;

	pTable = (IMAGE_IMPORT_DESCRIPTOR*)(p + pDir[1].VirtualAddress);
	while (pTable->Name)
	{
		hModule = (PCHAR)LoadLibraryA((char*)(p + pTable->Name));
		if (!hModule)
		{
			return -1;
		}

		if (isX64(p))
		{
			pThunk64 = (PUINT64)(p + pTable->OriginalFirstThunk);
			pThunk64IAT = (PUINT64)(p + pTable->FirstThunk);
			while (*pThunk64)
			{
				if (!((*pThunk64) >> 63))
				{
					pImportByName = (IMAGE_IMPORT_BY_NAME*)(p + (UINT32)*pThunk64);
					offset = getExportItemRvaByNameInMemBuffer((PCHAR)hModule, pImportByName->Name);
				}
				else
					offset = getExportItemRvaByNumberInMemBuffer((PCHAR)hModule, (UINT32)((*pThunk64 << 1) >> 1));
				*pThunk64IAT = (UINT64)hModule + offset;
				pThunk64++;
				pThunk64IAT++;
			}
		}
		else
		{
			pThunk32 = (PUINT32)(p + pTable->OriginalFirstThunk);
			pThunk32IAT = (PUINT32)(p + pTable->FirstThunk);
			while (*pThunk32)
			{
				if (!((*pThunk32) >> 31))
				{
					pImportByName = (IMAGE_IMPORT_BY_NAME*)(p + (UINT32)(*pThunk32));
					offset = getExportItemRvaByNameInMemBuffer((PCHAR)hModule, pImportByName->Name);
				}
				else
				{
					offset = getExportItemRvaByNumberInMemBuffer((PCHAR)hModule, (*pThunk32 << 1) >> 1);
				}
				*pThunk32IAT = (UINT32)(UINT64)hModule + offset;
				pThunk32++;
				pThunk32IAT++;
			}
		}
		pTable++;
	}
	return 0;
#endif
}






// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//	重定位表

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 重定位ImageBuffer
int relocateImageBuffer(PCHAR pBuffer, UINT64 newBase)
{
	UINT32 num = 0;
	UINT32 i = 0;
	PCHAR destAddr = NULL;
	IMAGE_BASE_RELOCATION* pTable = NULL;
	UINT16* pOffset = NULL;
	PE_VAR_DEFINITION;
	PE_VAR_ASSIGN(pBuffer);

	// 没有重定位表,就无法重定位,所以失败
	if (!pDir[5].VirtualAddress)
		return -1;

	pTable = (IMAGE_BASE_RELOCATION*)(p + pDir[5].VirtualAddress);

	while (pTable->VirtualAddress && !(pTable->VirtualAddress & 0xFFF)/*ntosknl.exe有问题,不是0结尾,判断是否对齐来结尾*/)
	{
		pOffset = (UINT16*)(pTable + 1);
		num = (pTable->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2;
		for (i = 0; i < num; i++)
		{
			if (pOffset[i] >> 12 == 0x3 || pOffset[i] >> 12 == 0xA)
			{
				destAddr = (PCHAR)(p + pTable->VirtualAddress + (pOffset[i] & 0x0FFF));
				if (isX64(p))
					*(UINT64*)destAddr = *(UINT64*)destAddr + newBase - pNt64->OptionalHeader.ImageBase;
				else
					*(UINT32*)destAddr = *(UINT32*)destAddr + (UINT32)newBase - pNt32->OptionalHeader.ImageBase;
			}
		}
		pTable = (IMAGE_BASE_RELOCATION*)((PCHAR)pTable + pTable->SizeOfBlock);
	}
	if (isX64(p))
		pNt64->OptionalHeader.ImageBase = newBase;
	else
		pNt32->OptionalHeader.ImageBase = (UINT32)newBase;

	return 0;
}

#if 0
// 重定位测试
int result;

PCHAR pBufferImage;
UINT32 sizeBufferImage;

result = fileBufferToImageBuffer(m_pBuffer, m_bufferSize, &pBufferImage, &sizeBufferImage);
if (result < 0)
	return;

relocateImageBuffer(pBufferImage, 0x600000);

PCHAR pBufferFile;
UINT32 sizeBufferFile;
result = imageBufferToFileBuffer(pBufferImage, sizeBufferImage, &pBufferFile, &sizeBufferFile);
if (result < 0)
	return;

free(pBufferImage);
#endif


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//	文件壳子

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// 加壳
int shellFileBufferApendExeFileBuffer(PCHAR pShellBuffer,UINT32 shellBufferSize ,PCHAR pExeBuffer,UINT32 exeBufferSize,PCHAR* ppNewBufferOut,UINT32* pNewBufferSizeOut)
{
	PE_VAR_DEFINITION;
	int result;
	PCHAR pNewBuffer;
	UINT32 newBufferSize;

	// 加节,用来存放exe文件内容
	result = addSectionInFileBuffer(pShellBuffer,shellBufferSize, (PCHAR)".add", exeBufferSize , 0xE00000E0, &pNewBuffer, &newBufferSize);
	if (result < 0)
		return -1;

	PE_VAR_ASSIGN(pNewBuffer);

	// 将exe文件内容拷贝到新加的节中
	memcpy(pNewBuffer + pSec[pNt32->FileHeader.NumberOfSections - 1].PointerToRawData, pExeBuffer, exeBufferSize);

	// 数据加密
	for (size_t i = 0; i < pSec[pNt32->FileHeader.NumberOfSections - 1].SizeOfRawData/ 4; i++)
		*(UINT32*)(pNewBuffer + pSec[pNt32->FileHeader.NumberOfSections - 1].PointerToRawData + i * sizeof(int)) ^= 0x23333333;

	*ppNewBufferOut = pNewBuffer;
	*pNewBufferSizeOut = newBufferSize;

	return 0;
}

