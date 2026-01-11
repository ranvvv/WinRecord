#pragma once

// 注册GDT和IDT描述符
UINT32 mAddIDTDescriptorx86(UINT64 descriptor);
UINT32 mAddGDTDescriptorx86(UINT64 descriptor);

// 非PAE分页
PUINT32 mGetPDEAddrx86(PVOID addr);
PUINT32 mGetPTEAddrx86(PVOID addr);
int mSetPageRWx86(PVOID addr, int flag);
int mSetPageUSx86(PVOID addr, int flag);
int mSetPageGx86(PVOID addr, int flag);

// PAE分页
PUINT64 mGetPDEAddrPAEx86(PVOID addr);
PUINT64 mGetPTEAddrPAEx86(PVOID addr);
int mSetPageRWPAEx86(PVOID addr, int flag);
int mSetPageUSPAEx86(PVOID addr, int flag);
int mSetPageGPAEx86(PVOID addr, int flag);
int mSetPageNXPAEx86(PVOID addr, int flag);


