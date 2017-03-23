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

		_wicBitmap = flipRotator;

		ThrowIfFailed(_gdImageBox.DisplayFromHBitmap((long)CreateHBITMAP(flipRotator.Get())));
	}
	else
	{
		_wicBitmap.Reset();
		_bitmap.Reset();
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

int CImageBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	CreateDeviceDependentResources();

	RECT rect;
	GetClientRect(&rect);
	ThrowIfNot(_gdImageBox.Create(nullptr, WS_CHILD | WS_VISIBLE, rect, this), L"cannot init window.");
	_gdImageBox.put_LicenseKEY(L"1519611432053604640600840");
	return 0;
}


void CImageBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	//RECT rect;
	//GetClientRect(&rect);
	//_renderTarget->BeginDraw();
	//if (_bitmap)
	//{
	//	_renderTarget->DrawBitmap(_bitmap.Get(), D2D1::RectF(
	//		rect.left,
	//		rect.top,
	//		rect.right,
	//		rect.bottom));
	//}
	//auto hr = _renderTarget->EndDraw();
}
