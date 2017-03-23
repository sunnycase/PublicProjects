#pragma once
#include <Tomato.Core/Tomato.Core.h>
#include <d2d1.h>
#include <wincodec.h>
#include "CGdImageBox.h"

// CImageBox

class CImageBox : public CStatic
{
	DECLARE_DYNAMIC(CImageBox)

public:
	CImageBox();
	virtual ~CImageBox();

	void SetPicture(HBITMAP bitmap);
protected:
	DECLARE_MESSAGE_MAP()
private:
	void CreateDeviceDependentResources();
private:
	WRL::ComPtr<ID2D1Factory> _d2dFactory;
	WRL::ComPtr<ID2D1HwndRenderTarget> _renderTarget;
	WRL::ComPtr<ID2D1SolidColorBrush> _brush;
	WRL::ComPtr<ID2D1Bitmap> _bitmap;
	WRL::ComPtr<IWICImagingFactory> _wicFactory;
	WRL::ComPtr<IWICBitmapSource> _wicBitmap;

	CGdImageBox _gdImageBox;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
};


