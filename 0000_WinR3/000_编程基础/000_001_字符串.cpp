#include"../common.h"

#include<strsafe.h>
#include<inttypes.h>
#include<shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

// 1. 字符宽度问题
// 2. v版变参函数的使用
// 3. 调试器格式化输出
// 4. 字符串转换
// 5. 打印错误文本信息


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				1. 字符宽度问题

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s1()
{
	size_t len1, len2, len3;
	// ----------------------- C 字符串类型
	char cStringA[] = "中国";				// GBK,		多字节字符集
	wchar_t cStringW1[] = L"中国";			// UTF-16   宽字符集 以16位为单位存储字符
	wchar_t cStringW2[] = L"𮧵‌";			// UTF-16   生僻字要用2个16位 例如: 𮧵‌ UTF-16编码‌：D87ADDF5

	printf("cStringA : %s\n", cStringA);
	wprintf(L"cStringW1 : %s\n", cStringW1);
	wprintf(L"cStringW2 : %s\n", cStringW2);


	len1 = strlen(cStringA);
	len2 = wcslen(cStringW1);
	len3 = wcslen(cStringW2);				// 由于有零宽度连接符, 所以长度为3 0xD87A 0xDDF5 0x200C 0x0000
	printf("len1 : %d, len2 : %d , len3 : %d\n", (int)len1, (int)len2, (int)len3);

	// ----------------------- windows 字符串类型
	CHAR winStringA[] = "中国";
	WCHAR winStringW1[] = L"中国";
	TCHAR winStringW2[] = TEXT("中国");		// 自适应宽度,根据项目宽度定义宏.

	len1 = lstrlenA(winStringA);
	len2 = lstrlenW(winStringW1);
	len3 = lstrlen(winStringW2);			// 自适应宽度
	printf("len1 : %d, len2 : %d , len3 : %d\n", (int)len1, (int)len2, (int)len3);

	printf("num : %x\n", 0x1234abcd);
	wprintf(L"num : %x\n", 0x1234abcd);
	_tprintf(TEXT("num : %x \n"), 0x1234abcd);


	// ----------------------- 字符串格式化
	char buf1[10];
	sprintf_s(buf1, 10, "%X\n", 0x22222222);    // 会在结尾补\0
	printf(buf1);

	wchar_t buf2[200];
	swprintf_s(buf2, 200, L"num %X\n", 0x22222222);
	wprintf(buf2);

	TCHAR buf3[200];
	_stprintf_s(buf3, 200, TEXT("data : %X \n"), 0x1234abcd);
	_tprintf(buf3);


	// ------------------------ safe 系列函数
	// <strsafe.h> 版本字符串操作函数,替换了sprintf系列函数.
	TCHAR buffer[0x200];
	TCHAR buffer2[0x200];
	StringCbPrintf(buffer, 0x200, TEXT("data : %X \n"), 0x1234abcd);
	StringCbCat(buffer, 0x200, TEXT("data : %X \n"));
	StringCbCopyN(buffer2, 0x200, buffer, 10);


	//	----------------------- T版函数原理
	// 	#define UNICDODE
	//	#else
	//	#endif

	// _tprintf(TEXT("define UNICDODE\n"));
	//	lstrlen();
	//	lstrcat();

	//	----------------------- V版函数可处理变参函数
	//	_vstprintf();

	//	内核中使用的都是W版的, 所以A后缀的winAPI进入内核前会做字符转换.
}




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				2. v版 变参函数的使用

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 格式化printf
static int mtsprintf(PTCHAR buffer, size_t bufferCount, const TCHAR* const format, ...)
{
	int result;
	va_list pArgs;
	va_start(pArgs, format);
	result = _vstprintf(buffer, bufferCount, format, pArgs);  // v版接受...可变参数 : <tchar.h>
	va_end(pArgs);
	return result;
}

static void s2()
{
	TCHAR buf4[200];
	mtsprintf(buf4, 200, TEXT("data : %X \n"), 0x1234abcd);
	_tprintf(buf4);
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				3. 调试器格式化输出

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void mDbgPrintA(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char buf[1024] = { 0 };
	vsprintf_s(buf, 1024, format, args);
	va_end(args);
	OutputDebugStringA(buf);
}

void mDbgPrintW(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	wchar_t buf[1024] = { 0 };
	vswprintf_s(buf, 1024, format, args);
	va_end(args);
	OutputDebugStringW(buf);
}

void mDbgPrint(const TCHAR* const format, ...)
{
	va_list args;
	va_start(args, format);
	TCHAR buf[1024] = { 0 };
#ifdef  UNICODE    
	vswprintf_s(buf, 1024, format, args);
	OutputDebugStringW(buf);
#else
	vsprintf_s(buf, 1024, format, args);
	OutputDebugStringA(buf);
#endif
	va_end(args);
}

static void s3()
{
	const char* a = "zzzzzzzzzz";
	const wchar_t* b = L"zzzzzzzzzz";
	const TCHAR* c = TEXT("zzzzzzzzzz");
	mDbgPrintA("测试 : %d %s\n", 123, a);
	mDbgPrintW(L"测试 : %d %s\n", 123, b);
	mDbgPrint(TEXT("测试 : %d %s\n"), 123, c);
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				4. 字符串转换

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


PCHAR GB2312ToUTF8(const CHAR* str)
{
	int len;  // len都是以字符为单位的.

	len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	if (len == 0)
		return NULL;

	PWCHAR  wstr = new WCHAR[len];
	len = MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, len);
	if (len == 0)
		return NULL;

	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (len == 0)
		return NULL;

	PCHAR utf8 = new CHAR[len];
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8, len, NULL, NULL);
	delete[] wstr;
	if (len == 0)
	{
		delete[] utf8;
		return NULL;
	}

	return utf8;
}

static void s4()
{
	char bufChar[0x20];
	wchar_t bufWChar[0x20];
	int dataInt;
	long long dataLongLong;

	// ============================= 数字转字符串

	// 数字转字符串
	_itoa_s(0x1234abcd, bufChar, 0x20, 16);
	printf("%s\n", bufChar);

	// 数字 转 宽字符
	_itow_s(0x1234abcd, bufWChar, 0x20, 16);
	wprintf(L"%s\n", bufWChar);

	// 64位数字转字符串
	_i64toa_s(0x1234567890abcdef, bufChar, 0x20, 16);
	printf("%s\n", bufChar);

	// 64位数字转宽字符
	_i64tow_s(0x1234567890abcdef, bufWChar, 0x20, 16);
	wprintf(L"%s\n", bufWChar);

	// ============================= 字符串转数字

	// 字符串转数字 // 16进制必须0x开头
	StrToInt64Ex(TEXT("0x1234567890abcdef"), STIF_SUPPORT_HEX, &dataLongLong);
	StrToIntEx(TEXT("0X1234abcd"), STIF_SUPPORT_HEX, &dataInt);
	printf("%I64X %08X\n", dataLongLong, dataInt);

	// 字符串转 数字
	dataInt = _tcstoul(TEXT("E0000000"), NULL, 16);		// 无符号转换
	printf("%08X\n", dataInt);
	dataInt = _tcstol(TEXT("-0003333"), NULL, 16);		// 有符号转换,  最高位不要为1
	printf("%08X\n", dataInt);

	// 字符串转64位数字    这个好用,要4字节就直接截取
	dataLongLong = _tcstoi64(TEXT("a0000000"), NULL, 16);
	printf("%I64X\n", dataLongLong);
	dataLongLong = _tcstoi64(TEXT("0xa0000000"), NULL, 16);
	printf("%I64X\n", dataLongLong);



	// ============================= 多字节与宽字符的转换

	// 宽字符转多字节
	wchar_t wideString[] = L"你好世界";
	int bufferSize = WideCharToMultiByte(CP_ACP, 0, wideString, -1, NULL, 0, NULL, NULL);	// 计算容量是包括\0在内的字符个数
	char* multiByteString = new char[bufferSize];
	WideCharToMultiByte(CP_ACP, 0, wideString, -1, multiByteString, bufferSize, NULL, NULL);
	printf("%s\n", multiByteString);
	delete[] multiByteString;

	// 定义多字节字符串
	char String[] = "Hello";
	int wcsLen = MultiByteToWideChar(CP_ACP, 0, String, -1, NULL, 0);
	wchar_t* wcs = new wchar_t[wcsLen];
	MultiByteToWideChar(CP_ACP, 0, String, -1, wcs, wcsLen);
	wprintf(L"%s\n", wcs);
	delete[] wcs;
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				5. 打印错误文本信息

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 打印错误信息
void mPError()
{
	TCHAR buf[0x200];
	if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 0x200, NULL))
		StringCbPrintf(buf, 0x200, TEXT("FormatMessage Error Code: %d"), GetLastError());
	_tprintf(TEXT("%s\n"), buf);
}

static void s5()
{
	DWORD err = GetLastError();		// 工具>错误查找
	printf("%d\n", err);
	mPError();
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				p000_001

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_001()
{
	s1();
	s2();
	s3();
	s4();
	s5();
}
