#include"common.h"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 1. PDE与PTE

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

PUINT32 mGetPDEAddrx86(PVOID addr)
{
	return (PUINT32)(0xC0300000 + (((UINT32)addr >> 22) << 2));
}

PUINT32 mGetPTEAddrx86(PVOID addr)
{
	return (PUINT32)(0xC0000000 + (((UINT32)addr >> 12) << 2));
}

int mSetPageRWx86(PVOID addr, int flag)
{
	PUINT32 pPde = mGetPDEAddrx86(addr);
	if (!(*pPde & 0x1))
		return -1;

	if (*pPde  & 0x80 ) // PS位,大页
	{
		if (flag)
			*pPde |= 0x2;
		else
			*pPde &= ~0x2;
		return 0;
	}

	PUINT32 pPte = mGetPTEAddrx86(addr);
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

int mSetPageUSx86(PVOID addr, int flag)
{
	PUINT32 pPde = mGetPDEAddrx86(addr);
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

	PUINT32 pPte = mGetPTEAddrx86(addr);
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

int mSetPageGx86(PVOID addr, int flag)
{
	PUINT32 pPde = mGetPDEAddrx86(addr);
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

	PUINT32 pPte = mGetPTEAddrx86(addr);
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


static void s1()
{
	static int val_s1;

	val_s1 = 1;
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"VirtualAddress : %p\n", &val_s1);
	PUINT32 pPde = mGetPDEAddrx86(&val_s1);
	PUINT32 pPte = mGetPTEAddrx86(&val_s1);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "PPDE : %p , PPTE : %p\n", pPde, pPte);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "PPDE : %08x , PPTE : %08x\n", *pPde, *pPte);

	//mSetPageUSx86(&val_s1, 1);

	mSetPageRWx86(&val_s1, 0);
	__asm {
		mov eax, cr3
		mov cr3, eax
	}
	pPde = mGetPDEAddrx86(&val_s1);
	pPte = mGetPTEAddrx86(&val_s1);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "PPDE : %p , PPTE : %p\n", pPde, pPte);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "PPDE : %08x , PPTE : %08x\n", *pPde, *pPte);

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

	PUINT32 pdeA, pdeB;
	PUINT32 pteA, pteB;
	UINT32  pteABak;

	pdeA = mGetPDEAddrx86(pageA);
	pteA = mGetPTEAddrx86(pageA);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "A addr pde:%08X pte:%08X\n", *pdeA, *pteA);

	pdeB = mGetPDEAddrx86(pageB);
	pteB = mGetPTEAddrx86(pageB);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "B addr pde:%08X pte:%08X\n", *pdeB, *pteB);

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


void p000_007()
{
	s1();
}
