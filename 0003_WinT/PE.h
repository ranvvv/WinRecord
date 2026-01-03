#pragma once

#ifndef _KERNEL_MODE
#include<windows.h>		// R3 
#else
#include<ntddk.h>		// R0 
#include<ntimage.h>		// 包含_IMAGE_DOS_HEADER, _IMAGE_NT_HEADERS 等结构体
#endif



#ifdef __cplusplus
extern "C" {
#endif

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	//	PE 宏定义

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	// 基础宏
#define PDOS(addr)  ((PIMAGE_DOS_HEADER)(addr))
#define PNT32(addr) ((PIMAGE_NT_HEADERS32)((PCHAR)(addr) + PDOS(addr)->e_lfanew))
#define PNT64(addr) ((PIMAGE_NT_HEADERS64)PNT32(addr))
#define isX64(addr) (PNT32(addr)->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)

// PE数据定义
#define PE_VAR_DEFINITION \
	PCHAR p=NULL;\
	PIMAGE_DOS_HEADER pDos=NULL;\
	PIMAGE_NT_HEADERS32 pNt32=NULL;\
	PIMAGE_NT_HEADERS64 pNt64=NULL;\
	PIMAGE_SECTION_HEADER pSec=NULL;\
	PIMAGE_DATA_DIRECTORY pDir=NULL;

// PE数据解析
#define PE_VAR_ASSIGN(BUFFER) \
	p = (PCHAR)BUFFER;\
	pDos = PDOS(p);\
	pNt32 = PNT32(p);\
	pNt64 = PNT64(p);\
	pSec = IMAGE_FIRST_SECTION(PNT32(p));\
	pDir = isX64(p) ? pNt64->OptionalHeader.DataDirectory : pNt32->OptionalHeader.DataDirectory;


// PE新数据定义
#define NEW_PE_VAR_DEFINITION \
	PCHAR pNew=NULL;\
	PIMAGE_DOS_HEADER pDosNew=NULL ;\
	PIMAGE_NT_HEADERS32 pNt32New=NULL;\
	PIMAGE_NT_HEADERS64 pNt64New=NULL;\
	PIMAGE_SECTION_HEADER pSecNew=NULL;\
	PIMAGE_DATA_DIRECTORY pDirNew=NULL;

// PE新数据解析
#define NEW_PE_VAR_ASSIGN(BUFFER) \
	pNew = (PCHAR)BUFFER;\
	pDosNew = PDOS(pNew);\
	pNt32New = PNT32(pNew);\
	pNt64New = PNT64(pNew);\
	pSecNew = IMAGE_FIRST_SECTION(PNT32(pNew));\
	pDirNew = isX64(pNew) ? pNt64New->OptionalHeader.DataDirectory : pNt32New->OptionalHeader.DataDirectory;

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	//	功能函数

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	UINT64 align(UINT64 num, UINT64 standard);			// 对齐函数
	UINT64 fileAlignment(PCHAR p, UINT64 num);			// 文件对齐
	UINT64 sectionAlignment(PCHAR p, UINT64 num);		// 节对齐
	UINT32 rvaToFoa(PCHAR pBuffer, UINT32 rva);			// rva到foa
	UINT32 foaToRva(PCHAR pBuffer, UINT32 foa);			// foa到rva

	// 通过rva 找到buffer中的foa 地址
	#define RVA_TO_FILE_BUFFER_VA(p,rva) (VOID*)((PCHAR)(p)+rvaToFoa((p),((UINT32)rva)))


	PCHAR mAllocBuffer(UINT32 size);					// 分配缓冲区
	VOID mFreeBuffer(PCHAR pBuffer);					// 释放缓冲区

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	//	查询

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	int IsValidPE(PCHAR pBuffer, UINT32 bufferSize, UINT32 isMemImage);		// 判断是否为有效的PE文件
	UINT32 getPEFileSize(PCHAR pBuffer);									// 获取PE文件大小
	UINT32 getPEImageSize(PCHAR pBuffer);									// 获取PE镜像大小

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	//	修改

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	int fileBufferToImageBuffer(PCHAR pBuffer, UINT32 bufferSize, PCHAR* pBufferNewOut, UINT32* pSizeOut);	// 文件缓冲区到镜像缓冲区的转换
	int imageBufferToFileBuffer(PCHAR pBuffer, UINT32 bufferSize, PCHAR* pBufferNewOut, UINT32* pSizeOut);	// 镜像缓冲区到文件缓冲区的转换

	int addSectionInFileBuffer(PCHAR pBuffer, UINT32 bufferSize, PCHAR name, UINT32 size, UINT32 charac, 
		PCHAR* pBufferNewOut, UINT32* pSizeNewOut);															// 添加节
	int modifySectionInFileBuffer(PCHAR pBuffer, UINT32 bufferSize, UINT32 index, PCHAR name, UINT32 size,
		UINT32 charac, PCHAR* pBufferNewOut, UINT32* pSizeNewOut);											// 修改节
	int mergeSectionInFileBuffer(PCHAR pBuffer, UINT32 bufferSize, UINT16 index, PCHAR* pBufferNewOut, 
		UINT32* pSizeNewOut);																				// 合并节

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	//	导出表

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	// 获取导出项 RVA
	UINT32 getExportItemRvaByNameInFileBuffer(PCHAR pBuffer, PCHAR name);
	UINT32 getExportItemRvaByNameInMemBuffer(PCHAR pBuffer, PCHAR name);
	UINT32 getExportItemRvaByNumberInFileBuffer(PCHAR pBuffer, UINT32 number);
	UINT32 getExportItemRvaByNumberInMemBuffer(PCHAR pBuffer, UINT32 number);
	//	导出表大小
	typedef struct _EXPORT_TABLE_SIZE
	{
		UINT32 nameSize;
		UINT32 tableSize;
		UINT32 arrFuncRvaSize;
		UINT32 arrNameRvaSize;
		UINT32 arrOrdinalSize;
	}EXPORT_TABLE_SIZE;
	UINT32 getExportTableSizeInFileBuffer(PCHAR pBuffer, EXPORT_TABLE_SIZE* pExportTableSize);
	int moveExportTableInFileBuffer(PCHAR pBuffer, UINT32 begin);


	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	//	导入表

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	// 导入表大小
	typedef struct _IMPORT_TABLE_SIZE
	{
		UINT32 nameSize;
		UINT32 importTableSize;
		UINT32 INTSize;
	}IMPORT_TABLE_SIZE;

	UINT32 getImportTableSizeInFileBuffer(PCHAR pBuffer, IMPORT_TABLE_SIZE* pImportTableSize);
	int moveImportTableInFileBuffer(PCHAR pBuffer, UINT32 begin);

	int importTableInjectionByNameInFileBuffer(PCHAR pBuffer, UINT32 bufferSize, PCHAR dllName, PCHAR funcName, PCHAR* ppNewBuffer, UINT32* pSizeOut);
	int importTableInjectionByNumberInFileBuffer(PCHAR pBuffer, UINT32 bufferSize, PCHAR dllName, UINT32 number, PCHAR* ppNewBuffer, UINT32* pSizeOut);

#ifndef _KERNEL_MODE
	int fixImportIATInImageBuffer(PCHAR pBuffer);
#endif

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	//	重定位表

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	INT32 relocateImageBuffer(PCHAR pBuffer, UINT64 newBase);


#ifdef __cplusplus
}
#endif
