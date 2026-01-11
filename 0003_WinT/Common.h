#pragma once


int mGetFileData(const char* path, char** ppBuffer, int* pSize);		// 读取文件到缓冲区
int mSaveFileData(const char* path, const char* pBuffer, int size);		// 保存缓冲区到文件


int mCreateConsole(const TCHAR title[]);	// 创建控制台窗口
void mCloseConsole();						// 关闭控制台窗口
