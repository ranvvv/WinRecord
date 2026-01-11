#include"common.h"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 1. PDE与PTE

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

PUINT64 mGetPDEAddrPAEx86(PVOID addr)
{
	return (PUINT64)(0xC0600000 + (((UINT32)addr >> 21) << 3));
}

PUINT64 mGetPTEAddrPAEx86(PVOID addr)
{
	return (PUINT64)(0xC0000000 + (((UINT32)addr >> 12) << 3));
}

int mSetPageRWPAEx86(PVOID addr, int flag)
{
	PUINT64 pPde = mGetPDEAddrPAEx86(addr);
	if (!(*pPde & 0x1))
		return -1;

	if (*pPde & 0x80) // PS位,大页
	{
		if (flag)
			*pPde |= 0x2;
		else
			*pPde &= ~0x2;
		return 0;
	}

	PUINT64 pPte = mGetPTEAddrPAEx86(addr);
	if (!(*pPte & 0x1))
		return -2;

	if (flag)
	{
		*pPde |= 0x2;
		*pPte |= 0x2;
	}
	else
	{
		*pPde &= ~0x2;
		*pPte &= ~0x2;
	}

	return 0;
}

int mSetPageUSPAEx86(PVOID addr, int flag)
{
	PUINT64 pPde = mGetPDEAddrPAEx86(addr);
	if (!(*pPde & 0x1))
		return -1;

	if (*pPde & 0x80) // PS位,大页
	{
		if (flag)
			*pPde |= 0x4;
		else
			*pPde &= ~0x4;
		return 0;
	}

	PUINT64 pPte = mGetPTEAddrPAEx86(addr);
	if (!(*pPte & 0x1))
		return -2;

	if (flag)
	{
		*pPde |= 0x4;
		*pPte |= 0x4;
	}
	else
	{
		*pPde &= ~0x4;
		*pPte &= ~0x4;
	}

	return 0;
}

int mSetPageGPAEx86(PVOID addr, int flag)
{
	PUINT64 pPde = mGetPDEAddrPAEx86(addr);
	if (!(*pPde & 0x1))
		return -1;

	if (*pPde & 0x80) // PS位,大页
	{
		if (flag)
			*pPde |= 0x100;
		else
			*pPde &= ~0x100;
		return 0;
	}

	PUINT64 pPte = mGetPTEAddrPAEx86(addr);
	if (!(*pPte & 0x1))
		return -2;

	if (flag)
	{
		*pPde |= 0x100;
		*pPte |= 0x100;
	}
	else
	{
		*pPde &= ~0x100;
		*pPte &= ~0x100;
	}

	return 0;
}

// 可能导致死机,原因未知
int mSetPageNXPAEx86(PVOID addr, int flag)
{
	PUINT64 pPde = mGetPDEAddrPAEx86(addr);
	if (!(*pPde & 0x1))
		return -1;

	if (*pPde & 0x80) // PS位,大页
	{
		if (flag)
			*pPde |= (1LL << 63LL);
		else
			*pPde &= ~(1LL << 63LL);
		return 0;
	}

	PUINT64 pPte = mGetPTEAddrPAEx86(addr);
	if (!(*pPte & 0x1))
		return -2;

	if (flag)
	{
		*pPde |= (1LL << 63LL);
		*pPte |= (1LL << 63LL);
	}
	else
	{
		*pPde &= ~(1LL << 63LL);
		*pPte &= ~(1LL << 63LL);
	}

	return 0;
}

static void s1()
{
	static UINT32 val_s1 = 0x12345678;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "VirtualAddress : %08X\n", &val_s1);

	PUINT64 pPde = mGetPDEAddrPAEx86(&val_s1);
	PUINT64 pPte = mGetPTEAddrPAEx86(&val_s1);

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " PDE : %p PTE : %p\n", pPde, pPte);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " PDE : %016llX PTE : %016llX\n", *pPde, *pPte);

	mSetPageRWPAEx86(&val_s1, 0);
	mSetPageNXPAEx86(&val_s1, 1);
	__asm {
		mov eax,cr3
		mov cr3,eax
	}
	pPde = mGetPDEAddrPAEx86(&val_s1);
	pPte = mGetPTEAddrPAEx86(&val_s1);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " PDE : %p PTE : %p\n", pPde, pPte);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " PDE : %016llX PTE : %016llX\n", *pPde, *pPte);

	// # !pte <VirtualAddress>
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 2. 替换物理页.

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s2()
{
	VOID* pageA = ExAllocatePoolWithTag(NonPagedPool, 0x1000, 'A');
	if (!pageA)
		return;
	VOID* pageB = ExAllocatePoolWithTag(NonPagedPool, 0x1000, 'B');
	if (!pageB)
		return;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pageA : %p\n", pageA);

	*(UINT32*)pageA = 0x11111111;
	*(UINT32*)pageB = 0x22222222;

	PUINT64 pdeA, pdeB;
	PUINT64 pteA, pteB;
	UINT64  pteABak;

	pdeA = mGetPDEAddrPAEx86(pageA);
	pteA = mGetPTEAddrPAEx86(pageA);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "A addr pde:%016llX pte:%016llX\n", *pdeA, *pteA);


	pdeB = mGetPDEAddrPAEx86(pageB);
	pteB = mGetPTEAddrPAEx86(pageB);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "B addr pde:%016llX pte:%016llX\n", *pdeB, *pteB);

	pteABak = *pteA;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "替换前 : A val %08X B val %08X\n", *(UINT32*)pageA, *(UINT32*)pageB);

	// 替换物理页.
	*pteA = *pteB;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "替换后 : A val %08X B val %08X\n", *(UINT32*)pageA, *(UINT32*)pageB);

	*pteA = pteABak;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "恢复后 : A val %08X B val %08X\n", *(UINT32*)pageA, *(UINT32*)pageB);
}





// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_008()
{
	s2();
}
