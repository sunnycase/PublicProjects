#pragma once
#include <Tomato.Core/Tomato.Core.h>
#include <d2d1.h>
#include <wincodec.h>

// CImageBox

class CImageBox : public CScrollView
{
	DECLARE_DYNAMIC(CImageBox)

public:
	CImageBox();
	virtual ~CImageBox();

	void SetPicture(HBITMAP bitmap);
	void SetZoom(float factor);
	void Rotate(uint32_t degree);
	void AutoFitSize();
protected:
	DECLARE_MESSAGE_MAP()
private:
	void CreateDeviceDependentResources();
	void UpdateScrollSizes();
	void UpdateBitmap();
private:
	WRL::ComPtr<ID2D1Factory> _d2dFactory;
	WRL::ComPtr<ID2D1HwndRenderTarget> _renderTarget;
	WRL::ComPtr<ID2D1SolidColorBrush> _brush;
	WRL::ComPtr<ID2D1Bitmap> _bitmap;
	WRL::ComPtr<IWICImagingFactory> _wicFactory;
	WRL::ComPtr<IWICBitmapSource> _origWicBitmap;
	WRL::ComPtr<IWICBitmapSource> _wicBitmap;
private:
	float _zoomFactor = 1.0f;
	WICBitmapTransformOptions _rotation = WICBitmapTransformRotate0;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnDraw(CDC* /*pDC*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void PostNcDestroy();
};


