#include"../common.h"

#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include<math.h>

// 1. GDI基础
// 2. 文本
// 3. 点线
// 4. 填充
// 5. 映射模式
// 6. 区域


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 1. GDI基础

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s1()
{
	// WM_PAINT消息中的HDC
	HDC hdc;
	HWND hwnd;

	// WM_PAINT消息处理函数中获取设备环境,并绘图
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);	// 将客户区变成了有效区域
	EndPaint(hwnd, &ps);

	// 窗口客户区设备环境获取,不包括非客户区(标题栏,边框等区域)
	hdc = GetDC(hwnd);				// 不会将客户区变成有效区域.
	ReleaseDC(hwnd, hdc);

	// 整个窗口设备环境获取,包括非客户区(标题栏,边框等区域)
	hdc = GetWindowDC(hwnd);		// 由WM_NCPAINT消息处理非客户区绘制
	ReleaseDC(hwnd, hdc);

	// 整个屏幕设备环境获取
	hdc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);	// 创建设备环境
	DeleteDC(hdc);	// 删除设备环境

	// 内存设备环境  兼容某一设备
	hdc = CreateCompatibleDC(hdc);	// 创建兼容设备环境
	DeleteDC(hdc);

	// 设备环境保存和恢复
	int dcId = SaveDC(hdc);		// 保存设备环境
	RestoreDC(hdc, dcId);		// 恢复设备环境

	// 窗口无效区域处理
	InvalidateRect(hwnd, NULL, TRUE);	// 设置一块区域无效,插入一个WM_PAINT消息通知窗口重绘  
										// param3 : TRUE:WM_PAINT中BeginPaint处理时发送WM_ERASEBKGND进行窗口重绘,FALSE:这个无效区域不重绘
	ValidateRect(hwnd, NULL);			// 手动使区域有效化.

	RECT rect;
	GetUpdateRect(hwnd, &rect, TRUE/* 发送背景擦除消息:WM_ERASEBKGND */);		// 获取无效矩形区域

	UpdateWindow(hwnd);	// 立即重绘窗口,WM_PAINT


	// 获取设备环境能力
	GetDeviceCaps(hdc, HORZSIZE);

	int length = GetDeviceCaps(hdc, HORZRES);					// 设备像素宽度
	int height = GetDeviceCaps(hdc, VERTRES);					// 设备像素高度
	mDbgPrint(TEXT("length:%d  height:%d\n"), length, height);
	length = GetDeviceCaps(hdc, HORZSIZE);						// 设备毫米宽度
	height = GetDeviceCaps(hdc, VERTSIZE);						// 设备毫米高度
	mDbgPrint(TEXT("length:%d  height:%d\n"), length, height);
	int dpi = GetDeviceCaps(hdc, LOGPIXELSX);					// 逻辑像素DPI
	printf("dpi:%d\n", dpi);


	// 有2种分辨率概念:
	//		每英寸多少像素点
	//		像素总数

	// 像素和磅:
	//		1磅 = 1/72英寸    
	//		分辨率 = 每英寸多少像素点
	//		1磅对应的像素数 = 分辨率/72

	// 设备的物理尺寸 = 25.4 * (像素总数/分辨率) ; 1英寸 = 25.4毫米  ; 

	// 颜色: RGB(255, 0, 0)


	// 像素转磅
	// GetDeviceCaps(hdc, LOGPIXELSX);		// DPI
	// int pixels = 16;
	// int a = (int)((pixels * 72.0) / GetDeviceCaps(hdc,LOGPIXELSY)); // 像素转磅
	// 1/DPI = x/72  ; 1磅是1/72英寸, DPI是每英寸的像素数



	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	SelectObject(hdc, hPen);		// 选择对象
	DeleteObject(hPen);			// 删除对象
	GetCurrentObject(hdc, OBJ_PEN);	// 获取当前对象
	SetBkColor(hdc, RGB(255, 0, 0));	// 设置背景颜色
	SetBkMode(hdc, TRANSPARENT);	// 设置背景模式
	SetROP2(hdc, R2_XORPEN);	// 设置绘图模式
	CreateSolidBrush(RGB(255, 0, 0));	// 创建实心画刷
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 2. 文本

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	TextOut(hdc, 100, 100, TEXT("Hello World"), lstrlen(TEXT("Hello World")));	// 绘制文本

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);	// 获取文本度量
	printf("tm.tmHeight:%d  tm.tmAveCharWidth:%d\n", tm.tmHeight, tm.tmAveCharWidth);	// 文本度量, 文本高度,小写字母平均字符宽度
	// 大写字母通常使用 tm.tmMaxCharWidth*1.5

	EndPaint(hwnd, &ps);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 3. 点线

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s3(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	SetPixel(hdc, 50, 50, RGB(255, 0, 0));	// 设置像素点
	COLORREF ref = GetPixel(hdc, 100, 100);	// 获取像素点

	MoveToEx(hdc, 100, 100, NULL);		// 设置当前点
	LineTo(hdc, 200, 200);				// 从当前点画线到指定点,不包括当前点

	POINT pt;
	GetCurrentPositionEx(hdc, &pt);		// 获取当前点

	// 绘制网格线
	RECT rc;
	GetClientRect(hwnd, &rc);
	for (int i = 0; i < rc.right; i += 50)
	{
		MoveToEx(hdc, i, 0, NULL);
		LineTo(hdc, i, rc.bottom);
	}
	for (int i = 0; i < rc.bottom; i += 50)
	{
		MoveToEx(hdc, 0, i, NULL);
		LineTo(hdc, rc.right, i);
	}

	//通过线绘制矩形
	POINT apt1[5] = { 55,55,105,55,105,105,55,105,55,55 };
	MoveToEx(hdc, apt1[0].x, apt1[0].y, NULL);
	for (size_t i = 0; i < 5; i++)
		LineTo(hdc, apt1[i].x, apt1[i].y);

	//将各个点连接
	POINT apt2[5] = { 155,155,205,155,205,205,155,205,155,155 };
	Polyline(hdc, apt2, 5); // 不使用也不改变当先点


	// 将各点连接,使用和修改当前点
	POINT apt3[5] = { 255,255,305,255,305,305,255,305,255,255 };
	MoveToEx(hdc, apt3[0].x, apt3[0].y, NULL);
	PolylineTo(hdc, apt3 + 1, 4);


	// 连接各点,来实现一个正弦曲线
#define NUM 1000					//定义了1000个点
#define TWOPI (2 * 3.14159)			//TWOPI 等于 2pi
	static int cxClient, cyClient;
	GetClientRect(hwnd, &rc);
	cxClient = rc.right;
	cyClient = rc.bottom;
	POINT apt4[NUM];
	MoveToEx(hdc, 0, cyClient / 2, NULL);
	LineTo(hdc, cxClient, cyClient / 2);		// x轴中心线
	for (int i = 0; i < NUM; i++)
	{
		apt4[i].x = i * cxClient / NUM;									// x坐标
		apt4[i].y = (int)(cyClient / 2 * (1 - sin(TWOPI * i / NUM)));	// y坐标
	}
	Polyline(hdc, apt4, NUM); //画一组连续的线


	EndPaint(hwnd, &ps);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 4. 边框

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s4(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);


	// 矩形区域,并用画刷填充背景.
	Rectangle(hdc, 20, 0, 50, 50);
	Ellipse(hdc, 20, 100, 50, 150);
	RoundRect(hdc, 20, 200, 50, 250, 6, 4);


	// 绘图颜色设置
	HPEN hPen;
	HBRUSH hBrush;
	//hPen = (HPEN)GetStockObject(WHITE_PEN);// 系统提供画笔
	hPen = CreatePen(PS_DASH, 3, RGB(255, 0, 0));
	hBrush = CreateSolidBrush(RGB(0, 255, 0));
	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);
	Rectangle(hdc, 20, 300, 50, 350);
	DeleteObject(hPen);
	DeleteObject(hBrush);


	SetBkColor(hdc, RGB(255, 0, 0));		// 设置背景颜色
	SetBkMode(hdc, TRANSPARENT);			// 设置背景模式为透明
	SetROP2(hdc, R2_NOTCOPYPEN);				// 绘图混合模式,覆盖,混合等...


	// 矩形的使用
	RECT rect;
	rect.top = 10;
	rect.left = 100;
	rect.bottom = 60;
	rect.right = 150;
	hBrush = CreateSolidBrush(RGB(0, 255, 0));
	FillRect(hdc, &rect, hBrush);

	rect.top = 70;
	rect.left = 100;
	rect.bottom = 120;
	rect.right = 150;
	FrameRect(hdc, &rect, hBrush);

	rect.top = 130;
	rect.left = 100;
	rect.bottom = 180;
	rect.right = 150;
	FrameRect(hdc, &rect, hBrush);
	InvertRect(hdc, &rect);


	EndPaint(hwnd, &ps);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 5. 映射模式

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s5(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	/*
	窗口: 逻辑画布, 绘图区域,无限大
	视口: 屏幕上的一个矩形区域,一块有限的区域.

	GDI绘图使用的是逻辑单位在逻辑画布("窗口")中绘制, windows将逻辑单位转成设备单位最终在显示器中的某个窗口("视口")上显示.
	这个转换通过窗口原点,窗口范围,视口原点,视口范围 来完成的.
	映射模式就是

	xView = (xLogical - xLogicalOrg) * (xLogicalExt / xViewExt) + xViewOrg;
	yView = (yLogical - yLogicalOrg) * (yLogicalExt / yViewExt) + yViewOrg;
	(xLogicalOrg, yLogicalOrg) 是逻辑画布的原点
	(xLogicalExt, yLogicalExt) 是逻辑画布的范围
	(xViewOrg, yViewOrg) 是逻辑原点
	(xViewExt, yViewExt) 是视口范围
	逻辑原点会映射到视口原点上.

	(xLogicalExt / xViewExt) : 关系到逻辑单位和设备单位的缩放比例, 正负影响绘图的方向


	*/

	/*
	GetMapMode(hdc);			// 获取当前映射模式
	SetMapMode(hdc, MM_TEXT);	// 逻辑单位和设备单位相同

	POINT pt;
	RECT rc;
	ClientToScreen(hwnd, &pt);	// 将客户区坐标转换为屏幕坐标
	ScreenToClient(hwnd, &pt);  // 将屏幕坐标转换为客户区坐标
	GetWindowRect(hwnd, &rc);	// 获取窗口坐标
	GetClientRect(hwnd, &rc);	// 获取客户区坐标
	DPtoLP(hdc, &pt, 1);		// 设备坐标转换为逻辑坐标
	LPtoDP(hdc, &pt, 1);		// 逻辑坐标转换为设备坐标
	*/

	// MM_TEXT模式: 
#if 0
	SetMapMode(hdc, MM_TEXT);
	SetWindowOrgEx(hdc, 30, 30, NULL);		// 设置窗口原点,逻辑点(30,30)映射到视口原点
	SetViewportOrgEx(hdc, 50, 50, NULL);	// 设置视口原点为设备像素点(50,50)
	Rectangle(hdc, 0, 0, 30, 30);
	// 逻辑点(30,30)映射到视口点(50,50)
#endif


	// ================================  MM_ISOTROPIC 模式 
	// MM_ISOTROPIC 能改变坐标转换因子.也就是修改逻辑单位的物理尺寸
	// MM_ISOTROPIC	  : 逻辑轴上,1x和1y代表相同的物理尺寸
	// windows会调整视口和窗口的范围,以达到逻辑单位和设备单位的缩放比例相同.
#if 0
	int cxClient, cyClient;
	RECT rect;
	GetClientRect(hwnd, &rect);
	cxClient = rect.right;
	cyClient = rect.bottom;

	// 理解: MM_ISOTROPIC 模式,和比例关系
	SetMapMode(hdc, MM_ISOTROPIC);					// 数学坐标系,x和y的单位相同

	SetWindowOrgEx(hdc, 0, 0, NULL);				// 逻辑0,0 映射到视口原点.
	SetViewportOrgEx(hdc, 100, 100, NULL);			// 设置视口原点为设备像素点(100,300)
	SetViewportExtEx(hdc, cxClient / 2, cyClient / 2, NULL);
	SetWindowExtEx(hdc, cxClient / 2, cyClient / 2, NULL);			// 1:1 	 1逻辑单位等于 1物理像素
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 0));
	rect.top = 50;
	rect.left = -50;
	rect.right = 50;
	rect.bottom = -50;
	FillRect(hdc, &rect, hBrush);

	SetViewportOrgEx(hdc, 100, 300, NULL);					// 设置视口原点为设备像素点(300,300)
	SetViewportExtEx(hdc, cxClient, cyClient, NULL);
	SetWindowExtEx(hdc, cxClient / 2, cyClient / 2, NULL);	// 2:1		1逻辑单位等于2物理像素
	hBrush = CreateSolidBrush(RGB(255, 0, 0));
	rect.top = 50;
	rect.left = -50;
	rect.right = 50;
	rect.bottom = -50;
	FillRect(hdc, &rect, hBrush);

	SetViewportOrgEx(hdc, 300, 300, NULL);
	SetViewportExtEx(hdc, cxClient, cyClient, NULL);
	SetWindowExtEx(hdc, 2 * cxClient, 2 * cyClient, NULL);		// 1:2		2逻辑单位等于1物理像素
	hBrush = CreateSolidBrush(RGB(255, 0, 0));
	rect.top = 50;
	rect.left = -50;
	rect.right = 50;
	rect.bottom = -50;
	FillRect(hdc, &rect, hBrush);
#endif 

	// MM_ISOTROPIC 模式, 逻辑单位和设备单位的缩放比例相同. 保证整个逻辑坐标在视口中显示完整.
#if 0
	int cxClient, cyClient;
	RECT rect;
	GetClientRect(hwnd, &rect);
	cxClient = rect.right;
	cyClient = rect.bottom;

	SetMapMode(hdc, MM_ISOTROPIC);					// 数学坐标系,x和y的单位相同

	SetViewportExtEx(hdc, cxClient, cyClient, NULL);
	SetWindowExtEx(hdc, 32767, 32767, NULL);		// 1:2		2逻辑单位等于1物理像素
	SetViewportOrgEx(hdc, 0, 0, NULL);
	SetWindowOrgEx(hdc, 300, 300, NULL);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
	rect.top = 0;
	rect.left = 0;
	rect.right = 32767;
	rect.bottom = 32767;
	FillRect(hdc, &rect, hBrush);
#endif

	// ================================  MM_ANISOTROPIC 模式 
	// MM_ANISOTROPIC	  : 保证逻辑坐标铺满整个视口.会拉伸逻辑图
#if 0
	int cxClient, cyClient;
	RECT rect;
	GetClientRect(hwnd, &rect);
	cxClient = rect.right;
	cyClient = rect.bottom;

	SetMapMode(hdc, MM_ANISOTROPIC);					// 数学坐标系,x和y的单位相同

	SetViewportExtEx(hdc, cxClient, cyClient, NULL);
	SetWindowExtEx(hdc, 32767, 32767, NULL);		// 1:2		2逻辑单位等于1物理像素
	SetViewportOrgEx(hdc, 0, 0, NULL);
	SetWindowOrgEx(hdc, 300, 300, NULL);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
	rect.top = 0;
	rect.left = 0;
	rect.right = 32767;
	rect.bottom = 32767;
	FillRect(hdc, &rect, hBrush);
#endif


#if 01
	// ============实现笛卡尔坐标系:
	int cxClient, cyClient;
	RECT rect;
	GetClientRect(hwnd, &rect);
	cxClient = rect.right;
	cyClient = rect.bottom;

	SetMapMode(hdc, MM_ISOTROPIC);

	SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL);	//  视口原点设置到窗口正中心
	SetWindowOrgEx(hdc, 0, 0, NULL);							//  0,0逻辑点映射到视口原点.

	SetViewportExtEx(hdc, cxClient / 2, -cyClient / 2, NULL);
	SetWindowExtEx(hdc, cxClient / 2, cyClient / 2, NULL);		// 比例 x 1:1 ,  y 1:-1 

	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
	rect.top = 50;
	rect.bottom = 0;
	rect.left = 0;
	rect.right = 50;
	FillRect(hdc, &rect, hBrush);
	hBrush = CreateSolidBrush(RGB(0, 255, 0));
	rect.top = 0;
	rect.bottom = -50;
	rect.left = 0;
	rect.right = 50;
	FillRect(hdc, &rect, hBrush);
	hBrush = CreateSolidBrush(RGB(0, 0, 255));
	rect.top = 0;
	rect.bottom = -50;
	rect.left = -50;
	rect.right = 0;
	FillRect(hdc, &rect, hBrush);
	hBrush = CreateSolidBrush(RGB(111, 111, 222));
	rect.top = 50;
	rect.bottom = 0;
	rect.left = -50;
	rect.right = 0;
	FillRect(hdc, &rect, hBrush);
#endif 

	EndPaint(hwnd, &ps);
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 6. 区域

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s6(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

#if 0
	// 可以通过区域裁剪绘图
	//HRGN hRgn = CreateRectRgn(100, 100, 200, 200);
	//HRGN hRgn = CreateEllipticRgn(100, 100, 200, 200);
	POINT pts[3] = { {100, 100},{200, 100},{150, 200} };
	HRGN hRgn = CreatePolygonRgn((POINT*)&pts, 3, ALTERNATE);
	SelectClipRgn(hdc, hRgn);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = 300;
	rect.bottom = 300;
	FillRect(hdc, &rect, hBrush);
#endif

#if 01
	// 可以通过区域裁剪绘图
	//HRGN hRgn = CreateRectRgn(100, 100, 200, 200);
	HRGN hRgn1 = CreateEllipticRgn(100, 100, 200, 200);
	POINT pts[3] = { {100, 100},{200, 100},{150, 200} };
	HRGN hRgn2 = CreatePolygonRgn((POINT*)&pts, 3, ALTERNATE);
	CombineRgn(hRgn1, hRgn1, hRgn2, RGN_XOR);		// 合并区域
	SelectClipRgn(hdc, hRgn1);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = 300;
	rect.bottom = 300;
	FillRect(hdc, &rect, hBrush);

#endif

	FillRgn(hdc, hRgn1, hBrush);				// 填充区域
	FrameRgn(hdc, hRgn1, hBrush, 1, 1);	// 绘制区域边框
	InvertRgn(hdc, hRgn1);				// 反转区域
	PaintRgn(hdc, hRgn1);				// 绘制区域

	EndPaint(hwnd, &ps);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 7. 位图

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s7(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	HDC hdcWindow = GetWindowDC(NULL);

	// 像素拷贝
#if 0
	BitBlt(hdc, 10, 10, 100, 100, hdcWindow, 0, 0, SRCCOPY);	// 从hdcWindow设备显存中拷贝0,0开始拷贝100*100 到 hdc的(10,10) 100*100
	BitBlt(hdc, 10, 120, 100, 100, hdcWindow, 0, 0, NOTSRCCOPY);// 从hdcWindow设备显存中拷贝0,0开始拷贝100*100 到 hdc的(10,120) 100*100
#endif 

	// 拉伸像素拷贝
#if 0
	SetStretchBltMode(hdc, HALFTONE);					// 设置缩放模式为半色调
	StretchBlt(hdc, 10, 150, 100, 100, hdcWindow, 0, 0, 300, 300, MERGECOPY); // 拉伸拷贝, hdcWindow显存中(0,0) 300*300 区域 显示到 (10,150) 100*100 区域  图像被压缩了
	StretchBlt(hdc, 10, 400/*反向要修正*/, 100, -100, hdcWindow, 0, 0, 300, 300, MERGECOPY); // 目标宽度负值可以进行图像的翻转
	StretchBlt(hdc, 10, 450, 100, 100, hdcWindow, 0, 0, 50, 50, MERGECOPY); // 拉伸拷贝, hdcWindow显存中(0,0) 50*50 区域 显示到 (10,300) 100*100 区域  图像被放大了
#endif 

	// 绘制像素
#if 0
	PatBlt(hdc, 10, 10, 100, 100, BLACKNESS);		// 填充黑色
#endif


	// 创建位图
#if 0
	HBITMAP hBitMap;		// 创建设备位图
	// hBitMap = CreateBitmap(9, 9, 5, 3, NULL);			// 方式1: 自己设计位图的格式,比较麻烦
	hBitMap = CreateCompatibleBitmap(hdc, 100, 100);		// 方式2: 根据设备环境创建位图, 省事儿
	//hBitMap = CreateBitmapIndirect(&bitMap);				// 方式3: 根据位图信息创建位图

	//SetBitmapBits(hBitMap, );	j							// 修改位图数据
	//GetBitmapBits()

	DeleteObject(hBitMap);
#endif


	// 内存中绘图, 然后再拷贝到设备中显示
#if 0
	HBITMAP hBitMap;
	HDC hdcMem;
	hdcMem = CreateCompatibleDC(hdc);					// 创建一个与hdc环境兼容的内存设备环境
	hBitMap = CreateCompatibleBitmap(hdc, 100, 100);	// 根据hdc的环境创建一个设备相关位图
	SelectObject(hdcMem, hBitMap);						// 将位图数据引入内存环境,这样这个内存dc,就能当做真实DC进行绘图了

	Rectangle(hdcMem, 10, 10, 30, 30);						// 在内存DC中绘图
	BitBlt(hdc, 0, 0, 100, 100, hdcMem, 0, 0, SRCCOPY);		// 将内存设备位图显示在hdc设备中

	DeleteObject(hBitMap);
	DeleteDC(hdcMem);
#endif

#if 01
	// PE文件位图资源的使用
	BITMAP bitMap;
	HBITMAP hBitMap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_TEST));

	GetObject(hBitMap, sizeof(BITMAP), &bitMap);						// 获取位图信息
	printf("width:%d height:%d\n", bitMap.bmWidth, bitMap.bmHeight);	// 打印位图信息

	HDC hdcMem = CreateCompatibleDC(hdc);				// 创建一个与hdc环境兼容的内存位图设备环境
	SelectObject(hdcMem, hBitMap);						// 将位图数据引入内存环境,这样这个内存dc,就能当做真实DC进行绘图了
	BitBlt(hdc, 0, 0, 100, 100, hdcMem, 0, 0, SRCCOPY); // 将内存设备位图显示在hdc设备中
	DeleteDC(hdcMem);
#endif 



	EndPaint(hwnd, &ps);
}










// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//			    window 窗口界面基础

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// 连接器>系统>子系统>窗口    就会使用WinMain作为入口函数,  如果是控制台就是使用main作为入口函数

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 模拟  win 桌面项目入口函数 
static int WINAPI TestWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 1. 注册窗口类
	WNDCLASSEX wcx = { 0 };

	TCHAR class_name[] = TEXT("test_window");
	wcx.cbSize = sizeof(wcx);					// size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW;		// redraw if size changes 
	wcx.lpfnWndProc = WindowProc;				// points to window procedure 
	wcx.cbClsExtra = 0;							// no extra class memory 
	wcx.cbWndExtra = 0;							// no extra window memory 
	wcx.hInstance = hInstance;					// handle to instance 
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);              // predefined app. icon 
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);	// predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // white background brush 
	wcx.lpszMenuName = TEXT("MainMenu");		// name of menu resource 
	wcx.lpszClassName = class_name;				// TEXT("MainWClass");  // name of window class 
	wcx.hIconSm = NULL;//LoadImage(hInstance, MAKEINTRESOURCE(5),IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),LR_DEFAULTCOLOR);
	ATOM a = RegisterClassEx(&wcx);
	if (!a)
	{
		mDbgPrint(TEXT("RegisterClassEx error: %d\n"), GetLastError());
		return 0;
	}

	// 2. 创建窗口,WM_CREATE消息会发送到窗口过程
	HWND hwnd = CreateWindowEx(0, class_name, TEXT("窗口程序"), WS_OVERLAPPEDWINDOW, 0, 0, 300, 300, NULL, NULL, hInstance, NULL);
	if (NULL == hwnd)
	{
		mDbgPrint(TEXT("CreateWindowEx error: %d\n"), GetLastError());
		return 0;
	}

	// 3. 显示窗口
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);			    // 发送首个WM_PAINT消息

	// 4. 接受消息并处理
	MSG msg;
	BOOL bRet;
	while ((bRet = GetMessage(&msg, hwnd, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			mDbgPrint(TEXT("GetMessage error: %d\n"), GetLastError());
			break;
		}
		else
		{
			TranslateMessage(&msg); // 转换消息 : 将虚拟键代码消息转换为字符消息,也就是 'a' 等消息
			DispatchMessage(&msg);  // 分发消息 : 进到内核,从内核发起消息处理函数的调用. 
		}
	}

	return 0;
}

// 消息处理函数
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:		// 窗口出现无效区域,就会收到PAINT消息进行重绘,  简单的理解: 需要重绘的时候重绘
	{
		s7(hwnd, msg, wParam, lParam);
		break;
	}
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	}

	return 0;
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void p000_008()
{
	TestWinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_NORMAL);
}
