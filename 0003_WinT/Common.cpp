#include "pch.h"
#include "Common.h"
#include<locale.h>


// 获取文件数据
int mGetFileData(const char* path, char** ppBuffer, int* pSize)
{
	/**
	 * @brief				获取文件数据
	 * @param path			文件路径
	 * @param ppBuffer		返回文件数据指针
	 * @param pSize			返回文件大小
	 * @return				成功返回0，失败返回错误码: -1:参数错误, -2:打开文件失败, -3:获取大小失败, -4:分配内存失败, -5:读取数据失败
	*/

	if( path == NULL || ppBuffer == NULL || pSize == NULL || strlen(path) == 0)
		return -1;

	//FILE* pf = fopen(pathA, "rb");
	FILE* pf = NULL;
	errno_t err = fopen_s(&pf, path, "rb");
	if (err != 0 || pf == NULL)
		return -2;

	fseek(pf, 0, SEEK_END);
	int size = ftell(pf);
	if (size == 0 ||size < 0)
		return -3;

	char* pBuffer = (char*)calloc(size, sizeof(char));
	if (pBuffer == NULL)
		return -4;

	fseek(pf, 0, SEEK_SET);
	int n = (int)fread(pBuffer, sizeof(char), size, pf);
	if (n != size)
		return -5;

	fclose(pf);

	*ppBuffer = pBuffer;
	*pSize = size;

	return 0;
}

// 保存文件数据
int mSaveFileData(const char* path, const char* pBuffer, int size)
{
	if (path == NULL || pBuffer == NULL || size <= 0)
		return -1;

	FILE* pf = NULL;
	errno_t err = fopen_s(&pf, path, "wb");
	if (err != 0 || pf == NULL)
		return -2;

	int n = (int)fwrite(pBuffer, sizeof(char), size, pf);
	if (n != size)
		return -3;

	fclose(pf);
	return 0;
}



static int b_console = 0;

int mCreateConsole(const TCHAR title[])
{
	if (!b_console )
	{
		if (!AllocConsole())
			return -1;

		b_console = 1;

		// 重载C 标准输入输出
		FILE* in, * out, * err;
		freopen_s(&in, "CONIN$", "r+t", stdin);
		freopen_s(&out, "CONOUT$", "w+t", stdout);
		freopen_s(&err, "CONERR$", "w+t", stderr);

		setlocale(LC_ALL, "");	// 设置本地化为当前环境,解决wprintf中文乱码问题

		if (title)
			SetConsoleTitle(title);

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

