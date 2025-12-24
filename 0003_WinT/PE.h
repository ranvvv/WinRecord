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

	UINT64 align(UINT64 num, UINT64 standard);						// 对齐函数
	UINT64 fileAlignment(const char* const p, UINT64 num);			// 文件对齐
	UINT64 sectionAlignment(const char* const p, UINT64 num);		// 节对齐
	UINT32 rvaToFoa(const char* const pBuffer, UINT32 rva);			// rva到foa
	UINT32 foaToRva(const char* const pBuffer, UINT32 foa);			// foa到rva

	// 通过rva 找到buffer中的foa 地址
#define RVA_TO_VA(p,rva) (VOID*)((PCHAR)(p)+rvaToFoa((p),(rva)))


	PCHAR mAllocBuffer(UINT32 size);
	void mFreeBuffer(PCHAR pBuffer);

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	//	查询

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	int IsValidPE(char const* const buffer, size_t bufferSize);		// 判断是否为有效的PE文件

	UINT32 getPEFileSize(const char* const pBuffer);				// 获取PE文件大小
	UINT32 getPEImageSize(const char* const pBuffer);				// 获取PE镜像大小

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	//	修改

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	char* fileBufferToImageBuffer(const char* const pBuffer);		// 文件缓冲区到镜像缓冲区的转换
	char* imageBufferToFileBuffer(const char* const pBuffer);		// 镜像缓冲区到文件缓冲区的转换

#ifdef __cplusplus
}
#endif
