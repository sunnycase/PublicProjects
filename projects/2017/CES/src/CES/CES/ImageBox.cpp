// ImageBox.cpp : 实现文件
//

#include "stdafx.h"
#include "CES.h"
#include "ImageBox.h"
#include <algorithm>
#include <unordered_set>

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

namespace std {
	template<> struct hash<GUID>
	{
		size_t operator()(const GUID& guid) const noexcept {
			const std::uint64_t* p = reinterpret_cast<const std::uint64_t*>(&guid);
			std::hash<std::uint64_t> hash;
			return hash(p[0]) ^ hash(p[1]);
		}
	};
}

namespace
{
	const std::unordered_set<GUID> _supportedWicFormats =
	{
		GUID_WICPixelFormat32bppPRGBA,
		GUID_WICPixelFormat32bppBGR,
		GUID_WICPixelFormat32bppPBGRA
	};
}

void CImageBox::SetPicture(std::wstring_view fileName)
{
	if (!fileName.empty())
	{
		ComPtr<IWICBitmapDecoder> decoder;
		ThrowIfFailed(_wicFactory->CreateDecoderFromFilename(fileName.data(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder));
		ComPtr<IWICBitmapFrameDecode> frame;
		ThrowIfFailed(decoder->GetFrame(0, &frame));

		WICPixelFormatGUID format;
		ThrowIfFailed(frame->GetPixelFormat(&format));
		if (_supportedWicFormats.find(format) != _supportedWicFormats.end())
			_origWicBitmap = frame;
		else
		{
			ComPtr<IWICFormatConverter> converter;
			ThrowIfFailed(_wicFactory->CreateFormatConverter(&converter));
			ThrowIfFailed(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeMedianCut));
			_origWicBitmap = converter;
		}
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

namespace
{
	void Save(IWICImagingFactory* wicFactory, IWICStream* destStream, IWICBitmapSource* source)
	{
		ComPtr<IWICBitmapEncoder> encoder;
		ThrowIfFailed(wicFactory->CreateEncoder(GUID_ContainerFormatJpeg, nullptr, &encoder));
		ThrowIfFailed(encoder->Initialize(destStream, WICBitmapEncoderNoCache));

		ComPtr<IWICBitmapFrameEncode> frameEncode;
		ComPtr<IPropertyBag2> encodeOptions;
		ThrowIfFailed(encoder->CreateNewFrame(&frameEncode, &encodeOptions));
		ThrowIfFailed(frameEncode->Initialize(encodeOptions.Get()));
		auto pixelFormat = GUID_WICPixelFormat24bppBGR;
		ThrowIfFailed(frameEncode->SetPixelFormat(&pixelFormat));
		if (source)
		{
			UINT width, height;
			ThrowIfFailed(source->GetSize(&width, &height));
			ThrowIfFailed(frameEncode->SetSize(width, height));
			ThrowIfFailed(frameEncode->WriteSource(source, nullptr));
			ThrowIfFailed(frameEncode->Commit());
		}
		ThrowIfFailed(encoder->Commit());
	}
}

void CImageBox::SaveAs(std::wstring_view fileName)
{
	ComPtr<IWICStream> stream;
	ThrowIfFailed(_wicFactory->CreateStream(&stream));
	ThrowIfFailed(stream->InitializeFromFilename(fileName.data(), GENERIC_WRITE));
	Save(_wicFactory.Get(), stream.Get(), _origWicBitmap.Get());
}

WRL::ComPtr<IStream> CImageBox::SaveToStream()
{
	ComPtr<IStream> memStream;
	ThrowIfFailed(CreateStreamOnHGlobal(nullptr, TRUE, &memStream));
	ComPtr<IWICStream> stream;
	ThrowIfFailed(_wicFactory->CreateStream(&stream));
	ThrowIfFailed(stream->InitializeFromIStream(memStream.Get()));
	Save(_wicFactory.Get(), stream.Get(), _origWicBitmap.Get());
	return memStream;
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
	UpdateScrollSizes();
}


void CImageBox::PostNcDestroy()
{

}
