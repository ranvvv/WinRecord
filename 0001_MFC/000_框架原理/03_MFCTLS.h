#pragma once
#include<windows.h>
#include"01_CSimpleList.h"
#include"02_NoTrack.h"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CSlotData  : TLS槽标记

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#define SLOT_USED 0x01 

struct CSlotData
{
	DWORD dwFlags;			// 槽的使用标志（被分配/未被分配）
	HINSTANCE hInst;		// 占用此槽的模块句柄
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CThreadData  : 线程自己的TLS数据区

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

struct CThreadData : public CNoTrackObject
{
	CThreadData* pNext;		// CSimpleList 类要使用此成员
	int nCount;				// 数组容量
	LPVOID* pData;			// TSL数据区数组
};


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CThreadSlotData  : 自定义TLS管理器	

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


class CThreadSlotData
{
public:
	DWORD m_tlsIndex;						// 用来访问系统TLS数据的索引,自定义TLS建立在系统TLS之上
	int m_nAlloc;							// 有多少个槽
	int m_nRover;							// 当前可能为空的槽, 为了提高分配效率
	int m_nMax;								// CThreadData 结构中pData 指向的数组的大小
	CSlotData* m_pSlotData;					// 槽状态标记

	CTypedSimpleList<CThreadData*> m_list;	// CThreadData 结构的列表

	CRITICAL_SECTION m_cs;					// 资源锁

	CThreadSlotData();
	void* operator new(size_t, void* p);					// 重载new操作符,将CThreadSloatData设置为全局单利对象.
	~CThreadSlotData();


	int AllocSlot();										// 分配一个槽
	void FreeSlot(int nSlot);								// 释放一个槽
	void* GetThreadValue(int nSlot);						// 获取一个槽的值
	void SetValue(int nSlot, void* pValue);					// 设置一个槽的值
	void DeleteValues(HINSTANCE hInst, BOOL bAll = FALSE);	// 删除一个或所有槽的值
	void DeleteValues(CThreadData* pData, HINSTANCE hInst);	// 删除一个线程槽的值
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CThreadLocalObject :  线程私有数据

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class CThreadLocalObject
{
public:
	DWORD m_nSlot;			// 本数据在TLS中的索引

	CNoTrackObject* GetData(CNoTrackObject* (__stdcall* pfnCreateObject)());
	CNoTrackObject* GetDataNA();
	~CThreadLocalObject();
};

extern CThreadSlotData* _afxThreadData;


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//						CThreadLocal :  线程私有数据模板类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

template<class TYPE> class CThreadLocal : public CThreadLocalObject
{
public:
	TYPE* GetData()
	{
		TYPE* pData = (TYPE*)CThreadLocalObject::GetData((CNoTrackObject * (__stdcall*)(void)) & CreateObject);
		return pData;
	}
	TYPE* GetDataNA()
	{
		TYPE* pData = (TYPE*)CThreadLocalObject::GetDataNA();
		return pData;
	}
	operator TYPE* ()
	{
		return GetData();
	}
	TYPE* operator->()
	{
		return GetData();
	}
	// 具体实现（Implementation）
public:
	static LPVOID CreateObject()
	{
		return new TYPE;
	}
};


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//						线程TLS数据的宏定义实现   : ★★★★★★

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#define THREAD_LOCAL(A,B) CThreadLocal<A> B;
#define EXTERN_THREAD_LOCAL(A,B) extern CThreadLocal<A> B;
