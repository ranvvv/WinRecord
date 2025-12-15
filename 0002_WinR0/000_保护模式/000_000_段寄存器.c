#include "../common.h"


// 1. ¶Î¼Ä´æÆ÷ÊôÐÔÌ½²â
// 2. ¶ÁÐ´¶Î¼Ä´æÆ÷




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 1. ¶Î¼Ä´æÆ÷ÊôÐÔÌ½²â

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#if 0
static void s3R3()
{
	int var;

	// ------ attribute
	__asm
	{
		// ss¿É¶Á,¿ÉÐ´
		mov ax, ss
		mov ds, ax
		mov dword ptr ds : [var] , 123456

		// cs¿É¶Á,²»¿ÉÐ´
		mov ax, cs
		mov ds, ax
		mov ebx, dword ptr ds : [var]
		mov dword ptr ds : [var] , 12345678h; ÒòÎªCSÊÇÖ»¶ÁµÄ, ËùÒÔÕâ¾ä»á³ö´í
	}

	// ------ base
	__asm {
		// 0µØÖ·²»¿É¶ÁÐ´.
		// mov al,byte ptr ds:[0x0]			

		mov ax, fs
		mov es, ax
		mov eax, dword ptr es : [0x0]			// ¿É¶ÁÐ´
		mov ebx, dword ptr ds : [0x7ffdf000]	// eax == ebx , so fs.base == 0x7FFDF000
	}

	// ------ limit
	__asm {
		mov ax, fs
		mov es, ax
		mov al, byte ptr es : [0FFFh]			// ¿É¶ÁÐ´
		mov al, byte ptr es : [1000h]			// ³¬¹ýlimit,²»¿É¶ÁÐ´
		mov eax, eax
	}
}
#endif

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 2. ¶ÁÐ´¶Î¼Ä´æÆ÷

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s2()
{
	// ¶Á¶Î¼Ä´æÆ÷
	short SegEs, SegCs, SegSs, SegDs, SegFs, SegGs, SegLdtr, SegTr;
	__asm
	{
		// Í¨ÓÃÖ¸Áî
		mov ax, es
		mov[SegEs], ax
		mov ax, cs
		mov[SegCs], ax
		mov ax, ss
		mov[SegSs], ax
		mov ax, Ds
		mov[SegDs], ax
		mov ax, Fs
		mov[SegFs], ax
		mov ax, gs
		mov[SegGs], ax

		// ×¨ÓÃÖ¸Áî
		sldt ax;
		mov[SegLdtr], ax
			str ax;
		mov[SegTr], ax
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "CPU ¶Î¼Ä´æÆ÷: es:%04X cs:%04X ss:%04X ds:%04X fs:%04X gs:%04X ldtr:%04X tr:%04X\n",
		SegEs, SegCs, SegSs, SegDs, SegFs, SegGs, SegLdtr, SegTr);


	// Ð´¶Î¼Ä´æÆ÷
	char tmp[6] = { 0 };
	UINT32 var_1;
	short SegGs_1, SegGs_2;
	__asm
	{
		// movÐ´¶Î¼Ä´æÆ÷
		mov ax, ds
		mov gs, ax
		mov bx, gs
		mov[SegGs_1], bx

		// lgsÐ´¶Î¼Ä´æÆ÷
		mov eax, 11111111h
		mov dword ptr[tmp], eax
		mov ax, fs
		mov word ptr[tmp + 4], ax
		mov eax, 0
		lgs eax, fword ptr[tmp]		// Í¬Ê±ÐÞ¸ÄeaxºÍgs,word ptr [tmp+4]Ð´gs¼Ä´æÆ÷,dword ptr [tmp]Ð´eax¼Ä´æÆ÷
		mov dword ptr[var_1], eax
		mov ax, gs
		mov[SegGs_2], ax
	}
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "gs_1:%04X gs_2 :%04X var_1 :%08X\n", SegGs_1, SegGs_2, var_1);
}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_000()
{
}




