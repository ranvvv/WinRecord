#if 0
#include<windows.h>
#include<stdio.h>
#include"01_CSimpleList.h"

struct MyThreadData
{
	MyThreadData* pNext;
	int nSomeData;
};

int main(int argc, char** argv)
{
	MyThreadData* pData;
	CTypedSimpleList<MyThreadData*> list;

	list.Construct(offsetof(MyThreadData, pNext)); // 告诉CSimpleList 类pNext 成员的偏移量

	// 向链表中添加成员
	for (int i = 0; i < 10; i++)
	{
		pData = new MyThreadData;
		pData->nSomeData = i;
		list.AddHead(pData);
	}

	// ...... // 使用链表中的数据
	// 遍历整个链表，释放MyThreadData 对象占用的空间
	pData = list;
	while (pData != NULL)
	{
		MyThreadData* pNextData = pData->pNext;
		printf(" The value of nSomeData is: %d \n", pData->nSomeData);
		delete pData;
		pData = pNextData;
	}
	return 0;
}
#endif