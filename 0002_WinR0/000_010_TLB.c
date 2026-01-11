#include"common.h"




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 1. TLB 使用测试

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$



static UINT32 param1_s1, param2_s1, param3_s1;
static PUINT64 pteA, pteB, pdeA, pdeB;
static UINT64  pteABak;
static int a, b;

static void __declspec(naked) func_call_gate_s1()
{
	__asm {
		push 0x30
		pop fs

		pushad
		pushfd

		//int 3;

		mov eax, [esp + 0x24 + 0x8 + 0x0]		// 参数1,2,3在栈上.此处计算偏移量.
		mov[param1_s1], eax
		mov eax, [esp + 0x24 + 0x8 + 0x4]
		mov[param2_s1], eax
		mov eax, [esp + 0x24 + 0x8 + 0x8]
		mov[param3_s1], eax

	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,	"param1:%08X param2:%08X param3:%08X\n", param1_s1, param2_s1, param3_s1);

	*(UINT32*)param1_s1 = 0x11111111;
	*(UINT32*)param2_s1 = 0x22222222;

	pteA = mGetPTEAddrPAEx86((PVOID)param1_s1);
	pteB = mGetPTEAddrPAEx86((PVOID)param2_s1);

#if 0
	// 证实tlb的存在.

	pteABak = *(UINT64*)0xC0000000;
	*((UINT64*)0xC0000000) = *pteA;		// 给0地址挂载一个pte.
	a = *((UINT32*)0x00000000);
	*((UINT64*)0xC0000000) = *pteB;		// 修改0地址的PTE
	b = *((UINT32*)0x00000000);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,	"param1:%08X param2:%08X\n", a, b);
	// 0x11111111,0x11111111 ,值是一样的,说明后修改的未生效.读的还是头一次挂的.
	*((UINT64*)0xC0000000) = pteABak;	// 恢复原来的PTE.
#endif

#if 0
	// 刷新TLB测试.
	pteABak = *(UINT64*)0xC0000000;
	*((UINT64*)0xC0000000) = *pteA;
	a = *((UINT32*)0x00000000);
	__asm
	{
		mov eax, cr3
		mov cr3, eax // 刷新TLB
	}
	*((UINT64*)0xC0000000) = *pteB;
	b = *((UINT32*)0x00000000);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,	"param1:%08X param2:%08X\n", a, b); 
	// 0x11111111,22222222   说明刷新了TLB.
	*((UINT64*)0xC0000000) = pteABak;	// 恢复原来的PTE.
#endif


#if 0
	// 全局页不刷新TLB测试.
	pteABak = *(UINT64*)0xC0000000;
	*((UINT64*)0xC0000000) = *pteA | 0x100; // G位置1.此项在TLB中不刷新.
	a = *((UINT32*)0x00000000);
	__asm
	{
		mov eax, cr3
		mov cr3, eax // 刷新TLB
	}
	*((UINT64*)0xC0000000) = *pteB;
	b = *((UINT32*)0x00000000);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "param1:%08X param2:%08X\n", a, b); 
	// 0x11111111,0x11111111  说明切换CR3没能更新这个地址的TLB.
	*(UINT64*)0xC0000000 = pteABak;	// 恢复原来的PTE.
#endif


#if 01
	// 强制刷新TLB测试.
	pteABak = *(UINT64*)0xC0000000;
	*((UINT64*)0xC0000000) = *pteA | 0x100;
	a = *((UINT32*)0x00000000);
	__asm
	{
		invlpg dword ptr ds : [0x0]		// 强制刷新0地址的TLB
	}
	*((UINT64*)0xC0000000) = *pteB;
	b = *((UINT32*)0x00000000);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "param1:%08X param2:%08X\n", a, b); 
	// 0x11111111,0x22222222   说明强制刷新了TLB. 不受G位影响.
	*(UINT64*)0xC0000000 = pteABak;	// 恢复原来的PTE.
#endif

	__asm {
		popfd
		popad
		retf 12			// 平栈平的是R3栈.R0栈顶不用管它.每次都是从TSS.Esp0取.
	}
}

static void s1()
{
	UINT64 descriptor;
	UINT32 index;
	UINT64 addr;

	index = 0x1;
	addr = (UINT64)func_call_gate_s1;
	descriptor = 0x0000EC0300000000;
	descriptor |= (UINT64)(index << 3 | 3) << 16;
	descriptor |= addr & 0xFFFF;
	descriptor |= (addr & 0xFFFF0000) << 32;
	index = mAddGDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "R0 3参数调用门 : %04X   描述符 : %08X`%08X\n", (index << 3) | 3 , (UINT32)(descriptor >> 32), (UINT32)descriptor);

#if 0
#include<Windows.h>
#include<stdio.h>

	int main()
	{

		void* pa = VirtualAlloc(NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		void* pb = VirtualAlloc(NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		printf("%08X %08X\n", *(int*)pa, *(int*)pb);
		char buf[6] = { 0,0,0,0,0x4b,0 };

		__asm {
			push fs
			push 0x0
			push pb
			push pa
			call fword ptr[buf]
			pop fs
		}

		printf("end\n");
		system("pause");
		return 0;
	}
#endif

}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_010()
{
	s1();
}

