#include"common.h"


// common main
int APIENTRY mWinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PTSTR cmdline, int cmdshow)
{
	p000_001();
	return 0;
}


// cmd main
int main(int argc, char** argv)
{
	/*!
	 * @brief 		console 入口函数
	 * @param argc  命令行参数个数
	 * @param argv  命令行参数
	 * @return
	*/

	int result = 0;

	//setlocale(LC_ALL, "Chinese_China.936");	// 控制台GBK编码,解决控制台中文乱码问题
	//SetConsoleOutputCP(936);				// GBK编码
	//SetConsoleOutputCP(1200);
	//SetConsoleOutputCP(65001);				// UTF-8编码

	setlocale(LC_ALL, "");

	result = mWinMain(GetModuleHandle(NULL), NULL, NULL, SW_NORMAL);
	system("pause");
	return result;
}


// gui  main
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	/*!
	 * @brief					GUI 入口函数		_tWinMain : WinMain的T版本
	 * @param hInstance			exe模块基址,模块句柄
	 * @param hPrevInstance		NULL
	 * @param lpCmdLine			命令行参数
	 * @param nCmdShow			显示方式  SW_NORMAL
	 * @return					int
	*/

	int result = 0;

	mCreateConsole(NULL);	// 新建控制台 重载标准输出/输入流
	result = mWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	mCloseConsole();		// 关闭控制台
	return result;
}