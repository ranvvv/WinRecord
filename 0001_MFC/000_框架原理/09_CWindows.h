#pragma once

#pragma once
#include"04_Afx.h"
#include"07_CPlex.h"

class CObject;
class CHandleMap
{
private:
	CMapPtrToPtr m_permanentMap;
public:
	CObject* LookupPermanent(HANDLE h);				// 查找句柄对应的C++对象指针
	void SetPermanent(HANDLE h, CObject* permOb);	// 设置句柄对应的C++对象指针
	void RemoveHandle(HANDLE h);					// 移除映射表中指定项
	CObject* FromHandle(HANDLE h);					// 这里同LookupPermanent 函数
};

__inline CObject* CHandleMap::LookupPermanent(HANDLE h)
{
	return (CObject*)m_permanentMap[h];
}

__inline void CHandleMap::SetPermanent(HANDLE h, CObject* permOb)
{
	m_permanentMap[h] = permOb;
}

__inline void CHandleMap::RemoveHandle(HANDLE h)
{
	m_permanentMap.RemoveKey(h);
}

__inline CObject* CHandleMap::FromHandle(HANDLE h)
{
	return LookupPermanent(h);
}

class CCmdTarget;
class CWnd;

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					窗口创建机制

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// 窗口类的类型标志
#define AFX_WND_REG (0x0001)			// 使用第1 位
#define AFX_WNDFRAMEORVIEW_REG (0x0002) // 使用第2 位。还可继续使用0x0004、0x0008、0x0010 等
#define AFX_WND TEXT("Wnd") 
#define AFX_WNDFRAMEORVIEW TEXT("FrameOrView") 

// 框架程序注册窗口类时使用的类名，这些变量定义在WINCORE.CPP 文件
extern const TCHAR _afxWnd[];
extern const TCHAR _afxWndFrameOrView[];


CHandleMap* afxMapHWND(BOOL bCreate = FALSE);
LRESULT __stdcall AfxWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
// LRESULT AfxCallWndProc(CWnd* pWnd, HWND hWnd, UINT nMsg, WPARAM wParam = 0, LPARAM lParam = 0);
LRESULT AfxCallWndProc(CWnd* pWnd, HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
LPCTSTR AfxRegisterWndClass(UINT nClassStyle, HCURSOR hCursor, HBRUSH hbrBackground, HICON hIcon);
BOOL AfxEndDeferRegisterClass(LONG fToRegister);
BOOL AfxRegisterClass(WNDCLASS* lpWndClass);
void AfxHookWindowCreate(CWnd* pWnd);
BOOL AfxUnhookWindowCreate();
WNDPROC AfxGetAfxWndProc();
LRESULT __stdcall _AfxCbtFilterHook(int code, WPARAM wParam, LPARAM lParam);


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					消息映射的建立

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


class CCmdTarget;
typedef void (CCmdTarget::* AFX_PMSG)(void);
typedef void (CWnd::* AFX_PMSGW)(void);



enum AfxSig // 函数签名标识
{
	AfxSig_end = 0, // 结尾标识
	AfxSig_vv, // void (void)，比如，void OnPaint()函数
	AfxSig_vw, // void (UINT)，比如，void OnTimer(UINT nIDEvent)函数
	AfxSig_is, // int (LPTSTR)，比如，BOOL OnCreate(LPCREATESTRUCT)函数
};

union MessageMapFunctions
{
	AFX_PMSG pfn;
	void (CWnd::* pfn_vv)(void);
	void (CWnd::* pfn_vw)(UINT);
	int (CWnd::* pfn_is)(LPTSTR);
};

// 一个映射表项
struct AFX_MSGMAP_ENTRY
{
	UINT nMessage;		// 窗口消息
	UINT nCode;			// 控制代码或WM_NOTIFY 通知码
	UINT nID;			// 控件ID，如果为窗口消息其值为0 
	UINT nLastID;		// 一定范围的命令的最后一个命令或控件ID，用于支持组消息映射
	UINT nSig;			// 指定了消息处理函数的类型
	AFX_PMSG pfn;		// 消息处理函数
};

struct AFX_MSGMAP // 继续在AFX_MSGMAP_ENTRY 结构之后定义此结构，_AFXWIN.H 文件中
{
	const AFX_MSGMAP* pBaseMap; // 其基类的消息映射表的地址
	const AFX_MSGMAP_ENTRY* pEntries; // 消息映射项的指针
};


// 消息映射机制声明
#define DECLARE_MESSAGE_MAP() \
private: \
	static const AFX_MSGMAP_ENTRY _messageEntries[]; \
protected: \
	static const AFX_MSGMAP messageMap; \
	virtual const AFX_MSGMAP* GetMessageMap() const; 


// 消息映射机制实现
#define BEGIN_MESSAGE_MAP(theClass, baseClass) \
	const AFX_MSGMAP* theClass::GetMessageMap() const { return &theClass::messageMap; } \
	const AFX_MSGMAP theClass::messageMap = { &baseClass::messageMap, & theClass::_messageEntries[0] }; \
	const AFX_MSGMAP_ENTRY theClass::_messageEntries[] = \
	{ 
#define END_MESSAGE_MAP() \
	{0, 0, 0, 0, 0, (AFX_PMSG)0} \
	}; 


#define afx_msg 
#define ON_WM_CREATE() \
	{ WM_CREATE, 0, 0, 0, AfxSig_is,(AFX_PMSG)(AFX_PMSGW)(int (CWnd::*)(LPCREATESTRUCT))& OnCreate },
#define ON_WM_PAINT() \
	{ WM_PAINT, 0, 0, 0, AfxSig_vv,(AFX_PMSG)(AFX_PMSGW)(void (CWnd::*)(void)) & OnPaint },
#define ON_WM_CLOSE() \
	{ WM_CLOSE, 0, 0, 0, AfxSig_vv,(AFX_PMSG)(AFX_PMSGW)(int (CWnd::*)(void)) & OnClose },
#define ON_WM_DESTROY() \
	{ WM_DESTROY, 0, 0, 0, AfxSig_vv,(AFX_PMSG)(AFX_PMSGW)(int (CWnd::*)(void)) & OnDestroy },
#define ON_WM_NCDESTROY() \
	{ WM_NCDESTROY, 0, 0, 0, AfxSig_vv,(AFX_PMSG)(AFX_PMSGW)(int (CWnd::*)(void)) & OnNcDestroy },
#define ON_WM_TIMER() \
	{ WM_TIMER, 0, 0, 0, AfxSig_vw,(AFX_PMSG)(AFX_PMSGW)(void (CWnd::*)(UINT))&OnTimer },


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CCmdTarget : 消息处理基类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class CCmdTarget : public CObject
{
public:
	// 运行时类信息
	DECLARE_DYNCREATE(CCmdTarget)
	// 消息映射声明
	DECLARE_MESSAGE_MAP()


	CCmdTarget();
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//					CWnd : 窗口基类

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class CWnd : public CCmdTarget
{
	// 运行时类信息
	DECLARE_DYNCREATE(CWnd)
	// 消息映射声明
	DECLARE_MESSAGE_MAP()
	/*
	DECLARE_MESSAGE_MAP() 对应的宏展开:
	static const AFX_MSGMAP_ENTRY _messageEntries[];
	protected:
		static const AFX_MSGMAP messageMap;
		virtual const AFX_MSGMAP* GetMessageMap() const;
		*/


		// =================== 消息分发函数
public:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	// =================== 消息处理函数
public:
	//void OnCreate();	// { /* 响应WM_CREAT 消息的代码*/ }
	//void OnPaint();		// { /* 响应WM_PAINT 消息的代码*/ }
	//void OnDestory();	// { /* 响应WM_DESTROY 消息的代码*/ }
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct); // WM_CREATE 消息
	afx_msg void OnPaint();
	afx_msg void OnClose(); // WM_CLOSE 消息
	afx_msg void OnDestroy(); // WM_DESTROY 消息
	afx_msg void OnNcDestroy(); // WM_NCDESTROY 消息
	afx_msg void OnTimer(UINT nIDEvent); // WM_TIMER 消息


public:
	HWND m_hWnd;
	WNDPROC m_pfnSuper;	// 默认消息处理函数的地址

	CWnd();
	virtual ~CWnd();
	operator HWND() const;
	HWND GetSafeHwnd();
	static CWnd* FromHandle(HWND hWnd);
	static CWnd* FromHandlePermanent(HWND hWnd);
	BOOL Attach(HWND hWndNew);
	HWND Detach();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	friend LRESULT AfxCallWndProc(CWnd*, HWND, UINT, WPARAM, LPARAM);
	virtual WNDPROC* GetSuperWndProcAddr();
	// 对消息进行默认处理
	LRESULT Default();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	// 挂钩消息的实现
	friend LRESULT __stdcall _AfxCbtFilterHook(int, WPARAM, LPARAM);
	// 为创建各种子窗口设置
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL);
	// 最终创建窗口的代码
	BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight,
		HWND hWndParent, HMENU nIDorHMenu, LPVOID lpParam = NULL);
	// 创建前置操作
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PostNcDestroy();
	virtual void PreSubclassWindow();
};
















