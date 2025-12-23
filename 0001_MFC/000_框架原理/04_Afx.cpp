#include"04_afx.h"
#include<stdio.h>
#include<tchar.h>

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CRuntimeClass : 运行时信息

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

CObject* CRuntimeClass::CreateObject()
{
	if (m_pfnCreateObject == NULL)
		return NULL;
	return (*m_pfnCreateObject)(); // 调用创建类的函数
}

BOOL CRuntimeClass::IsDerivedFrom(const CRuntimeClass* pBaseClass) const
{
	const CRuntimeClass* pClassThis = this;
	while (pClassThis != NULL)
	{
		if (pClassThis == pBaseClass) // 判断标识类的CRuntimeClass 的首地址是否相同
			return TRUE;
		pClassThis = pClassThis->m_pBaseClass;
	}
	return FALSE; // 查找到了继承结构的顶层，没有一个匹配
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CObject : 基类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


const struct CRuntimeClass CObject::classCObject = { "CObject"/*类名*/, sizeof(CObject)/*大小*/, 0xffff/*无版本号*/,NULL/*不支持动态创建*/, NULL/*没有基类*/, NULL };

CObject::~CObject()
{

}

CRuntimeClass* CObject::GetRuntimeClass() const
{
	return RUNTIME_CLASS(CObject); // 展开 :  return ((CRuntimeClass*)&(CObject::classCObject));
}

BOOL CObject::IsKindOf(const CRuntimeClass* pClass) const
{
	CRuntimeClass* pClassThis = GetRuntimeClass();
	return pClassThis->IsDerivedFrom(pClass);
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					调试支持

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#ifdef _DEBUG 
void __cdecl AfxTrace(LPCTSTR lpszFormat, ...)
{ // 格式化我们得到的数据
	va_list args;
	va_start(args, lpszFormat);
	TCHAR szBuffer[512];
	// _vsnprintf(szBuffer, 512, lpszFormat, args);
	_vstprintf(szBuffer, 512, lpszFormat, args);
	// 输出到调试器
	::OutputDebugString(szBuffer);
	va_end(args);
}
#endif // _DEBUG

