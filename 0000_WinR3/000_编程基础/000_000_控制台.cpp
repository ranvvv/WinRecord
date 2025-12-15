#include"../common.h"

//				1. console的基本使用
//				2. 新建控制台


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				1. console的基本使用

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
static void s1()
{
	char buf[1024];
	DWORD outNum;

	// 获取控制台句柄
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
	printf("hIn=%p hOut=%p hErr=%p\n", hIn, hOut, hErr);

	// 写
	WriteConsoleA(hOut, "1234567890\n", 11, NULL, NULL);

	// 读
	memset(buf, 0, sizeof buf);
	ReadConsoleA(hIn, &buf, 1024 - 1, &outNum, NULL);
	printf("%d %s\n", outNum, buf);

	// 管理
	SetConsoleTitleA("1234567890\n");		// 设置标题
	GetConsoleTitleA(buf, 1024);			// 获取标题
	HWND hwnd = GetConsoleWindow();			// 获取窗口句柄
	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY); // 设置文字颜色
	FillConsoleOutputAttribute(hOut, 0x07, 1024 * 1024, COORD{ 0, 0 }, &outNum); // 设置文字颜色
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				2. 新建控制台

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#include<locale.h>
static int b_console = 0;

int mCreateConsole(const TCHAR title[])
{
	if (!b_console && AllocConsole())
	{
		// 重载C 标准输入输出
		FILE* in, * out, * err;
		freopen_s(&in, "CONIN$", "r+t", stdin);
		freopen_s(&out, "CONOUT$", "w+t", stdout);
		freopen_s(&err, "CONERR$", "w+t", stderr);


		setlocale(LC_ALL, "");	// 设置本地化为当前环境,解决wprintf中文乱码问题

		if (title)
			SetConsoleTitle(title);
		b_console = 1;
		return 1;
	}
	return 0;
}

void mCloseConsole()
{
	if (b_console)
	{
		printf("按任意键退出...\n");
		getchar();
		fclose(stderr);
		fclose(stdout);
		fclose(stdin);
		FreeConsole();
		b_console = 0;
	}
}

static void s2()
{
	mCreateConsole(TEXT("测试"));
	printf("hello world!\n");
	mCloseConsole();
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void p000_000()
{
	s1();
}





