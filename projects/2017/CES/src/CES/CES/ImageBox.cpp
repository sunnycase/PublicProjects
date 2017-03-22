// ImageBox.cpp : 实现文件
//

#include "stdafx.h"
#include "CES.h"
#include "ImageBox.h"

using namespace WRL;

// CImageBox

IMPLEMENT_DYNAMIC(CImageBox, CStatic)

CImageBox::CImageBox()
{

}

CImageBox::~CImageBox()
{
}


BEGIN_MESSAGE_MAP(CImageBox, CStatic)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CImageBox 消息处理程序


void CImageBox::SetPicture(IWICBitmap * wicBitmap)
{
	if (wicBitmap)
	{
		ComPtr<ID2D1Bitmap> bitmap;
		ThrowIfFailed(_renderTarget->CreateBitmapFromWicBitmap(wicBitmap, &bitmap));
		_bitmap = bitmap;
	}
	else
		_bitmap.Reset();
	Invalidate();
}

void CImageBox::CreateDeviceDependentResources()
{
	ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&_d2dFactory)));

	RECT rect;
	GetClientRect(&rect);
	ThrowIfFailed(_d2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			GetSafeHwnd(), 
			D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top)), &_renderTarget));
	ThrowIfFailed(_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &_brush));
}

int CImageBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	CreateDeviceDependentResources();
	return 0;
}


void CImageBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	RECT rect;
	GetClientRect(&rect);
	_renderTarget->BeginDraw();
	if (_bitmap)
	{
		_renderTarget->DrawBitmap(_bitmap.Get(), D2D1::RectF(
			rect.left,
			rect.top,
			rect.right,
			rect.bottom));
	}
	auto hr = _renderTarget->EndDraw();
}
