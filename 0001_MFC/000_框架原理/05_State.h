#pragma once

#pragma once
#include "02_NoTrack.h"
#include "03_MFCTLS.h"

class CWinThread;
class CHandleMap;
struct CRuntimeClass;
class CWinApp;
class CWnd;
class AFX_MODULE_THREAD_STATE;

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					 AFX_MODULE_STATE : 模块信息

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class AFX_MODULE_STATE : public CNoTrackObject
{
public:
	CTypedSimpleList<CRuntimeClass*> m_listClass; // 记录模块中的类信息
	CWinApp* m_pCurrentWinApp; // 当前CWinApp 对象的指针
	HINSTANCE m_hCurrentInstanceHandle; // 当前模块的实例句柄（也就是模块句柄）
	HINSTANCE m_hCurrentResourceHandle; // 包含资源的实例句柄
	// CThreadLocal<AFX_MODULE_THREAD_STATE> m_thread; 
	THREAD_LOCAL(AFX_MODULE_THREAD_STATE, m_thread) // 特定于线程的状态数据
};
AFX_MODULE_STATE* AfxGetAppModuleState();
AFX_MODULE_STATE* AfxGetModuleState();


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					 _AFX_THREAD_STATE : 线程信息

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class _AFX_THREAD_STATE : public CNoTrackObject
{
public:
	AFX_MODULE_STATE* m_pModuleState;
	// 注册窗口类时使用（缓冲区）
	TCHAR m_szTempClassName[96];
	// 创建窗口时使用
	CWnd* m_pWndInit; // 正在初始化的CWnd 对象的指针
	HHOOK m_hHookOldCbtFilter; // 钩子句柄
	tagMSG m_lastSendMsg;	// 当前消息

};
EXTERN_THREAD_LOCAL(_AFX_THREAD_STATE, _afxThreadState);
_AFX_THREAD_STATE* AfxGetThreadState();


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					 AFX_MODULE_THREAD_STATE : 模块线程信息

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// AFX_MODULE_THREAD_STATE;版本1 
//class AFX_MODULE_THREAD_STATE : public CNoTrackObject
//{
//public:
//	CWinThread* m_pCurrentWinThread;	// 指向当前线程对象(CWinThread 对象)的指针
//};
//
//EXTERN_THREAD_LOCAL(AFX_MODULE_THREAD_STATE, _afxModuleThreadState)
//AFX_MODULE_THREAD_STATE* AfxGetModuleThreadState();


// AFX_MODULE_THREAD_STATE;版本2
class AFX_MODULE_THREAD_STATE : public CNoTrackObject
{
public:
	CWinThread* m_pCurrentWinThread;		// 指向当前线程对象(CWinThread 对象)的指针
	CHandleMap* m_pmapHWND;					// 窗口句柄映射
	CHandleMap* m_pmapHDC;					// 设备环境句柄映射

};
EXTERN_THREAD_LOCAL(AFX_MODULE_THREAD_STATE, _afxModuleThreadState)
AFX_MODULE_THREAD_STATE* AfxGetModuleThreadState();

