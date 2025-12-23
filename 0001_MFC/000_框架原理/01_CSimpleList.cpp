#include "01_CSimpleList.h"

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CSimpleList 链表类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


CSimpleList::CSimpleList(int nNextOffset)
{
	m_pHead = NULL;
	m_nNextOffset = nNextOffset;
}

void CSimpleList::Construct(int nNextOffset)
{
	m_nNextOffset = nNextOffset;
}


BOOL CSimpleList::IsEmpty() const
{
	return m_pHead == NULL;
}


void CSimpleList::RemoveAll()
{
	m_pHead = NULL;
}


void* CSimpleList::GetHead() const
{
	return m_pHead;
}


void* CSimpleList::GetNext(void* preElement) const
{
	return *GetNextPtr(preElement);
}


void** CSimpleList::GetNextPtr(void* p) const
{
	return (void**)((BYTE*)p + m_nNextOffset);
}


void CSimpleList::AddHead(void* p)
{
	*GetNextPtr(p) = m_pHead;
	m_pHead = p;
}


BOOL CSimpleList::Remove(void* p) 
{
	if (p == NULL)
		return FALSE;

	BOOL bResult = FALSE;	// 假设移除失败
	if (p == m_pHead)
	{
		// 要移除头元素
		m_pHead = *GetNextPtr(p);
		bResult = TRUE;
	}
	else
	{
		// 试图在表中查找要移除的元素
		void* pTest = m_pHead;
		while (pTest != NULL && *GetNextPtr(pTest) != p)
			pTest = *GetNextPtr(pTest);
		// 如果找到，就将元素移除
		if (pTest != NULL)
		{
			*GetNextPtr(pTest) = *GetNextPtr(p);
			bResult = TRUE;
		}
	}
	return bResult;
}
