#include"../common.h"

// 1. CPU的3个表寄存器: GDT表,IDT表,TSS表
// 2. 打印段描述符
// 3. 打印GDT表
// 4. 添加GDT表项
// 5. 打印IDT表 
// 6. 添加IDT段描述符

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 1. CPU的3个表寄存器: GDT表,IDT表,TSS表

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s1()
{
	// GDT表: gdtr
	char _gdtr[6] = { 0 };
	__asm sgdt fword ptr ds : [_gdtr] ;
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "GDT base:%08X  num:%04X\n", (UINT32)(*(UINT32*)(_gdtr + 2)), *(UINT16*)(_gdtr + 0));

	// IDT表: idtr
	char _idtr[6] = { 0 };
	__asm sidt fword ptr ds : [_idtr] ;
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IDT base:%08X  num:%04X\n", (UINT32)(*(UINT32*)(_idtr + 2)), *(UINT16*)(_idtr + 0));

	// TSS表: tr
	short SegTr;
	__asm {
		str ax;
		mov[SegTr], ax
	}
	UINT64 descriptor = *(PUINT64)((*(UINT32*)(_gdtr + 2)) + SegTr & (~7));
	UINT32 base = ((descriptor >> 16) & 0xFFFFFF) | ((descriptor >> 32) & 0xFF000000);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "TSS base:%08X\n", base);
}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 2. 打印段描述符

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void mPrintDescritporx86(UINT64 descriptor)
{
	UCHAR P = (UCHAR)((descriptor >> 47) & 1);				// 有效性位P
	UCHAR DPL = (UCHAR)((descriptor >> 45) & 3);			// 特权级DPL
	UCHAR S = (UCHAR)((descriptor >> 44) & 1);				// 描述符类型S
	UCHAR TYPE = (UCHAR)((descriptor >> 40) & 0xF);			// 类型字段TYPE

	if (P)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"%08X`%08X \t\t", (UINT32)(descriptor >> 32), (UINT32)descriptor);

		if (S)// 代码段或数据段
		{
			UCHAR G = (UCHAR)((descriptor >> 55) & 1);				// 粒度G
			UCHAR D_B = (UCHAR)((descriptor >> 54) & 1);			// 操作大小D_B
			UCHAR AVL = (UCHAR)((descriptor >> 52) & 1);			// 可用位AVL
			UINT32 Limit = (UINT32)(descriptor & 0xFFFF);			// 段界限字段Limit
			Limit |= (UINT32)((descriptor >> 32) & 0x0F0000);		// 扩展段界限字段Limit
			UINT32 Base = (UINT32)((descriptor >> 16) & 0xFFFFFF);	// 基址字段Base
			Base |= (UINT32)((descriptor >> 32) & 0xFF000000);		// 扩展基址字段Base

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"P:%d DPL:%01X S:%d TYPE:%01X G:%d D_B:%d AVL:%d Base:%08X Limit:%08X\n", P, DPL, S, TYPE, G, D_B, AVL, Base, Limit);

			if (TYPE & 0X8)
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"\t代码段:");
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  ");
				(TYPE & 0x4) ? DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"一致") : DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"非一致");
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  ");
				(TYPE & 0x2) ? DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"可读") : DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"不可读");
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  ");
				(TYPE & 0x1) ? DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"访问过") : DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"未访问过");
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  ");
			}
			else
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"\t数据段:");
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  ");
				(TYPE & 0x4) ? DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"向下拓展") : DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"向上拓展");
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  ");
				(TYPE & 0x2) ? DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"可写") : DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"不可写");
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  ");
				(TYPE & 0x1) ? DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"访问过") : DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"未访问过");
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  ");
			}
		}
		else // 系统描述符
		{
			UINT32 selector;
			UINT32 offset;
			UINT32 base;
			UINT32 limit;
			switch (TYPE)
			{
			case 0:
			case 8:
			case 10:
			case 13:
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"Reserved\n");
				break;
			case 1:
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"16-Bit TSS Available\n");
				break;
			case 2:
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"LDT\n");
				break;
			case 3:
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"16-Bit TSS Busy\n");
				break;
			case 4:
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"16-Bit Call Gate\n");
				break;
			case 5:
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"Task Gate\n");
				break;
			case 6:
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"16-Bit Interrupt Gate\n");
				break;
			case 7:
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"16-Bit Trap Gate\n");
				break;
			case 9:
			case 11:
				if (TYPE == 9)
					DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"32-Bit TSS Available");
				else
					DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"32-Bit TSS Busy");
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  P:%d DPL:%01X S:%d TYPE:%01X ", P, DPL, S, TYPE);
				base = (UINT32)(descriptor >> 16) & 0xFFFFFF | (UINT32)((descriptor >> 32) & 0xFF000000);
				limit = (UINT32)(descriptor & 0xFFFF) | (UINT32)((descriptor >> 32) & 0x0F0000);
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  Base:%08X Limit:%08X\n", base, limit);
				break;
			case 12:
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"32-Bit Call Gate");
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  P:%d DPL:%01X S:%d TYPE:%01X Param:%01X ", P, DPL, S, TYPE, (UINT32)((descriptor >> 32) & 0x1F));
				selector = (UINT32)((descriptor >> 16) & 0xFFFF);
				offset = (UINT32)(descriptor & 0xFFFF);
				offset |= (UINT32)((descriptor >> 32) & 0xFFFF0000);
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  selector : %04X  offset : %08X\n", selector, offset);
				break;
			case 14:
			case 15:
				if (TYPE == 14)
					DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"32-Bit Interrupt Gate");
				else
					DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"32-Bit Trap Gate");
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  P:%d DPL:%01X S:%d TYPE:%01X", P, DPL, S, TYPE);
				selector = (UINT32)((descriptor >> 16) & 0xFFFF);
				offset = (UINT32)(descriptor & 0xFFFF);
				offset |= (UINT32)((descriptor >> 32) & 0xFFFF0000);
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"  selector : %04X  offset : %08X\n", selector, offset);
				break;
			default:
				break;
			}
		}
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"\n");
	}
}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 3. 打印GDT表

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s3()
{
	PUINT64 p_descriptor;
	UINT32 descriptor_num;
	UINT32 index;

	CHAR _gdtr[6] = { 0 };
	__asm sgdt fword ptr ds : [_gdtr] ;

	p_descriptor = (PUINT64) * (UINT32*)(_gdtr + 2);
	descriptor_num = *(UINT16*)(_gdtr + 0) / 8;
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "GDT base:%08X  num:%04X\n", (UINT32)p_descriptor, descriptor_num);

	for (index = 0; index < descriptor_num; index++)
	{
		if (!(p_descriptor[index] & (1LL << 47LL)))
			continue;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d : ", index);
		mPrintDescritporx86(p_descriptor[index]);
	}
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 4. 添加GDT表项

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static int g_flag_gdt = 0;

#define GDT_ITEM_MAX_NUM 0x20

struct GDT_ITEM
{
	UINT32 index;
	UINT64 descriptor;
};
static struct GDT_ITEM g_GDT[GDT_ITEM_MAX_NUM];


UINT32 mAddGDTDescriptorx86(UINT64 descriptor)
{
	KAFFINITY processors_bit_flag;				// 当前所有CPU位标记
	KAFFINITY current_processor_bit_flag;		// 要运行的CPU标记
	UINT32 cpu_max_num;							// 最大CPU数
	UINT32 i;
	char _gdtr[6] = { 0 };
	UINT32 descriptor_num;
	PUINT64 p_descriptor;
	UINT32 item_index = (UINT32)-1;
	UINT32 gdt_index = (UINT32)-1;

	for (i = 0; i < GDT_ITEM_MAX_NUM; i++)
	{
		if (!g_GDT[i].index)
		{
			item_index = i;
			break;
		}
	}
	if (item_index == -1)
		return (UINT32)-1;

	__asm sgdt fword ptr ds : [_gdtr] ;
	p_descriptor = (PUINT64) * (UINT32*)(_gdtr + 2);
	descriptor_num = *(UINT16*)(_gdtr + 0) / 8;
	for (i = 1; i < descriptor_num; i++)			// 从1号开始找, P 位为0的.
	{
		if (!(p_descriptor[i] & (1LL << 47)))		// 无效描述符
		{
			gdt_index = i;
			break;
		}
	}
	if (gdt_index == -1)
		return (UINT32)-1;


	// 切核设置描述符
	processors_bit_flag = KeQueryActiveProcessors();
	cpu_max_num = sizeof(KAFFINITY) * 8;
	for (i = 0; i < cpu_max_num; i++)
	{
		current_processor_bit_flag = processors_bit_flag & (1 << i);
		if (current_processor_bit_flag)
		{
			KeSetSystemAffinityThread(current_processor_bit_flag);
			__asm sgdt fword ptr ds : [_gdtr] ;
			p_descriptor = (PUINT64) * (UINT32*)(_gdtr + 2);
			p_descriptor[gdt_index] = descriptor;
		}
	}

	g_GDT[item_index].descriptor = descriptor;
	g_GDT[item_index].index = gdt_index;
	g_flag_gdt = 1;

	KeSetSystemAffinityThread(processors_bit_flag);	// 还原为所有CPU都可运行	

	return gdt_index;
}

void mCleanupGDTDescriptor()
{
	KAFFINITY processors_bit_flag;				// 当前所有CPU位标记
	KAFFINITY current_processor_bit_flag;		// 要运行的CPU标记
	UINT32 cpu_max_num;							// 最大CPU数
	UINT32 i, j;
	PUINT64 p_descriptor;

	if (g_flag_gdt == 0)
		return;

	char _gdtr[6] = { 0 };
	processors_bit_flag = KeQueryActiveProcessors();
	cpu_max_num = sizeof(KAFFINITY) * 8;

	for (i = 0; i < GDT_ITEM_MAX_NUM; i++)
	{
		if (g_GDT[i].index)
		{
			for (j = 0; j < cpu_max_num; j++)
			{
				current_processor_bit_flag = processors_bit_flag & (1 << j);
				if (current_processor_bit_flag)
				{
					KeSetSystemAffinityThread(current_processor_bit_flag);
					__asm sgdt fword ptr ds : [_gdtr] ;
					p_descriptor = (PUINT64) * (UINT32*)(_gdtr + 2);
					p_descriptor[g_GDT[i].index] = 0;
				}
			}
			g_GDT[i].descriptor = 0;
			g_GDT[i].index = 0;
		}
	}
	g_flag_gdt = 0;
	KeSetSystemAffinityThread(processors_bit_flag);	// 还原为所有CPU都可运行	
}

static void s4()
{
	UINT32 index = mAddGDTDescriptorx86(0x1111E22233334444);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "new gdt descriptor %04X\n", index);
	// # dq gdtr L30 ;
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 5. 打印IDT表 

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s5()
{
	PUINT64 p_descriptor;
	UINT32 descriptor_num;
	UINT32 index;
	char _idtr[6] = { 0 };
	__asm sidt fword ptr ds : [_idtr] ;

	p_descriptor = (PUINT64) * (UINT32*)(_idtr + 2);
	descriptor_num = *(UINT16*)(_idtr + 0) / 8;
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"IDT base:%08X  num:%04X\n", (UINT32)p_descriptor, descriptor_num);

	for (index = 0; index < descriptor_num; index++)
	{
		if (!(p_descriptor[index] & (1LL << 47LL)))
			continue;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,	"%04d : ", index);
		mPrintDescritporx86(p_descriptor[index]);
	}
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 6. 添加IDT段描述符

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static int g_flag_idt = 0;

#define IDT_ITEM_MAX_NUM 0x20

struct IDT_ITEM
{
	UINT32 index;
	UINT64 descriptor;
};
static struct IDT_ITEM g_IDT[IDT_ITEM_MAX_NUM];


UINT32 mAddIDTDescriptorx86(UINT64 descriptor)
{
	KAFFINITY processors_bit_flag;				// 当前所有CPU位标记
	KAFFINITY current_processor_bit_flag;		// 要运行的CPU标记
	UINT32 cpu_max_num;							// 最大CPU数


	char _idtr[6] = { 0 };
	UINT32 descriptor_num;
	PUINT64 p_descriptor;
	UINT32 i;
	UINT32 idt_index = (UINT32)-1;
	UINT32 item_index = (UINT32)-1;

	for (i = 0; i < IDT_ITEM_MAX_NUM; i++)
	{
		if (!g_IDT[i].index)
		{
			item_index = i;
			break;
		}
	}
	if (item_index == -1)
		return (UINT32)-1;

	__asm sidt fword ptr ds : [_idtr] ;
	p_descriptor = (PUINT64) * (UINT32*)(_idtr + 2);
	descriptor_num = *(UINT16*)(_idtr + 0) / 8;

	for (i = 1; i < descriptor_num; i++)			// 从1号开始找, P 位为0的.
	{
		if (!(p_descriptor[i] & (1LL << 47)))
		{
			idt_index = i;
			break;
		}
	}
	if (idt_index == -1)
		return (UINT32)-1;


	processors_bit_flag = KeQueryActiveProcessors();
	cpu_max_num = sizeof(KAFFINITY) * 8;

	for (i = 0; i < cpu_max_num; i++)
	{
		current_processor_bit_flag = processors_bit_flag & (1 << i);
		if (current_processor_bit_flag)
		{
			KeSetSystemAffinityThread(current_processor_bit_flag);
			__asm sidt fword ptr ds : [_idtr] ;
			p_descriptor = (PUINT64) * (UINT32*)(_idtr + 2);
			p_descriptor[idt_index] = descriptor;
		}
	}


	g_IDT[item_index].descriptor = descriptor;
	g_IDT[item_index].index = idt_index;

	g_flag_idt = 1;

	KeSetSystemAffinityThread(processors_bit_flag);	// 还原为所有CPU都可运行	

	return idt_index;
}

void mCleanupIDTDescriptor()
{
	KAFFINITY processors_bit_flag;	// 当前所有CPU位标记
	KAFFINITY current_processor_bit_flag;		// 要运行的CPU标记
	UINT32 cpu_max_num;				// 最大CPU数
	UINT32 i, j;
	PUINT64 p_descriptor;
	char _idtr[6] = { 0 };

	if (g_flag_idt == 0)
		return;

	processors_bit_flag = KeQueryActiveProcessors();
	cpu_max_num = sizeof(KAFFINITY) * 8;


	for (i = 0; i < IDT_ITEM_MAX_NUM; i++)
	{
		if (g_IDT[i].index)
		{
			for (j = 0; j < cpu_max_num; j++)
			{
				current_processor_bit_flag = processors_bit_flag & (1 << j);
				if (current_processor_bit_flag)
				{
					KeSetSystemAffinityThread(current_processor_bit_flag);
					__asm sidt fword ptr ds : [_idtr] ;
					p_descriptor = (PUINT64) * (UINT32*)(_idtr + 2);
					p_descriptor[g_IDT[i].index] = 0;
				}
			}
			g_IDT[i].descriptor = 0;
			g_IDT[i].index = 0;
		}
	}
	KeSetSystemAffinityThread(processors_bit_flag);	// 还原为所有CPU都可运行

	g_flag_idt = 0;
}

static void s6()
{
	UINT32 index = mAddIDTDescriptorx86(0x1111E22233334444);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"new Idt descriptor %04X\n", index);

	// # dq idtr L30
}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

VOID p000_001()
{
	s4();
	s6();
}

VOID p000_001_cleanup()
{
	mCleanupGDTDescriptor();
	mCleanupIDTDescriptor();
}
