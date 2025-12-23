#pragma warning(disable:4291)
#include"03_MFCTLS.h"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CThreadSlotData  : 自定义TLS管理器	

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


BYTE __afxThreadData[sizeof(CThreadSlotData)];	// _afxThreadData变量空间.
CThreadSlotData* _afxThreadData;				// __afxThreadData的指针


void* CThreadSlotData::operator new(size_t, void* p)
{
	return p;
}

CThreadSlotData::CThreadSlotData()
{
	m_list.Construct(offsetof(CThreadData, pNext)); // 初始化CTypedSimpleList 对象
	m_nMax = 0;								// 需要为线程私有数据分配的槽的最大个数.
	m_nAlloc = 0;							// 当前槽个数.
	m_nRover = 1;							// 我们假定Slot1 还未被分配（第一个槽（Slot0）总是保留下来不被使用）
	m_pSlotData = NULL;
	m_tlsIndex = ::TlsAlloc();				// 使用系统的TLS 申请一个索引
	::InitializeCriticalSection(&m_cs);		// 初始化关键段变量
}

// 分配一个槽（SLOT）
int CThreadSlotData::AllocSlot()
{
	::EnterCriticalSection(&m_cs); // 进入临界区
	int nAlloc = m_nAlloc;
	int nSlot = m_nRover;
	if (nSlot >= nAlloc || m_pSlotData[nSlot].dwFlags & SLOT_USED)
	{
		// 搜索m_pSlotData，查找空槽（SLOT）
		for (nSlot = 1; nSlot < nAlloc && m_pSlotData[nSlot].dwFlags & SLOT_USED; nSlot++);
		
		// 如果不存在空槽，申请更多的空间
		if ( nSlot >= nAlloc )
		{
			// 增加全局数组的大小，分配或再分配内存以创建新槽
			int nNewAlloc = nAlloc + 32;
			// HGLOBAL hSlotData;
			if (m_pSlotData == NULL) // 第一次使用
			{
				// hSlotData = ::GlobalAlloc(GMEM_MOVEABLE, nNewAlloc * sizeof(CSlotData));
				m_pSlotData = (CSlotData*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nNewAlloc * sizeof(CSlotData));
			}
			else
			{
				// hSlotData = ::GlobalHandle(m_pSlotData);
				// ::GlobalUnlock(hSlotData);
				// hSlotData = ::GlobalReAlloc(hSlotData,nNewAlloc * sizeof(CSlotData), GMEM_MOVEABLE);
				m_pSlotData = (CSlotData*)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_pSlotData, nNewAlloc * sizeof(CSlotData));
			}
			// CSlotData* pSlotData = (CSlotData*)::GlobalLock(hSlotData);
			// 将新申请的空间初始化为0 
			// memset(pSlotData + m_nAlloc, 0, (nNewAlloc - nAlloc) * sizeof(CSlotData));
			m_nAlloc = nNewAlloc;
			// m_pSlotData = pSlotData;
		}
	}
	// 调整m_nMax 的值，以便为各线程的私有数据分配内存
	if (nSlot >= m_nMax)
		m_nMax = nSlot + 1;
	m_pSlotData[nSlot].dwFlags |= SLOT_USED; // 标志该SLOT 为已用
	m_nRover = nSlot + 1; // 更新m_nRover 的值(我们假设下一个槽未被使用) 
	::LeaveCriticalSection(&m_cs);
	return nSlot; // 返回的槽号可以被FreeSlot, GetThreadValue, SetValue 函数使用了
}

// 设置数据
void CThreadSlotData::SetValue(int nSlot, void* pValue)
{
	// 通过系统TLS,获取本线程的私有数据结构
	CThreadData* pData = (CThreadData*)::TlsGetValue(m_tlsIndex);  
	// 为线程私有数据申请内存空间
	if ((pData == NULL || nSlot >= pData->nCount) && pValue != NULL)
	{ 
		// pData 的值为空，表示该线程第一次访问线程私有数据
		if (pData == NULL)
		{
			pData = new CThreadData;
			pData->nCount = 0;
			pData->pData = NULL;
			// 将新申请的内存的地址添加到全局列表中
			::EnterCriticalSection(&m_cs);
			m_list.AddHead(pData);
			::LeaveCriticalSection(&m_cs);
		}
		// pData->pData 指向真正的线程私有数据，
		// 下面的代码将私有数据占用的空间增长到m_nMax 指定的大小
		if (pData->pData == NULL)
			// pData->pData = (void**)::GlobalAlloc(LMEM_FIXED, m_nMax * sizeof(LPVOID));
			pData->pData = (void**)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_nMax * sizeof(LPVOID));
		else
			// pData->pData = (void**)::GlobalReAlloc(pData->pData,m_nMax * sizeof(LPVOID), LMEM_MOVEABLE);
			pData->pData = (void**)::HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_pSlotData, m_nMax * sizeof(LPVOID));
		// 将新申请的内存初始话为0 
		// memset(pData->pData + pData->nCount, 0,(m_nMax - pData->nCount) * sizeof(LPVOID));
		pData->nCount = m_nMax;
		::TlsSetValue(m_tlsIndex, pData);
	}
	// 设置线程私有数据的值
	pData->pData[nSlot] = pValue;
}

// 获取数据
inline void* CThreadSlotData::GetThreadValue(int nSlot)
{
	CThreadData* pData = (CThreadData*)::TlsGetValue(m_tlsIndex);
	if (pData == NULL || nSlot >= pData->nCount)
		return NULL;
	return pData->pData[nSlot];
}

// 释放一个槽（SLOT）所占用的空间
void CThreadSlotData::FreeSlot(int nSlot)
{
	::EnterCriticalSection(&m_cs);
	// 删除所有线程中的数据
	CThreadData* pData = m_list;
	while (pData != NULL)
	{
		if (nSlot < pData->nCount)
		{
			delete (CNoTrackObject*)pData->pData[nSlot];
			pData->pData[nSlot] = NULL;
		}
		pData = pData->pNext;
	}
	// 将此槽号标识为未被使用
	m_pSlotData[nSlot].dwFlags &= ~SLOT_USED;
	::LeaveCriticalSection(&m_cs);
}

// 删除所有线程的私有数据所占用的空间
void CThreadSlotData::DeleteValues(HINSTANCE hInst, BOOL bAll)
{
	::EnterCriticalSection(&m_cs);
	if (!bAll)
	{ 
		// 仅仅删除当前线程的线程局部存储占用的空间
		CThreadData* pData = (CThreadData*)::TlsGetValue(m_tlsIndex);
		if (pData != NULL)
			DeleteValues(pData, hInst);
	}
	else
	{ 
		// 删除所有线程的线程局部存储占用的空间
		CThreadData* pData = m_list.GetHead();
		while (pData != NULL)

		{
			CThreadData* pNextData = pData->pNext;
			DeleteValues(pData, hInst);
			pData = pNextData;
		}
	}
	::LeaveCriticalSection(&m_cs);
}

void CThreadSlotData::DeleteValues(CThreadData* pData, HINSTANCE hInst)
{ // 释放表中的每一个元素
	BOOL bDelete = TRUE;
	for (int i = 1; i < pData->nCount; i++)
	{
		if (hInst == NULL || m_pSlotData[i].hInst == hInst)
		{ // hInst 匹配，删除数据
			delete (CNoTrackObject*)pData->pData[i];
			pData->pData[i] = NULL;
		}
		else
		{ // 还有其他模块在使用，不要删除数据
			if (pData->pData[i] != NULL)
				bDelete = FALSE;
		}
	}
	if (bDelete)
	{ // 从列表中移除
		::EnterCriticalSection(&m_cs);
		m_list.Remove(pData);
		::LeaveCriticalSection(&m_cs);
		// ::LocalFree(pData->pData);
		HeapFree(GetProcessHeap(), 0, pData->pData);
		delete pData;
		// 清除TLS 索引，防止重用
		::TlsSetValue(m_tlsIndex, NULL);
	}
}

CThreadSlotData::~CThreadSlotData()
{
	CThreadData* pData = m_list;
	while (pData != NULL)
	{
		CThreadData* pDataNext = pData->pNext;
		DeleteValues(pData, NULL);
		pData = pData->pNext;
	}
	if (m_tlsIndex != (DWORD)-1)
		::TlsFree(m_tlsIndex);
	if (m_pSlotData != NULL)
	{
		HGLOBAL hSlotData = ::GlobalHandle(m_pSlotData);
		// ::GlobalUnlock(hSlotData);
		// ::GlobalFree(m_pSlotData);
		HeapFree(GetProcessHeap(), 0, m_pSlotData);
	}
	::DeleteCriticalSection(&m_cs);
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CThreadLocalObject :  线程私有数据

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


CNoTrackObject* CThreadLocalObject::GetData(CNoTrackObject* (__stdcall* pfnCreateObject)())
{
	// 首次使用,分配槽号
	if (m_nSlot == 0)
	{
		// CThreadSlotData TLS 管理类未初始化,就进行初始化.
		if (_afxThreadData == NULL)
			_afxThreadData = new(__afxThreadData) CThreadSlotData;

		m_nSlot = _afxThreadData->AllocSlot(); // 分配一个槽号
	}

	CNoTrackObject* pValue = (CNoTrackObject*)_afxThreadData->GetThreadValue(m_nSlot);
	if (pValue == NULL) // 为创建数据
	{ 
		// 创建一个对象，此对象的成员会被初始化为0 
		pValue = pfnCreateObject();
		// 使用线程私有数据保存新创建的对象
		_afxThreadData->SetValue(m_nSlot, pValue);
	}

	return pValue;
}


CNoTrackObject* CThreadLocalObject::GetDataNA()
{
	if (m_nSlot == 0 || _afxThreadData == 0)
		return NULL;
	return (CNoTrackObject*)_afxThreadData->GetThreadValue(m_nSlot);
}


CThreadLocalObject::~CThreadLocalObject()
{
	if (m_nSlot != 0 && _afxThreadData != NULL)
		_afxThreadData->FreeSlot(m_nSlot);
	m_nSlot = 0;
}

