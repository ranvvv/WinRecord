#pragma once

#include<ntddk.h>
#include<ntstrsafe.h>
#include<stdarg.h>
#include<stdio.h>
#include<ntimage.h>	// 定义了PE结构.例如IMAGE_DOS_HEADER,IMAGE_NT_HEADERS
#include<intrin.h>	// 定义了内联汇编指令
#include<wdm.h>		// 定义了驱动开发常用的宏
#include"order_function.h"



#ifdef _WIN64

#else
#include"000_000_保护模式x86.h"
#endif







