#include<afxwin.h>


class C0002Obj : public CView
{
public:
	int mPixelsToPoints(int pixels);
	void DrawInMem();
	BOOL OnEraseBkgnd(CDC* pDC);
};

// 转换像素到磅
int C0002Obj::mPixelsToPoints(int pixels)
{
	CDC* pDC = GetDC();
	// 获取屏幕的DPI
	int dpi = pDC->GetDeviceCaps(LOGPIXELSY); // 使用LOGPIXELSX获取水平DPI
	// 转换像素到磅
	return (int)(pixels * 10 * 72.0 / dpi + 0.5);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//                                            解决闪烁问题

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//重绘要避过子窗口控件 : InvalidateRect(rect,TRUE);

// 处理掉重绘,否则子控件会一直重绘闪动
BOOL C0002Obj::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	// return CView::OnEraseBkgnd(pDC);
}

// 内存中绘图
void C0002Obj::DrawInMem()
{
	/*
	双缓冲绘图减少绘图次数
	区域InvalidateRect() 避开子控件区域,让子控件避免闪烁.
	OnEraseBkgnd() return TRUE; 避免背景擦除闪烁.
	*/

	CDC* pDC = this->GetDC();

	CBitmap bitmapBuffer;
	CDC memDC;

	// LockWindowUpdate();

	memDC.CreateCompatibleDC(NULL);

	CRect rect; 
	rect.top = 0;
	rect.left = 0;
	rect.right = 1024;
	rect.bottom = 768;

	// 创建一个兼容的位图
	if (!bitmapBuffer.m_hObject)
	{
		bitmapBuffer.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
		memDC.SelectObject(&bitmapBuffer);
	}

	// 字体定义
	CFont fontNormal, fontSmall;
	fontNormal.CreatePointFont(mPixelsToPoints(16), TEXT("Arial"));
	fontSmall.CreatePointFont(mPixelsToPoints(12), TEXT("Arial"));

	rect.top = 0;
	rect.left = 0;
	rect.right = rect.Width();
	rect.bottom = rect.Height();
	memDC.FillRect(&rect, &CBrush(RGB(255, 0 , 255)));

	memDC.Draw3dRect(rect, RGB(255, 0, 0), RGB(0, 255, 0));

	// 绘制边框
	memDC.SetBkColor(RGB(255, 255, 255));
	memDC.SetTextColor(RGB(0, 0, 0));
	memDC.SelectObject(&fontNormal);

	GetClientRect(&rect);
	pDC->BitBlt(rect.left, rect.top , rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	bitmapBuffer.DeleteObject();
	memDC.DeleteDC();
}
