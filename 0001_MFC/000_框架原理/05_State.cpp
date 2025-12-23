#include"04_afx.h"
#include"05_State.h"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					 AFX_MODULE_STATE : 模块信息

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// ★★★★★ : 必须在theApp前面初始化,否则theApp设置的全局指针就被覆盖成0了.
AFX_MODULE_STATE _afxBaseModuleState;	// 主模块信息

AFX_MODULE_STATE* AfxGetAppModuleState()
{
	return &_afxBaseModuleState;
}

AFX_MODULE_STATE* AfxGetModuleState()
{
	_AFX_THREAD_STATE* pState = _afxThreadState.GetData();
	AFX_MODULE_STATE* pResult;
	if (pState->m_pModuleState != NULL)
		pResult = pState->m_pModuleState;
	else
		pResult = AfxGetAppModuleState();
	ASSERT(pResult != NULL);
	return pResult;
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					 _AFX_THREAD_STATE : 线程信息

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

_AFX_THREAD_STATE* AfxGetThreadState() // afxstate.cpp 文件
{
	return _afxThreadState.GetData();
}
THREAD_LOCAL(_AFX_THREAD_STATE, _afxThreadState);




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					 AFX_MODULE_THREAD_STATE : 模块线程信息

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// 版本1
//AFX_MODULE_THREAD_STATE* AfxGetModuleThreadState()
//{
//	return _afxModuleThreadState.GetData();
//}
//THREAD_LOCAL(AFX_MODULE_THREAD_STATE, _afxModuleThreadState)


// 版本2
AFX_MODULE_THREAD_STATE* AfxGetModuleThreadState()
{
	return AfxGetModuleState()->m_thread.GetData();
}
THREAD_LOCAL(AFX_MODULE_THREAD_STATE, _afxModuleThreadState)


