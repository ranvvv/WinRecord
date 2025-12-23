#pragma once
#include<windows.h>

class CObject;


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CRuntimeClass : 运行时信息

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

struct CRuntimeClass
{
	LPCSTR m_lpszClassName;										// 类的名称
	int m_nObjectSize;											// 类的大小
	UINT m_wSchema;												// 类的版本号
	CObject* (__stdcall* m_pfnCreateObject)();					// 创建类的函数的指针
	CRuntimeClass* m_pBaseClass;								// 其基类中CRuntimeClass 结构的地址
	CObject* __stdcall CreateObject();							// 动态创建实例
	BOOL IsDerivedFrom(const CRuntimeClass* pBaseClass) const;	// 判断是否派生自某个类
	CRuntimeClass* m_pNextClass;								// 将所有CRuntimeClass 对象用简单链表连在一起
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CObject : 基类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class CObject
{
public:
	virtual ~CObject();

	virtual CRuntimeClass* GetRuntimeClass() const;				// 返回类的运行时类信息
	BOOL IsKindOf(const CRuntimeClass* pClass) const;			// 判断是否派生自某个类
	static const CRuntimeClass classCObject;					// 标识类的静态成员
};


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					运行时识别+动态创建  宏定义

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// RUNTIME_CLASS 宏用来取得class_name 类中CRuntimeClass 结构的地址
#define RUNTIME_CLASS(class_name) ((CRuntimeClass*)&class_name::class##class_name)


// ========== 声明运行时类信息

// 运行时结构支持
#define DECLARE_DYNAMIC(class_name) \
public: \
	static const CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const;

// 运行时结构支持+动态创建
#define DECLARE_DYNCREATE(class_name) \
			DECLARE_DYNAMIC(class_name) \
			static CObject* __stdcall CreateObject();


// ========== 实现运行时类信息

// 运行时结构支持
#define IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew) \
	const CRuntimeClass class_name::class##class_name = { #class_name, sizeof(class class_name), wSchema, pfnNew,  RUNTIME_CLASS(base_class_name), NULL }; \
	CRuntimeClass* class_name::GetRuntimeClass() const { return RUNTIME_CLASS(class_name); } 

// 运行时结构支持+默认参数
#define IMPLEMENT_DYNAMIC(class_name, base_class_name) \
		  IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, NULL)

// 运行时结构支持+默认参数+动态创建
#define IMPLEMENT_DYNCREATE(class_name, base_class_name) \
			CObject* __stdcall class_name::CreateObject(){ return (CObject*)(new class_name); } \
			IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF,class_name::CreateObject)

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					调试支持

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$



#ifdef _WIN64
#define AfxDebugBreak() __debugbreak();
#else
#define AfxDebugBreak() _asm int 3 ;
#endif


#ifdef _DEBUG 
void __cdecl AfxTrace(LPCTSTR lpszFormat, ...);
#define TRACE ::AfxTrace 
#define ASSERT(f) \
		if (!(f)) AfxDebugBreak();
#define VERIFY(f) ASSERT(f) 
#else // _DEBUG 
#define ASSERT(f) ((void)0) 
#define VERIFY(f) ((void)(f)) 
inline void __cdecl AfxTrace(LPCTSTR, ...) {}
#define TRACE (void)0 
#endif 

