#include "common.h"




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 1. 如何判断是否PAE分页

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

UINT32 __stdcall mIsPAE()
{
	UINT32 b = 0;
	__asm {
		// mov eax,cr4  无法直接写指令,要用硬编码
		__emit 0x0f
		__emit 0x20
		__emit 0xe0
		and eax, 0x20;  // CR4.PAE位
		mov dword ptr[b], eax
	}
	return b;
}

static void s1()
{
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "mIsPAEx86 : %d\n", mIsPAE());
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 2. WP 位的开启与关闭

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

 // 关闭wp位
void __declspec(naked)  wp_off()
{
	__asm {
		cli;				// 禁用中断
		mov eax, cr0;
		and eax, ~0x10000;	// 清除 WP 位
		mov cr0, eax;
		ret
	}
}

// 开启wp位
void __declspec(naked)  wp_on()
{
	__asm {
		mov eax, cr0
		or eax, 0x10000;// 设置 WP 位
		mov cr0, eax;
		sti; // 启用中断
		ret;
	}
}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 3. 开放高2G地址,让R3可以读写	: 配合R3

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static UINT32 r0_data_s3 = 0x11111111;

static void __declspec(naked) func_interrupt_gate_s3()
{
	__asm
	{
		push 0x30
		pop fs

		pushad
		pushfd
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "r0_data_s3  %p :%08X\n", &r0_data_s3, r0_data_s3);

	if (mIsPAE())
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pde : %016I64X\n", *(UINT64*)mGetPDEAddrPAEx86(&r0_data_s3));
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pde : %016I64X\n", *(UINT64*)mGetPTEAddrPAEx86(&r0_data_s3));

		mSetPageRWPAEx86(&r0_data_s3, 1);
		mSetPageUSPAEx86(&r0_data_s3, 1);
		//mSetPageExecuteFlag(&r0_data_s3, 1);
		__invlpg(&r0_data_s3);	// 刷新TLB
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pde : %016I64X\n", *(UINT64*)mGetPDEAddrPAEx86(&r0_data_s3));
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pde : %016I64X\n", *(UINT64*)mGetPTEAddrPAEx86(&r0_data_s3));
	}
	else
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pde : %08X\n", *(UINT32*)mGetPDEAddrx86(&r0_data_s3));
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pte : %08X\n", *(UINT32*)mGetPTEAddrx86(&r0_data_s3));
		mSetPageRWx86(&r0_data_s3,0);
		mSetPageUSx86(&r0_data_s3,1);
		__invlpg(&r0_data_s3);						// 刷新TLB
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pde : %08X\n", *(UINT32*)mGetPDEAddrx86(&r0_data_s3));
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pte : %08X\n", *(UINT32*)mGetPTEAddrx86(&r0_data_s3));
	}

	__asm
	{
		popfd
		popad

		mov eax, offset r0_data_s3

		iretd
	}
}

static void s3()
{
	UINT32 index;
	UINT64 descriptor;
	UINT64 addr;

	addr = (UINT64)func_interrupt_gate_s3;
	descriptor = 0x0000EE0000080000;
	descriptor |= (addr & 0xFFFF);
	descriptor |= ((addr >> 16) << 48);
	index = mAddIDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "index : 0x%x   descriptor : %08X`%08X\n", index, (UINT32)(descriptor >> 32), (UINT32)descriptor);

#if 0
	// R3 测试代码
#include<stdio.h>
#include<Windows.h>


	int main()
	{
		int* a;
		__asm
		{
			push fs
			int 0x20
			pop fs
			mov[a], eax  // 获取地址,在中断门中将其设置位R3可访问.
		}
		printf("%p %x\n", a, *a); // 这里可以直接读高2G了.
		system("pause");
		return 0;
	}
#endif
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 4. 开放高2G代码位置,让R3可以直接调用.

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void __declspec(naked) func_test_s4()
{
	__asm {
		mov eax, 0x22221111
		ret
	}
}

static void __declspec(naked) func_interrupt_gate_s4()
{
	__asm
	{
		push 0x30
		pop fs

		pushad
		pushfd
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "interrupt gate run  data addr : %08X\n", (UINT32)func_test_s4);

	if (mIsPAE())
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pde : %016I64X\n", *(UINT64*)mGetPDEAddrPAEx86((PVOID)func_test_s4));
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pte : %016I64X\n", *(UINT64*)mGetPTEAddrPAEx86((PVOID)func_test_s4));
		mSetPageUSPAEx86((PVOID)func_test_s4, 1);
		__invlpg((PVOID)func_test_s4);	// 刷新TLB
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d\n", *(char*)func_test_s4); // 取个值
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pde : %016I64X\n", *(UINT64*)mGetPDEAddrPAEx86((PVOID)func_test_s4));
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pte : %016I64X\n", *(UINT64*)mGetPTEAddrPAEx86((PVOID)func_test_s4));
	}
	else
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pde : %08X\n", *(UINT32*)mGetPDEAddrx86((PVOID)func_test_s4));
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pte : %08X\n", *(UINT32*)mGetPTEAddrx86((PVOID)func_test_s4));
		mSetPageUSx86((PVOID)func_test_s4, 1);
		__invlpg((PVOID)func_test_s4);						// 刷新TLB
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d\n", *(char*)func_test_s4);
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pde : %08X\n", *(UINT32*)mGetPDEAddrx86((PVOID)func_test_s4));
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pte : %08X\n", *(UINT32*)mGetPTEAddrx86((PVOID)func_test_s4));
	}

	__asm
	{
		popfd
		popad
		mov eax, offset func_test_s4
		iretd
	}
}

static void s4()
{
	UINT32 index;
	UINT64 descriptor;
	UINT64 addr;

	addr = (UINT64)func_interrupt_gate_s4;
	descriptor = 0x0000EE0000080000;
	descriptor |= (addr & 0xFFFF);
	descriptor |= ((addr >> 16) << 48);
	index = mAddIDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "index : 0x%x   descriptor : %08X`%08X\n", index, (UINT32)(descriptor >> 32), (UINT32)descriptor);

#if 0
	// R3测试
#include<stdio.h>
#include<Windows.h>


	int main()
	{
		int* a;
		__asm
		{
			push fs
			int 0x20
			pop fs
			mov[a], eax
		}
		printf("%p %x\n", a, *a);

		int data;
		__asm {
			mov eax, [a]
			call eax
			mov[data], eax
		}
		printf("%x", data);

		system("pause");
		return 0;
	}
#endif
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_009()
{
	s4();
}