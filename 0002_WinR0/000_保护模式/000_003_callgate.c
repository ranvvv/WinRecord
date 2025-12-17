#include"../common.h"


// 1. 调用门实验: 不提权 无参数 : 配合R3
// 2. 调用门实验: 不提权 3参数 : 配合R3
// 3. 调用门实验: 提权  无参
// 4. 调用门实验: 提权  3参数
// 5. 调用门翻墙返回: 配合R3代码




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 1. R3 调用门实验: 不提权 无参数 : 配合R3

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s1()
{
	UINT64 descriptor;
	UINT32 index;
	UINT64 addr;

	// R3 非一致代码段选择子
	descriptor = 0x00cffb000000ffff;
	index = mAddGDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "R3 非一致代码段选择子: %04X   描述符 : %08X`%08X\n", (index << 3) | 3, (UINT32)(descriptor >> 32), (UINT32)descriptor);


	addr = (UINT64)0x009113D0;	// 根据R3函数地址切换,需要从R3进程中获取

	// 添加一个调用门描述符
	descriptor = 0x0000EC0000000000;
	descriptor |= (UINT64)(index << 3 | 3) << 16;
	descriptor |= addr & 0xFFFF;
	descriptor |= (addr & 0xFFFF0000) << 32;
	index = mAddGDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "R3 调用门 : %04X   描述符 : %08X`%08X\n", (index << 3) | 3, (UINT32)(descriptor >> 32), (UINT32)descriptor);

#if 0
	//测试代码
#include<stdio.h>
#include<windows.h>

	int a;
	static void __declspec(naked) test()
	{
		__asm
		{
			mov eax, 0x11112222
			mov[a], eax
			retf		// 调用门要用retf返回.
		}
	}

	int main()
	{
		char buf[6] = { 0x00,0x00,0x00,0x00,0x6B,0x00 };	// 选择子要根据实际情况修改
		printf("test %p\n", test);	// 输出代码位置,然后根据这个位置 设置调用门选择子对应描述符中的offset
		getchar();

		printf("before call gate %08X\n", a);

		__asm call fword ptr buf;		// 调用门

		printf("end call gate %08X\n", a);

		getchar();

		return 0;
	}
#endif
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 2. 调用门实验: 不提权 3参数 : 配合R3

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s2()
{

	UINT64 descriptor;
	UINT32 index;
	UINT64 addr;

	descriptor = 0x00cffb000000ffff;
	index = mAddGDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "R3 非一致代码段选择子: %04X   描述符 : %08X`%08X\n", (index << 3) | 3, (UINT32)(descriptor >> 32), (UINT32)descriptor);

	addr = (UINT64)0x9413d0;	// 根据R3函数地址切换

	descriptor = 0x0000EC0300000000;
	descriptor |= (UINT64)(index << 3 | 3) << 16;  // 3个参数
	descriptor |= addr & 0xFFFF;
	descriptor |= (addr & 0xFFFF0000) << 32;
	index = mAddGDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "R3 调用门 : %04X   描述符 : %08X`%08X\n", (index << 3) | 3, (UINT32)(descriptor >> 32), (UINT32)descriptor);

#if 0
	//测试代码
#include<stdio.h>

	int a;
	static void __declspec(naked) test()
	{
		__asm
		{
			//fa 3c 41 00 1b 00 00 00 33 33 33 33 22 22 22 22 11 11 11 11    ; 此时的栈结构: eip,cs,param3,param2,param1
			mov eax, 0x11112222
			mov[a], eax
			retf 12		// 返回时, 弹出3个参数
		}
	}

	int main()
	{
		char buf[6] = { 0x00,0x00,0x00,0x00,0x63,0x00 };
		printf("test %p\n", test);
		getchar();

		printf("before call gate %08X\n", a);

		__asm {
			push 0x11111111
			push 0x22222222
			push 0x33333333
			call fword ptr buf;
		}

		printf("end call gate %08X\n", a);

		getchar();

		return 0;
	}
#endif

}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 3. 调用门实验: 提权  无参

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void __declspec(naked) func_call_gate_s3()
{
	__asm {
		// 此时的栈结构: eip,cs,esp,ss
		push 0x30
		pop fs			// 进入R0,fs.base需要指向KPCR

		pushad			// 保存现场
		pushfd

		int 3;
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "call gate run \n");

	__asm {
		popfd			// 恢复现场
		popad
		retf
	}
}

static void s3()
{
	UINT64 descriptor;
	UINT32 index;
	UINT64 addr;

	descriptor = 0x00cf9b000000ffff;
	index = mAddGDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "R0 非一致代码段选择子: %04X   描述符 : %08X`%08X\n", (index << 3) | 3, (UINT32)(descriptor >> 32), (UINT32)descriptor);

	addr = (UINT64)func_call_gate_s3;

	descriptor = 0x0000EC0000000000;
	descriptor |= (UINT64)(index << 3 | 3) << 16;
	descriptor |= addr & 0xFFFF;
	descriptor |= (addr & 0xFFFF0000) << 32;
	index = mAddGDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "R0 调用门 : %04X   描述符 : %08X`%08X\n", (index << 3) | 3, (UINT32)(descriptor >> 32), (UINT32)descriptor);

#if 0
	//测试代码:
#include<stdio.h>

	int main()
	{
		char buf[6] = { 0x00,0x00,0x00,0x00,0x63,0x00 };

		printf("before call gate\n");

		__asm {
			push fs					// 保存原来的fs,由于进内核后会改变fs的值.这里要保存原来的.
			call fword ptr buf;
			pop fs
		}

		printf("end call gate\n");

		getchar();

		return 0;
	}
#endif

}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 4. 调用门实验: 提权  3参数

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static UINT32 param1_s4, param2_s4, param3_s4;

static void __declspec(naked) func_call_gate_s4()
{
	__asm {
		// 此时的栈结构: eip,cs,param3,param2,param1,esp,ss
		push 0x30
		pop fs

		pushad
		pushfd

		//int 3;

		mov eax, [esp + 0x24 + 0x8 + 0x0]		// 参数1,2,3在栈上.此处计算偏移量.
		mov[param1_s4], eax
		mov eax, [esp + 0x24 + 0x8 + 0x4]
		mov[param2_s4], eax
		mov eax, [esp + 0x24 + 0x8 + 0x8]
		mov[param3_s4], eax
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%08X %08X %08X\n", param1_s4, param2_s4, param3_s4);

	__asm {
		popfd
		popad
		retf 12			// 平栈平的是R3栈.R0栈顶不用管它.每次都是从TSS.Esp0取.
	}
}

static void s4()
{
	UINT64 descriptor;
	UINT32 index;
	UINT64 addr;

	descriptor = 0x00cf9b000000ffff;
	index = mAddGDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "R0 非一致代码段选择子: %04X   描述符 : %08X`%08X\n", (index << 3) | 3, (UINT32)(descriptor >> 32), (UINT32)descriptor);

	addr = (UINT64)func_call_gate_s4;
	descriptor = 0x0000EC0300000000;
	descriptor |= (UINT64)(index << 3 | 3) << 16;
	descriptor |= addr & 0xFFFF;
	descriptor |= (addr & 0xFFFF0000) << 32;
	index = mAddGDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "R0 调用门 : %04X   描述符 : %08X`%08X\n", (index << 3) | 3, (UINT32)(descriptor >> 32), (UINT32)descriptor);

#if 0
	//测试代码:

#include<stdio.h>

	int main()
	{
		char buf[6] = { 0x00,0x00,0x00,0x00,0x6b,0x00 };

		printf("before call gate\n");

		__asm {
			push fs
			push 0x11111111
			push 0x22222222
			push 0x33333333
			call fword ptr buf;
			pop fs
		}

		printf("end call gate\n");

		getchar();

		return 0;
	}
#endif
}





// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 5. 调用门翻墙返回: 配合R3代码

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static UINT32 param1_s5, param2_s5, param3_s5;

static void __declspec(naked) func_call_gate_s5()
{
	__asm {
		push 0x30
		pop fs

		pushad
		pushfd

		mov eax, [esp + 0x24 + 0x8 + 0x0]
		mov[param1_s5], eax
		mov eax, [esp + 0x24 + 0x8 + 0x4]
		mov[param2_s5], eax
		mov eax, [esp + 0x24 + 0x8 + 0x8]
		mov[param3_s5], eax
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%08X %08X %08X\n", param1_s5, param2_s5, param3_s5);

	__asm
	{
		mov eax, [param1_s5]
		mov[esp + 0x24], eax		// 修改栈中的返回地址,实现翻墙返回.
	}

	__asm {
		popfd
		popad
		retf 12
	}
}

static void s5()
{

	UINT64 descriptor;
	UINT32 index;
	UINT64 addr;

	descriptor = 0x00cf9b000000ffff;
	index = mAddGDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "R0 非一致代码段选择子: %04X   描述符 : %08X`%08X\n", (index << 3) | 3, (UINT32)(descriptor >> 32), (UINT32)descriptor);

	addr = (UINT64)func_call_gate_s5;
	descriptor = 0x0000EC0300000000;
	descriptor |= (UINT64)(index << 3 | 3) << 16;
	descriptor |= addr & 0xFFFF;
	descriptor |= (addr & 0xFFFF0000) << 32;
	index = mAddGDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "R0 调用门 : %04X   描述符 : %08X`%08X\n", (index << 3) | 3, (UINT32)(descriptor >> 32), (UINT32)descriptor);

#if 0
#include<stdio.h>

	int a;
	int main()
	{
		char buf[6] = { 0x00,0x00,0x00,0x00,0x6b,0x00 };

		printf("before call gate\n");

		__asm {
			push fs
			push 0x11111111
			push 0x22222222
			push t1				// 作为新的返回地址.
			call fword ptr buf;
			pop fs
				mov eax, 0x55555555
				mov[a], eax
				jmp end
				t1 :
			pop fs
				mov eax, 0x66666666
				mov[a], eax
				end :
			mov eax, eax
		}

		printf("a : %08X\n", a);		// 0x66666666, 说明调用门返回后, 执行了t1标签处的代码.

		printf("end call gate\n");

		getchar();

		return 0;
	}
#endif
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_003()
{
	s5();
}

