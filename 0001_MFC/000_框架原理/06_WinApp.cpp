#if 0

#include"03_MFCTLS.h"
#include"08_CWinApp.h"

class CMyApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};

CMyApp theApp; // 应用程序实例对象


BOOL CMyApp::InitInstance()
{
	::MessageBoxA(NULL, "主线程开始执行！", "CMyApp::InitInstance", 0);
	return FALSE; // 不要进入消息循环
}

int CMyApp::ExitInstance()
{
	::MessageBoxA(NULL, "主线程将要退出！", "CMyApp::ExitInstance", MB_OK);
	return 0;
}

#endif

