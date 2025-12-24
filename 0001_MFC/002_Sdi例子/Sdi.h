#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"       // 主符号



class CSdiApp : public CWinApp
{
	DECLARE_MESSAGE_MAP()

public:
	CSdiApp() noexcept;

	virtual BOOL InitInstance();
	afx_msg void OnFileNew();
};


extern CSdiApp theApp;
