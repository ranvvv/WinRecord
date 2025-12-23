#include<windows.h>
#include"07_CPlex.h"
#include<iostream>
using namespace std;

#if 0

// 数据区
struct CMyData
{
	int nSomeData;
	int nSomeMoreData;
};

int main()
{
	CPlex* pBlocks = NULL;							// 用于保存链中第一个内存块的首地址，必须被初始化为NULL 
	CPlex::Create(pBlocks, 10, sizeof(CMyData));
	CMyData* pData = (CMyData*)pBlocks->data();
	// 现在pData 是CPlex::Create 函数申请的10 个CMyData 结构的首地址
	//... // 使用pData 指向的内存
	// 使用完毕，继续申请
	CPlex::Create(pBlocks, 10, sizeof(CMyData));
	pData = (CMyData*)pBlocks->data();
	// 最后释放链中的所有内存块
	pBlocks->FreeDataChain();
}
#endif

#if 0
int main() // English-Chinese 字典
{
	CMapPtrToPtr map;
	char szDay[][16] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
	// 向映射中添加项
	map[szDay[0]] = (void*)"星期日"; // 这里主要调用了operator [ ]函数
	map[szDay[1]] = (void*)"星期一";
	map[szDay[2]] = (void*)"星期二";
	map[szDay[3]] = (void*)"星期三";
	map[szDay[4]] = (void*)"星期四";
	map[szDay[5]] = (void*)"星期五";
	map[szDay[6]] = (void*)"星期六";
	// 查询
	cout << szDay[4] << " : " << (char*)map[szDay[4]] << "\n";
}


#endif
