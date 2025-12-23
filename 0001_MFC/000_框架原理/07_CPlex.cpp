#include"07_CPlex.h"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					 CPlex   :  数据区

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void* CPlex::data()
{
	return this + 1;  // 这里是真正的数据区，BYTE data[maxNum*elementSize]; 
}

CPlex* CPlex::Create(CPlex*& pHead, UINT nMax, UINT cbElement)
{
	CPlex* p = (CPlex*)new BYTE[sizeof(CPlex) + nMax * cbElement];		// 将新增加的内存块添加到链中，并将其地址做为首地址
	p->pNext = pHead;
	pHead = p;		// 以相反方向添加数据项的方式大大减化了程序设计
	return p;
}

void CPlex::FreeDataChain()
{ 
	// 以当前内存块的地址为首地址
	CPlex* p = this;
	// 释放链中所有内存块占用的内存
	while (p != NULL)
	{
		BYTE* pBytes = (BYTE*)p;
		CPlex* pNext = p->pNext;
		delete[] pBytes;
		p = pNext;
	}
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					 CMapPtrToPtr   :  key=>value

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

CMapPtrToPtr::CMapPtrToPtr(int nBlockSize)
{
	m_pHashTable = NULL;
	m_nHashTableSize = 17; // 默认大小
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
	m_pFreeList = NULL;
	m_nCount = 0;
}

CMapPtrToPtr::~CMapPtrToPtr()
{
	RemoveAll();
}

int CMapPtrToPtr::GetCount() const
{
	return m_nCount;
}

BOOL CMapPtrToPtr::IsEmpty() const
{
	return m_nCount == 0;
}

void CMapPtrToPtr::SetAt(void* key, void* newValue)
{
	(*this)[key] = newValue;
}

UINT CMapPtrToPtr::GetHashTableSize() const
{
	return m_nHashTableSize;
}

CMapPtrToPtr::CAssoc* CMapPtrToPtr::NewAssoc()
{
	// 预留的空间已经被使用完
	if (m_pFreeList == NULL)
	{
		// 向 m_pBlocks 指向的链中添加一个新的内存块。m_nBlockSize 的值可以由用户指定
		CPlex* newBlock = CPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CAssoc));
		// 将新内存块中各CAssoc 结构添加到m_pFreeList 指向的链中（空闲链）
		CAssoc* pAssoc = (CAssoc*)newBlock->data();
		pAssoc += m_nBlockSize - 1; // 此时pAssoc 指向内存块中最后一个CAssoc 结构
		for (int i = m_nBlockSize - 1; i >= 0; i--, pAssoc--)
		{ // 将pAssoc 做为链的首地址添加到空闲链中（还是以相反的顺序向链中添加元素）
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	// 从空闲链中取出一个元素pAssoc 
	CAssoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++; // 又多使用了一个CAssoc 结构
	// 初始化新关联的值
	pAssoc->key = 0;
	pAssoc->value = 0;
	return pAssoc;
}

void CMapPtrToPtr::FreeAssoc(CAssoc* pAssoc)
{
	// 将要释放的关联做为链的首地址添加到空闲链中（以相反的顺序）
	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--; // 释放了一个CAssoc 结构
	// 如果全部的关联都没被使用，就释放所有的内存空间，包括CPlex::Create 函数申请的内存块
	if (m_nCount == 0)
		RemoveAll(); // 此函数会释放所有内存空间，待会儿我们再讨论
}

inline UINT CMapPtrToPtr::HashKey(void* key) const
{
	return ((UINT)(void*)(DWORD)key) >> 4;
}

void CMapPtrToPtr::InitHashTable(UINT nHashSize, BOOL bAllocNow)
{
	if (m_pHashTable != NULL)
	{ // 释放哈希表
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}
	if (bAllocNow)
	{ // 为哈希表申请空间
		m_pHashTable = new CAssoc * [nHashSize];
		memset(m_pHashTable, 0, sizeof(CAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

CMapPtrToPtr::CAssoc* CMapPtrToPtr::GetAssocAt(void* key, UINT& nHash) const
{
	// 计算包含key 的项在表中的位置
	nHash = HashKey(key) % m_nHashTableSize;
	if (m_pHashTable == NULL)
		return NULL;
	// 在以m_pHashTable[nHash]为头指针的链表中查找
	CAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (pAssoc->key == key)
			return pAssoc;
	}
	return NULL;
}

BOOL CMapPtrToPtr::Lookup(void* key, void*& rValue)
{
	UINT nHash;
	CAssoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE; // 没有在映射中
	rValue = pAssoc->value;
	return TRUE;
}

void*& CMapPtrToPtr::operator [] (void* key)
{
	UINT nHash;
	CAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);
		// 既然映射中没有用户指定的项，我们就添加一个新的关联
		pAssoc = NewAssoc();
		pAssoc->key = key;
		// 将新的关联放入哈希表中（放在表头，不是表尾）
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->value; // 返回value 的引用
}

BOOL CMapPtrToPtr::RemoveKey(void* key)
{
	if (m_pHashTable == NULL)
		return FALSE; // 表中什么也没有
	CAssoc** ppAssocPre; // 记录要删除的项的地址的变量的地址（如果存在的话，我们会改变此地址处的值）
	ppAssocPre = &m_pHashTable[HashKey(key) % m_nHashTableSize];
	CAssoc* pAssoc;
	for (pAssoc = *ppAssocPre; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (pAssoc->key == key)
		{ // 移除pAssoc 指向的项
			*ppAssocPre = pAssoc->pNext; // 从表中移除
			FreeAssoc(pAssoc); // 释放内存
			return TRUE;
		}
		ppAssocPre = &pAssoc->pNext;
	}
	return FALSE; // 没有找到
}



void CMapPtrToPtr::RemoveAll()
{
	if (m_pHashTable != NULL)
	{ // 释放哈希表
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}