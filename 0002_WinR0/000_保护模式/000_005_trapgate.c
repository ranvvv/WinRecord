#include"../common.h"

// 1. 陷阱门实现retf返回. 配合R3代码


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 1. 陷阱门实现retf返回. 配合R3代码

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void __declspec(naked) func_interrupt_gate_s1()
{
	__asm
	{
		push 0x30
		pop fs

		pushad
		pushfd

		mov eax, eax
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "trap gate run with retf to return \n");

	__asm
	{
		popfd
		popad

		push[esp + 8]
		popfd

		mov eax, [esp + 0xC]
		mov[esp + 0x8], eax
		mov eax, [esp + 0x10]
		mov[esp + 0xc], eax
		retf				// retf不会解除CPU内部的阻塞标记.但由于是陷阱门不阻塞中断.所以没事
	}
}

static void s1()
{
	UINT32 index;
	UINT64 descriptor;
	UINT64 addr;

	addr = (UINT64)func_interrupt_gate_s1;
	descriptor = 0x0000EF0000080000;
	descriptor |= (addr & 0xFFFF);
	descriptor |= ((addr >> 16) << 48);
	index = mAddIDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "index : 0x%x   descriptor : %08X`%08X\n", index, (UINT32)(descriptor >> 32), (UINT32)descriptor);

#if 0
	// 测试代码
#include<stdio.h>
#include<Windows.h>

	int main()
	{
		__asm {
			push fs
			int 0x20;
			pop fs
		}
		system("pause");
		return 0;
	}
#endif
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void p000_005()
{
	s1();
}
