#include<stdio.h>
#include"04_afx.h"


#if 0

// 1  : 定义一个类CPerson，继承自CObject
class CPerson : public CObject
{
public:
	virtual CRuntimeClass* GetRuntimeClass() const
	{
		return (CRuntimeClass*)&classCPerson;
	}
	static const CRuntimeClass classCPerson;
};
const CRuntimeClass CPerson::classCPerson = { "CPerson", sizeof(CPerson), 0xffff, NULL, (CRuntimeClass*)&CObject::classCObject, NULL };

int main()
{
	CObject* pMyObject = new CPerson;

	// 判断对象pMyObject 是否属于CPerson 类或者此类的派生类
	if (pMyObject->IsKindOf(RUNTIME_CLASS(CPerson)))
	{
		// RUNTIME_CLASS(CPerson)宏被展开后相当于((CRuntimeClass*)&CPerson::classCPerson) 
		CPerson* pMyPerson = (CPerson*)pMyObject;
		printf("a CPersion1 Object!\n");
		delete pMyPerson;
	}
	else
	{
		delete pMyObject;
	}
}
#endif 


#if 0

// 2  :  实现自动创建
class CPerson : public CObject
{
public:
	virtual CRuntimeClass* GetRuntimeClass() const
	{
		return (CRuntimeClass*)&classCPerson;
	}

	static const CRuntimeClass classCPerson;

	// ★ ★ 自动创建
	static CObject* __stdcall  CreateObject()
	{
		return new CPerson;
	}
};

// 注册自动创建函数
const CRuntimeClass CPerson::classCPerson = { "CPerson", sizeof(CPerson), 0xffff, &CPerson::CreateObject/*添加到这里*/, (CRuntimeClass*)&CObject::classCObject, NULL };

int  main()
{
	CRuntimeClass* pRuntimeClass = RUNTIME_CLASS(CPerson);
	// 取得了pRuntimeClass 指针，不用知道类的名字就可以创建该类
	CObject* pObject = pRuntimeClass->CreateObject();
	if (pObject != NULL && pObject->IsKindOf(RUNTIME_CLASS(CPerson)))
	{
		printf("a CPersion1 Object!\n");
		delete pObject;
	}
}
#endif 


#if 0

// 3  :  宏实现
class CPerson : public CObject
{
	DECLARE_DYNCREATE(CPerson)
};
IMPLEMENT_DYNCREATE(CPerson, CObject)
int  main()
{
	CRuntimeClass* pRuntimeClass = RUNTIME_CLASS(CPerson);
	CObject* pObject = pRuntimeClass->CreateObject();
	if (pObject != NULL && pObject->IsKindOf(RUNTIME_CLASS(CPerson)))
	{
		printf("a CPersion1 Object!\n");
		delete pObject;
	}
}
#endif
