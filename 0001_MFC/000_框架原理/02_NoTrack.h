#pragma once

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CNoTrackObject 自定义内存分配.

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class CNoTrackObject
{
public:
	void* operator new(size_t nSize);
	void operator delete(void*);
	virtual ~CNoTrackObject() {}
};

