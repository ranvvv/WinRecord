#include"../common.h"

// 1. 代码跨段跳转




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 1. 代码跨段跳转 

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s1()
{
	// 添加一个非一致代码段描述符,用来执行远跳转
	UINT64 descriptor;
	UINT32 index;
	descriptor = 0x00cf9b000000ffff;
	index = mAddGDTDescriptorx86(descriptor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"r0 非一致代码段选择子: %04X   描述符 : %08X`%08X\n", (index << 3), (UINT32)(descriptor >> 32), (UINT32)descriptor);


	// jmp far数据
	static char code_[6] = { 0x0,0x0,0x0,0x0,0x0,0x0 };	// 远跳数据
	static char code_old[6] = { 0x0,0x0,0x0,0x0,0x0,0x0 }; // 远跳回数据


	// ============================= jmp far 测试
#if 01

	code_[0x4] = (UCHAR)(index << 3);		// 设置远跳转选择子

	__asm
	{
		// 设置返回地址
		lea eax, position_code
		mov dword ptr[code_old], eax
		mov ax, cs
		mov word ptr[code_old + 4], ax

		// 设置远跳转地址
		lea eax, position_jmpfar
		mov dword ptr[code_], eax

		// 执行远跳转
		jmp fword ptr[code_]
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL," ============ not be output ========\n");		// 跨段跳过此行代码
	return;

	__asm {
	position_jmpfar:		// 远跳位置
		mov eax, eax
		mov eax, eax
		mov eax, eax
		jmp fword ptr ds : [code_old]  // 跳回去
	}

	__asm {
	position_code:
		mov eax, eax
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL," ============ jmp far ok! ========\n");

#endif



	// ============================= call far 测试
#if 01
	code_[0x4] = (UCHAR)(index << 3);

	__asm
	{
		lea eax, position_callfar
		mov dword ptr[code_], eax
		call fword ptr[code_]
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL," ============ call far ok! ========\n");

	return;

	__asm {
	position_callfar:
		mov eax, eax
			mov eax, eax
			mov eax, eax
			retf
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL," ============ not be output ========\n");		// 跨段跳过此行代码
#endif
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_002()
{
	s1();
}
