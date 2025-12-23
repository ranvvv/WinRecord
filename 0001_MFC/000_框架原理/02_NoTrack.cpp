#include"02_NoTrack.h"
#include<windows.h>



void* CNoTrackObject::operator new(size_t nSize)
{
	// 申请一块带有GMEM_FIXED 和 GMEM_ZEROINIT 标志的内存
	// void* p = ::GlobalAlloc(GPTR, nSize);
	void* p = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nSize);
	return p;
}


void CNoTrackObject::operator delete(void* p)
{
	if (p != NULL)
	{
		// ::GlobalFree(p);
		HeapFree(GetProcessHeap(), 0, p);
	}
}