// ImageBox.cpp : 实现文件
//

#include "stdafx.h"
#include "CES.h"
#include "ImageBox.h"
#include <algorithm>

using namespace WRL;

// CImageBox

IMPLEMENT_DYNAMIC(CImageBox, CScrollView)

CImageBox::CImageBox()
{

}

CImageBox::~CImageBox()
{
}


BEGIN_MESSAGE_MAP(CImageBox, CScrollView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CImageBox 消息处理程序

namespace
{
	HBITMAP CreateHBITMAP(IWICBitmapSource * ipBitmap)
	{
		// initialize return value
		HBITMAP hbmp = NULL;

		// get image attributes and check for valid image

		UINT width = 0;

		UINT height = 0;

		if (FAILED(ipBitmap->GetSize(&width, &height)) || width == 0 || height == 0)

			goto Return;

		// prepare structure giving bitmap information (negative height indicates a top-down DIB)

		BITMAPINFO bminfo;

		ZeroMemory(&bminfo, sizeof(bminfo));

		bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

		bminfo.bmiHeader.biWidth = width;

		bminfo.bmiHeader.biHeight = -((LONG)height);

		bminfo.bmiHeader.biPlanes = 1;

		bminfo.bmiHeader.biBitCount = 32;

		bminfo.bmiHeader.biCompression = BI_RGB;

		// create a DIB section that can hold the image

		void * pvImageBits = NULL;

		HDC hdcScreen = GetDC(NULL);

		hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, NULL, 0);

		ReleaseDC(NULL, hdcScreen);

		if (hbmp == NULL)

			goto Return;

		// extract the image into the HBITMAP

		const UINT cbStride = width * 4;

		const UINT cbImage = cbStride * height;

		if (FAILED(ipBitmap->CopyPixels(NULL, cbStride, cbImage, static_cast<BYTE *>(pvImageBits))))

		{

			// couldn't extract image; delete HBITMAP

			DeleteObject(hbmp);

			hbmp = NULL;

		}

	Return:

		return hbmp;

	}
}

void CImageBox::SetPicture(HBITMAP bitmap)
{
	if (bitmap)
	{
		ComPtr<IWICBitmap> wicBitmap;
		ThrowIfFailed(_wicFactory->CreateBitmapFromHBITMAP(bitmap, nullptr, WICBitmapIgnoreAlpha, &wicBitmap));

		ComPtr<IWICBitmapFlipRotator> flipRotator;
		ThrowIfFailed(_wicFactory->CreateBitmapFlipRotator(&flipRotator));
		ThrowIfFailed(flipRotator->Initialize(wicBitmap.Get(), WICBitmapTransformFlipVertical));

		_origWicBitmap = flipRotator;
	}
	else
	{
		_origWicBitmap.Reset();
	}
	UpdateBitmap();
}

void CImageBox::SetZoom(float factor)
{
	_zoomFactor = factor;
	UpdateScrollSizes();
	Invalidate();
}

void CImageBox::Rotate(uint32_t degree)
{
	switch (degree)
	{
	case 0:
		_rotation = WICBitmapTransformRotate0;
		break;
	case 90:
		_rotation = WICBitmapTransformRotate90;
		break;
	case 180:
		_rotation = WICBitmapTransformRotate180;
		break;
	case 270:
		_rotation = WICBitmapTransformRotate270;
		break;
	default:
		ThrowAlways(L"Invalid rotation degree.");
		break;
	}
	UpdateBitmap();
}

#undef min

void CImageBox::AutoFitSize()
{
	if (_wicBitmap)
	{
		RECT rect;
		GetClientRect(&rect);

		UINT width, height;
		ThrowIfFailed(_wicBitmap->GetSize(&width, &height));
		if (width && height)
		{
			_zoomFactor = std::min(float(rect.right - rect.left) / width, float(rect.bottom - rect.top) / height);
			UpdateScrollSizes();
		}
	}
	Invalidate();
}

void CImageBox::CreateDeviceDependentResources()
{
	ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC, IID_PPV_ARGS(&_wicFactory)));

	ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&_d2dFactory)));

	RECT rect;
	GetClientRect(&rect);
	ThrowIfFailed(_d2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			GetSafeHwnd(), 
			D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top)), &_renderTarget));
	ThrowIfFailed(_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &_brush));
}

void CImageBox::UpdateScrollSizes()
{
	if (_wicBitmap)
	{
		UINT width, height;
		ThrowIfFailed(_wicBitmap->GetSize(&width, &height));

		width *= _zoomFactor;
		height *= _zoomFactor;
		SetScrollSizes(MM_TEXT, CSize(width, height));
	}
	else
		SetScrollSizes(MM_TEXT, CSize(0, 0));
}

void CImageBox::UpdateBitmap()
{
	if (_origWicBitmap)
	{
		ComPtr<IWICBitmapFlipRotator> flipRotator;
		ThrowIfFailed(_wicFactory->CreateBitmapFlipRotator(&flipRotator));
		ThrowIfFailed(flipRotator->Initialize(_origWicBitmap.Get(), _rotation));

		ComPtr<ID2D1Bitmap> d2dBitmap;
		ThrowIfFailed(_renderTarget->CreateBitmapFromWicBitmap(flipRotator.Get(), &d2dBitmap));
		_wicBitmap = flipRotator;
		_bitmap = d2dBitmap;
	}
	else
	{
		_wicBitmap.Reset();
		_bitmap.Reset();
	}
	UpdateScrollSizes();
	Invalidate();
}

int CImageBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CreateDeviceDependentResources();

	SetScrollSizes(MM_TEXT, CSize(0, 0));
	return 0;
}


void CImageBox::OnDraw(CDC* /*pDC*/)
{
	RECT rect;
	GetClientRect(&rect);
	_renderTarget->BeginDraw();
	if (_bitmap)
	{
		auto position = GetScrollPosition();

		_renderTarget->DrawBitmap(_bitmap.Get(), D2D1::RectF(
			rect.left,
			rect.top,
			rect.right,
			rect.bottom), 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(
				position.x / _zoomFactor,
				position.y / _zoomFactor,
				(position.x + rect.right - rect.left) / _zoomFactor,
				(position.y + rect.bottom - rect.top) / _zoomFactor));
	}
	auto hr = _renderTarget->EndDraw();
}


void CImageBox::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED)
	{
		//需要判断一下，是最小化则退出。
		//如果是最小化，恢复的时候会BUG，因为整数除以0
		return;
	}

	RECT rect;
	GetClientRect(&rect);
	ThrowIfFailed(_renderTarget->Resize(D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top)));
}


void CImageBox::PostNcDestroy()
{

}
