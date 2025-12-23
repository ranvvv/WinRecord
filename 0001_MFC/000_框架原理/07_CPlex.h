#pragma once
#include<windows.h>


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					 CPlex   :  数据区

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


struct CPlex
{
	CPlex* pNext; // 向每个内层块中添加的额外信息，指向下一个内存块首地址的指针

	void* data();

	static CPlex* Create(CPlex*& pHead, UINT nMax, UINT cbElement);		// 用于申请内存的全局函数。申请cbElement 大小的空间nMax 个

	void FreeDataChain();				// 释放以当前对象为首地址（this 指针）的内存链中的所有内存
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					 CMapPtrToPtr   :  key=>value

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class CMapPtrToPtr
{
protected:
	// 关联（Association）
	struct CAssoc
	{
		CAssoc* pNext;		// 指向下一个CAssoc 结构
		void* key;			// 键对象
		void* value;		// 值对象
	};
protected:
	CAssoc** m_pHashTable;
	int m_nHashTableSize;

	struct CPlex* m_pBlocks;					// 保存用CPlex::Create 函数申请的内存块的首地址
	int m_nBlockSize;							// 指定每个内存块可以容纳多少个CAssoc 结构
	CAssoc* m_pFreeList;						// 预留空间中没有被使用的CAssoc 结构组成的链中第一个关联的指针
	int m_nCount;								// 记录了程序一共使用了多少个CAssoc 结构，即关联的个数
	CAssoc* NewAssoc();							// 为一个新的CAssoc 结构提供空间，相当于使用new 操作符
	void FreeAssoc(CAssoc* pAssoc);				// 释放一个CAssoc 结构占用的空间，相当于使用delete 操作符
	CAssoc* GetAssocAt(void* key, UINT& nHash) const;	// 寻找键key 所在的关联
public:
	CMapPtrToPtr(int nBlockSize = 10);
	~CMapPtrToPtr();

	int GetCount() const;
	BOOL IsEmpty() const;
	BOOL Lookup(void* key, void*& rValue);
	void*& operator[](void* key);
	void SetAt(void* key, void* newValue);
	BOOL RemoveKey(void* key);
	void RemoveAll();
	UINT GetHashTableSize() const;
	void InitHashTable(UINT nHashSize, BOOL bAllocNow = TRUE);
	UINT HashKey(void* key) const;

public:
};


