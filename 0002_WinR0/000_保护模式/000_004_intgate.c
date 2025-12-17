#include"../common.h"


// 1. 中断门提权
// 2. 中断门实现retf返回. 配合R3代码




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 1. 中断门提权

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void __declspec(naked) func_interrupt_gate_s1()
{
	__asm
	{
		push 0x30
		pop fs

		pushad
		pushfd

		// int 3;
		mov eax, eax
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "interrupt gate run\n");

	__asm
	{
		popfd
		popad
		iretd
	}
}

static void s1()
{
	UINT32 index;
	UINT64 descriptor;
	UINT64 addr;

	addr = (UINT64)func_interrupt_gate_s1;
	descriptor = 0x0000EE0000080000;
	descriptor |= (addr & 0xFFFF);
	descriptor |= ((addr >> 16) << 48);
	index = mAddIDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "index : 0x%x   descriptor : %08X`%08X\n", index, (UINT32)(descriptor >> 32), (UINT32)descriptor);


#if 0
	//测试代码
#include<stdio.h>

	int main()
	{
		printf("before int gate\n");

		__asm {
			push fs
			int 0x20;
			pop fs
		}

		printf("end int gate\n");

		getchar();

		return 0;
	}
#endif
}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 2. 中断门实现retf返回. 配合R3代码

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void __declspec(naked) func_interrupt_gate_s2()
{
	__asm
	{
		push 0x30
		pop fs

		pushad
		pushfd

		mov eax, eax
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "interrupt gate run with retf to return \n");

	__asm
	{
		popfd
		popad

		sti					// 解除iretd的阻塞标记.
		// iretd 会解除CPU内部的一个阻塞中断标记.  中断返回.
		// retf实现iretd功能.蓝屏的真正原因就是中断门的CPU内部的阻塞.  修复了就不会蓝

		push[esp + 8]		// 修复eflags
		popfd

		mov eax, [esp + 0xC]
		mov[esp + 0x8], eax
		mov eax, [esp + 0x10]
		mov[esp + 0xc], eax
		retf				// retf不会解除CPU内部的阻塞标记.
	}
}

static void s2()
{
	UINT32 index;
	UINT64 descriptor;
	UINT64 addr;

	addr = (UINT64)func_interrupt_gate_s2;
	descriptor = 0x0000EE0000080000;
	descriptor |= (addr & 0xFFFF);
	descriptor |= ((addr >> 16) << 48);
	index = mAddIDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "index : 0x%x   descriptor : %08X`%08X\n", index, (UINT32)(descriptor >> 32), (UINT32)descriptor);

#if 0
	//测试代码
#include<stdio.h>

	int main()
	{
		__asm {
			push fs
			int 0x20;
			pop fs
		}

		printf("end int gate\n");  // 蓝屏.位置.

		getchar();

		return 0;
	}
#endif
}






// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_004()
{
	s1();
}
