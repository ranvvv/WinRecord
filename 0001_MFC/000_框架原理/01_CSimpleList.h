#pragma once
#include<windows.h>

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CSimpleList 链表类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class CSimpleList
{
public:
	void* m_pHead;						// 头节点
	size_t m_nNextOffset;				// next偏移,节点遍历通过这个

	CSimpleList(int nNextOffset = 0);

	void Construct(int nNextOffset);	// 设置m_nNextOffset
	void* GetHead() const;			
	void* GetNext(void* p) const;		
	void** GetNextPtr(void* p) const;	
	BOOL IsEmpty() const;				// 是否为空
	void AddHead(void* p);	
	BOOL Remove(void* p);				// 移除p节点
	void RemoveAll();			
};



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CTypeSimpleList  链表类模板封装

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

template<class TYPE> class CTypedSimpleList : public CSimpleList
{
public:
	CTypedSimpleList(int nNextOffset = 0) : CSimpleList(nNextOffset)
	{
	}

	void AddHead(TYPE p)
	{
		CSimpleList::AddHead((void*)p);
	}

	TYPE GetHead()
	{
		return (TYPE)CSimpleList::GetHead();
	}

	TYPE GetNext(TYPE p)
	{
		return (TYPE)CSimpleList::GetNext(p);
	}

	BOOL Remove(TYPE p)
	{
		return CSimpleList::Remove(p);
	}

	operator TYPE() // 直接引用类的对象是会调用此函数，见下面的例子
	{
		return (TYPE)CSimpleList::GetHead();
	}

};

