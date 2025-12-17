#include"../common.h"


// 1. 打印TSS结构
// 2. 通过调用门构建R3任务段,实现任务切换  


typedef struct _KTSS32 {
    /* 0x000 */ UINT16 Backlink;         // 上一个TSS的选择子（任务切换时使用）
    /* 0x002 */ UINT16 Reserved0;        // 保留字段
    /* 0x004 */ UINT32 Esp0;             // 特权级0的栈指针（内核模式栈）
    /* 0x008 */ UINT16 Ss0;              // 特权级0的栈段选择子
    /* 0x00a */ UINT16 Reserved1;        // 保留字段
    /* 0x00c */ UINT32 NotUsed1[4];      // 未使用区域（保留字段）
    /* 0x01c */ UINT32 CR3;              // 页目录基址寄存器（控制地址空间）
    /* 0x020 */ UINT32 Eip;              // 指令指针寄存器
    /* 0x024 */ UINT32 EFlags;           // 标志寄存器
    /* 0x028 */ UINT32 Eax;              // 通用寄存器
    /* 0x02c */ UINT32 Ecx;              // 通用寄存器
    /* 0x030 */ UINT32 Edx;              // 通用寄存器
    /* 0x034 */ UINT32 Ebx;              // 通用寄存器
    /* 0x038 */ UINT32 Esp;              // 栈指针寄存器
    /* 0x03c */ UINT32 Ebp;              // 基址指针寄存器
    /* 0x040 */ UINT32 Esi;              // 源变址寄存器
    /* 0x044 */ UINT32 Edi;              // 目的变址寄存器
    /* 0x048 */ UINT16 Es;               // 附加段寄存器
    /* 0x04a */ UINT16 Reserved2;        // 保留字段
    /* 0x04c */ UINT16 Cs;               // 代码段寄存器
    /* 0x04e */ UINT16 Reserved3;        // 保留字段
    /* 0x050 */ UINT16 Ss;               // 栈段寄存器
    /* 0x052 */ UINT16 Reserved4;        // 保留字段
    /* 0x054 */ UINT16 Ds;               // 数据段寄存器
    /* 0x056 */ UINT16 Reserved5;        // 保留字段
    /* 0x058 */ UINT16 Fs;               // 附加段寄存器（Windows特殊用途）
    /* 0x05a */ UINT16 Reserved6;        // 保留字段
    /* 0x05c */ UINT16 Gs;               // 附加段寄存器
    /* 0x05e */ UINT16 Reserved7;        // 保留字段
    /* 0x060 */ UINT16 LDT;              // 局部描述符表选择子
    /* 0x062 */ UINT16 Reserved8;        // 保留字段
    /* 0x064 */ UINT16 Flags;            // 处理器标志（原类型：Uint2B）
    /* 0x066 */ UINT16 IoMapBase;        // I/O权限位图基址偏移
    /* 0x068 */ UCHAR IoMaps[1];         // I/O访问位图（原类型：_KiIoAccessMap[1]）
    /* 0x208c */ UCHAR IntDirectionMap[32]; // 中断方向映射表（32字节）
} KTSS32, * PKTSS32;



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 1. 打印TSS结构

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s1()
{
    KTSS32* pTSS;
    __asm
    {
        mov eax, fs: [0x40]
        mov[pTSS], eax
    }
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Backlink : %08X \n", pTSS->Backlink);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Esp0 : %08X \n", pTSS->Esp0);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Ss0 : %08X \n", pTSS->Ss0);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "CR3 : %08X \n", pTSS->CR3);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Eip : %08X \n", pTSS->Eip);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "EFlags : %08X \n", pTSS->EFlags);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Eax : %08X \n", pTSS->Eax);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Ecx : %08X \n", pTSS->Ecx);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Edx : %08X \n", pTSS->Edx);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Ebx : %08X \n", pTSS->Ebx);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Esp : %08X \n", pTSS->Esp);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Ebp : %08X \n", pTSS->Ebp);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Es : %08X \n", pTSS->Es);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Cs : %08X \n", pTSS->Cs);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Ss : %08X \n", pTSS->Ss);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Ds : %08X \n", pTSS->Ds);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Fs : %08X \n", pTSS->Fs);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Gs : %08X \n", pTSS->Gs);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "LDT : %08X \n", pTSS->LDT);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Flags : %08X \n", pTSS->Flags);
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 2. 通过调用门构建   任务段,实现任务切换  

//   有2个问题: 1. 应该是CR3的问题,导致异常,win7和xp的处理不同
//              2. int 3中断导致异常.
//    x64直接放弃了任务切换,没什么意义调这个


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static UINT32 param1_s2, param2_s2, param3_s2;
static KTSS32 tss;
static char* pStackEsp;
static char* pStackEsp0;
static UINT32 g_cr3;
static char buf[6] = { 0,0,0,0,0x28,0 };

static void __declspec(naked) func_task_s2()
{
    _asm {
        // win7开始,需要设置TSS.cr3,
        //mov eax, fs: [0x40] ;        // 获取当前TSS
        //mov ebx, [g_cr3];
        //mov[eax + 0x1c], ebx       // 设置TSS的CR3字段

    }

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Task run\n");

    __asm
    {
        // 无法解决问题:  int 3后会蓝屏
        // int 3;        int 3 ;会清0  NT位.导致iretd无法执行任务返回,而是执行栈返回,导致蓝屏
        // pushfd
        // or dword ptr[esp], 0x4000
        // popfd 
        // 但是修复后还是会蓝屏.根本无法单步跟踪.

        // int 3;    // jmp 同样会蓝屏.


        // 由于win7任务切换没有设置TSS.CR3字段,导致任务切换后无法访问内核空间.
        // 这里要自己补上CR3.



        iretd
        // jmp fword ptr buf;
    }
}

static void __declspec(naked) func_call_gate_s2()
{
    __asm {
        push 0x30
        pop fs

        pushad
        pushfd

        //int 3;

        // 获取CR3,并保存到TSS的CR3字段中.
        mov eax, fs: [0x124]        // pEThread
        mov eax, [eax + 0x150]      // pEPRocess
        mov eax, [eax + 0x18]       // cr3
        lea ebx, tss
        mov[ebx + 0x1c], eax        // tss.cr3
        mov[g_cr3], eax             // 保存CR3到全局变量.

        lea eax, func_task_s2
        mov[ebx + 0x20], eax		// tss.eip
    }

    tss.Ss0 = 0x10;
    tss.Esp0 = (ULONG)pStackEsp0 + 0x2000;

    tss.Ss = 0x10;
    tss.Esp = (ULONG)pStackEsp + 0x2000;

    tss.Cs = 0x8;
    tss.Ds = 0x23;
    tss.Es = 0x23;
    tss.Fs = 0x30;
    tss.EFlags = 0x2;
    tss.IoMapBase = 0x20ac;
    tss.CR3 = g_cr3;

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "TSS : %08X\n", &tss);

    __asm {
        popfd
        popad
        retf 			// 平栈平的是R3栈.R0栈顶不用管它.每次都是从TSS.Esp0取.
    }
}

static void s2()
{
    UINT64 descriptor;
    UINT32 index;
    UINT64 addr;

    index = 0x1;
    addr = (UINT64)func_call_gate_s2;
    descriptor = 0x0000EC0000000000;
    descriptor |= (UINT64)(index << 3 | 3) << 16;
    descriptor |= addr & 0xFFFF;
    descriptor |= (addr & 0xFFFF0000) << 32;
    index = mAddGDTDescriptorx86(descriptor);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "R0 调用门 : %04X   描述符 : %08X`%08X\n", (index << 3) | 3, (UINT32)(descriptor >> 32), (UINT32)descriptor);

    // 添加一个TSS描述符.
    descriptor = 0x0000E90000000200;
    addr = (UINT64)&tss;
    descriptor |= (addr & 0xFFFFFF) << 16;
    descriptor |= (addr >> 24) << 56;
    index = mAddGDTDescriptorx86(descriptor);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "TSS selector : %04X   descriptor : %08X`%08X\n", (index << 3 | 3), (UINT32)(descriptor >> 32), (UINT32)descriptor);

    pStackEsp = (PCHAR)ExAllocatePoolWithTag(NonPagedPool, 0x3000, 'xxx');
    pStackEsp0 = (PCHAR)ExAllocatePoolWithTag(NonPagedPool, 0x3000, 'xxx');

    pStackEsp0[0x2000 - 1] = 0;
    pStackEsp[0x2000 - 1] = 0;
    pStackEsp0[0x1000 - 1] = 0;
    pStackEsp[0x1000 - 1] = 0;

#if 0
#include<Windows.h>
#include<stdio.h>

    int main()
    {
        char buf1[6] = { 0,0,0,0,0x4b,0 };
        char buf2[6] = { 0,0,0,0,0x63,0 };
        __asm {
            push fs
            call fword ptr buf1
            pop fs

            call fword ptr buf2
        }
        printf("end\n");
        system("pause");
        return 0;
    }
#endif 
}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void p000_006()
{
    s2();
}
