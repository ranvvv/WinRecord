#include "pch.h"
#include "Common.h"

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

