// CGdImageBox.h : 由 Microsoft Visual C++ 创建的 ActiveX 控件包装器类的声明

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CGdImageBox

class CGdImageBox : public CWnd
{
protected:
	DECLARE_DYNCREATE(CGdImageBox)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x7FD025C0, 0xA085, 0x44EA, { 0xB4, 0x11, 0x40, 0x0, 0xE, 0x23, 0x79, 0x18 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff,
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID = 0xffff, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// 特性
public:

// 操作
public:

	long get_MousePointer()
	{
		long result;
		InvokeHelper(0x6803002c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_MousePointer(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6803002c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_BorderStyle()
	{
		long result;
		InvokeHelper(0x6803002b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_BorderStyle(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6803002b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void Terminate()
	{
		InvokeHelper(0x6003002d, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	long DisplayNextFrame()
	{
		long result;
		InvokeHelper(0x6003002e, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long DisplayPreviousFrame()
	{
		long result;
		InvokeHelper(0x6003002f, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long DisplayFirstFrame()
	{
		long result;
		InvokeHelper(0x60030030, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long DisplayLastFrame()
	{
		long result;
		InvokeHelper(0x60030031, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long DisplayFrame(long nFrame)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030032, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nFrame);
		return result;
	}
	unsigned long get_BackColor()
	{
		unsigned long result;
		InvokeHelper(0x6803002a, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_BackColor(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x6803002a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long DisplayFromStdPicture(LPDISPATCH * oStdPicture)
	{
		long result;
		static BYTE parms[] = VTS_PDISPATCH ;
		InvokeHelper(0x60030033, DISPATCH_METHOD, VT_I4, (void*)&result, parms, oStdPicture);
		return result;
	}
	void CloseImage()
	{
		InvokeHelper(0x60030034, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void CloseImageEx()
	{
		InvokeHelper(0x60030035, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void ImageClosed()
	{
		InvokeHelper(0x60030036, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	BOOL isRectDrawed()
	{
		BOOL result;
		InvokeHelper(0x60030037, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void GetDisplayedArea(long * nLeft, long * nTop, long * nWidth, long * nHeight)
	{
		static BYTE parms[] = VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x60030038, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nLeft, nTop, nWidth, nHeight);
	}
	void GetDisplayedAreaMM(float * nLeft, float * nTop, float * nWidth, float * nHeight)
	{
		static BYTE parms[] = VTS_PR4 VTS_PR4 VTS_PR4 VTS_PR4 ;
		InvokeHelper(0x60030039, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nLeft, nTop, nWidth, nHeight);
	}
	void GetRectValues(long * nLeft, long * nTop, long * nWidth, long * nHeight)
	{
		static BYTE parms[] = VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x6003003a, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nLeft, nTop, nWidth, nHeight);
	}
	long GetRectX()
	{
		long result;
		InvokeHelper(0x6003003b, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetRectY()
	{
		long result;
		InvokeHelper(0x6003003c, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetRectHeight()
	{
		long result;
		InvokeHelper(0x6003003d, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetRectWidth()
	{
		long result;
		InvokeHelper(0x6003003e, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	void GetRectValuesMM(float * nLeft, float * nTop, float * nWidth, float * nHeight)
	{
		static BYTE parms[] = VTS_PR4 VTS_PR4 VTS_PR4 VTS_PR4 ;
		InvokeHelper(0x6003003f, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nLeft, nTop, nWidth, nHeight);
	}
	void SetRectValuesMM(float nLeft, float nTop, float nWidth, float nHeight)
	{
		static BYTE parms[] = VTS_R4 VTS_R4 VTS_R4 VTS_R4 ;
		InvokeHelper(0x60030040, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nLeft, nTop, nWidth, nHeight);
	}
	void GetRectValuesObject(long * nLeft, long * nTop, long * nWidth, long * nHeight)
	{
		static BYTE parms[] = VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x60030041, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nLeft, nTop, nWidth, nHeight);
	}
	void SetRectValues(long nLeft, long nTop, long nWidth, long nHeight)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x60030042, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nLeft, nTop, nWidth, nHeight);
	}
	void SetRectValuesObject(long nLeft, long nTop, long nWidth, long nHeight)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x60030043, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nLeft, nTop, nWidth, nHeight);
	}
	long PlayGif()
	{
		long result;
		InvokeHelper(0x60030044, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	void StopGif()
	{
		InvokeHelper(0x60030045, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	long DisplayFromStream(LPUNKNOWN oStream)
	{
		long result;
		static BYTE parms[] = VTS_UNKNOWN ;
		InvokeHelper(0x60030046, DISPATCH_METHOD, VT_I4, (void*)&result, parms, oStream);
		return result;
	}
	long DisplayFromStreamICM(LPUNKNOWN oStream)
	{
		long result;
		static BYTE parms[] = VTS_UNKNOWN ;
		InvokeHelper(0x60030047, DISPATCH_METHOD, VT_I4, (void*)&result, parms, oStream);
		return result;
	}
	void DisplayFromURLStop()
	{
		InvokeHelper(0x60030048, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	long DisplayFromFTP(LPCTSTR sHost, LPCTSTR sPath, LPCTSTR sLogin, LPCTSTR sPassword, long nFTPPort)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 ;
		InvokeHelper(0x60030049, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sHost, sPath, sLogin, sPassword, nFTPPort);
		return result;
	}
	void SetHttpTransfertBufferSize(long nBuffersize)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6003004a, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nBuffersize);
	}
	void SetFtpPassiveMode(BOOL bPassiveMode)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x6003004b, DISPATCH_METHOD, VT_EMPTY, NULL, parms, bPassiveMode);
	}
	long DisplayFromURL(LPCTSTR sHost, LPCTSTR sPath, long nHTTPPort)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 ;
		InvokeHelper(0x6003004c, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sHost, sPath, nHTTPPort);
		return result;
	}
	long DisplayFromByteArray(SAFEARRAY * * arBytes)
	{
		long result;
		static BYTE parms[] = VTS_UNKNOWN ;
		InvokeHelper(0x6003004d, DISPATCH_METHOD, VT_I4, (void*)&result, parms, arBytes);
		return result;
	}
	long DisplayFromString(LPCTSTR sImageString)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x60030094, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sImageString);
		return result;
	}
	long DisplayFromByteArrayICM(SAFEARRAY * * arBytes)
	{
		long result;
		static BYTE parms[] = VTS_UNKNOWN ;
		InvokeHelper(0x6003004e, DISPATCH_METHOD, VT_I4, (void*)&result, parms, arBytes);
		return result;
	}
	long DisplayFromFile(LPCTSTR sFilePath)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x6003004f, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sFilePath);
		return result;
	}
	long DisplayFromMetaFile(LPCTSTR sFilePath, float nScaleBy)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_R4 ;
		InvokeHelper(0x60030090, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sFilePath, nScaleBy);
		return result;
	}
	long DisplayFromFileICM(LPCTSTR sFilePath)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x60030050, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sFilePath);
		return result;
	}
	long DisplayFromGdPictureImage(long nImageID)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030051, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nImageID);
		return result;
	}
	long DisplayFromHBitmap(long nHbitmap)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030052, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nHbitmap);
		return result;
	}
	long DisplayFromHICON(long nHICON)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6003009b, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nHICON);
		return result;
	}
	long DisplayFromClipboardData()
	{
		long result;
		InvokeHelper(0x60030053, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long DisplayFromGdiDib(long nGdiDibRef)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030054, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nGdiDibRef);
		return result;
	}
	long ZoomIN()
	{
		long result;
		InvokeHelper(0x60030055, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long ZoomOUT()
	{
		long result;
		InvokeHelper(0x60030056, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long SetZoom(float nZoomPercent)
	{
		long result;
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x60030057, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nZoomPercent);
		return result;
	}
	long get_hdc()
	{
		long result;
		InvokeHelper(0x68030029, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	BOOL get_ScrollBars()
	{
		BOOL result;
		InvokeHelper(0x68030028, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ScrollBars(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030028, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_ForceScrollBars()
	{
		BOOL result;
		InvokeHelper(0x68030027, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ForceScrollBars(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030027, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void ClearRect()
	{
		InvokeHelper(0x60030058, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	double get_ZOOM()
	{
		double result;
		InvokeHelper(0x68030026, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
		return result;
	}
	void put_ZOOM(double newValue)
	{
		static BYTE parms[] = VTS_R8 ;
		InvokeHelper(0x68030026, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long SetZoom100()
	{
		long result;
		InvokeHelper(0x60030059, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long SetZoomFitControl()
	{
		long result;
		InvokeHelper(0x6003005a, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long SetZoomWidthControl()
	{
		long result;
		InvokeHelper(0x6003005b, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long SetZoomHeightControl()
	{
		long result;
		InvokeHelper(0x6003005c, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long SetZoomControl()
	{
		long result;
		InvokeHelper(0x6003005d, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	BOOL SetLicenseNumber(LPCTSTR sKey)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x6003005e, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, sKey);
		return result;
	}
	void Copy2Clipboard()
	{
		InvokeHelper(0x6003005f, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void CopyRegion2Clipboard(long nSrcLeft, long nSrcTop, long nWidth, long nHeight)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x60030060, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nSrcLeft, nSrcTop, nWidth, nHeight);
	}
	long GetTotalFrame()
	{
		long result;
		InvokeHelper(0x60030061, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	CString GetImageFormat()
	{
		CString result;
		InvokeHelper(0x60030099, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	long Redraw()
	{
		long result;
		InvokeHelper(0x60030062, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long Rotate90()
	{
		long result;
		InvokeHelper(0x60030063, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long Rotate180()
	{
		long result;
		InvokeHelper(0x60030064, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long Rotate270()
	{
		long result;
		InvokeHelper(0x60030065, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long FlipX()
	{
		long result;
		InvokeHelper(0x60030066, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long FlipX90()
	{
		long result;
		InvokeHelper(0x60030067, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long FlipX180()
	{
		long result;
		InvokeHelper(0x60030068, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long FlipX270()
	{
		long result;
		InvokeHelper(0x60030069, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	void SetBackGroundColor(long nRGBColor)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6003006a, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nRGBColor);
	}
	long get_ImageWidth()
	{
		long result;
		InvokeHelper(0x68030025, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ImageWidth(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030025, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ImageHeight()
	{
		long result;
		InvokeHelper(0x68030024, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ImageHeight(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030024, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long GetNativeImage()
	{
		long result;
		InvokeHelper(0x6003006b, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long SetNativeImage(long nImageID)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6003006c, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nImageID);
		return result;
	}
	long GetHScrollBarMaxPosition()
	{
		long result;
		InvokeHelper(0x6003006d, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetVScrollBarMaxPosition()
	{
		long result;
		InvokeHelper(0x6003006e, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetHScrollBarPosition()
	{
		long result;
		InvokeHelper(0x6003006f, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetVScrollBarPosition()
	{
		long result;
		InvokeHelper(0x60030070, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	void SetHScrollBarPosition(long nNewHPosition)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030071, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewHPosition);
	}
	void SetVScrollBarPosition(long nNewVPosition)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030072, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewVPosition);
	}
	void SetHVScrollBarPosition(long nNewHPosition, long nNewVPosition)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x60030073, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewHPosition, nNewVPosition);
	}
	long ZoomRect()
	{
		long result;
		InvokeHelper(0x60030074, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long ZoomArea(long nLeft, long nTop, long nWidth, long nHeight)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x60030075, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nLeft, nTop, nWidth, nHeight);
		return result;
	}
	long get_MouseMode()
	{
		long result;
		InvokeHelper(0x68030023, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_MouseMode(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030023, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void CenterOnRect()
	{
		InvokeHelper(0x60030076, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	long GetMouseX()
	{
		long result;
		InvokeHelper(0x60030077, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetMouseY()
	{
		long result;
		InvokeHelper(0x60030078, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	unsigned long get_RectBorderColor()
	{
		unsigned long result;
		InvokeHelper(0x68030022, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_RectBorderColor(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x68030022, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ZoomStep()
	{
		long result;
		InvokeHelper(0x68030021, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ZoomStep(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030021, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	short get_RectBorderSize()
	{
		short result;
		InvokeHelper(0x68030020, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
		return result;
	}
	void put_RectBorderSize(short newValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x68030020, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_ClipControls()
	{
		BOOL result;
		InvokeHelper(0x6803001f, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ClipControls(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x6803001f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	short get_ScrollSmallChange()
	{
		short result;
		InvokeHelper(0x6803001e, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
		return result;
	}
	void put_ScrollSmallChange(short newValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x6803001e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long GetImageTop()
	{
		long result;
		InvokeHelper(0x60030079, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetImageLeft()
	{
		long result;
		InvokeHelper(0x6003007a, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	short get_ScrollLargeChange()
	{
		short result;
		InvokeHelper(0x6803001d, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
		return result;
	}
	void put_ScrollLargeChange(short newValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x6803001d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	double GetMaxZoom()
	{
		double result;
		InvokeHelper(0x6003007b, DISPATCH_METHOD, VT_R8, (void*)&result, NULL);
		return result;
	}
	float get_VerticalResolution()
	{
		float result;
		InvokeHelper(0x6803001c, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	void put_VerticalResolution(float newValue)
	{
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x6803001c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	float get_HorizontalResolution()
	{
		float result;
		InvokeHelper(0x6803001b, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	void put_HorizontalResolution(float newValue)
	{
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x6803001b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long GetLicenseMode()
	{
		long result;
		InvokeHelper(0x6003007c, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_PageCount()
	{
		long result;
		InvokeHelper(0x6803001a, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_PageCount(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6803001a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_CurrentPage()
	{
		long result;
		InvokeHelper(0x68030019, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_CurrentPage(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030019, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	double GetVersion()
	{
		double result;
		InvokeHelper(0x6003007d, DISPATCH_METHOD, VT_R8, (void*)&result, NULL);
		return result;
	}
	BOOL get_SilentMode()
	{
		BOOL result;
		InvokeHelper(0x68030018, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_SilentMode(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030018, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_ImageForceTemporaryMode()
	{
		BOOL result;
		InvokeHelper(0x68030017, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ImageForceTemporaryMode(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030017, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_SkipImageResolution()
	{
		BOOL result;
		InvokeHelper(0x68030016, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_SkipImageResolution(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030016, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_hwnd()
	{
		long result;
		InvokeHelper(0x68030015, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void Clear()
	{
		InvokeHelper(0x6003007e, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	BOOL get_LockControl()
	{
		BOOL result;
		InvokeHelper(0x68030014, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_LockControl(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030014, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ZoomMode()
	{
		long result;
		InvokeHelper(0x68030013, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ZoomMode(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030013, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_RectBorderStyle()
	{
		long result;
		InvokeHelper(0x68030012, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_RectBorderStyle(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030012, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_RectDrawMode()
	{
		long result;
		InvokeHelper(0x68030011, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_RectDrawMode(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030011, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_Enabled()
	{
		BOOL result;
		InvokeHelper(0x68030010, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Enabled(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030010, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_EnableMouseWheel()
	{
		BOOL result;
		InvokeHelper(0x6803000f, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_EnableMouseWheel(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x6803000f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long ExifTagCount()
	{
		long result;
		InvokeHelper(0x6003007f, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long IPTCTagCount()
	{
		long result;
		InvokeHelper(0x60030080, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	CString ExifTagGetName(long nTagNo)
	{
		CString result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030081, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, nTagNo);
		return result;
	}
	CString ExifTagGetValue(long nTagNo)
	{
		CString result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030082, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, nTagNo);
		return result;
	}
	long ExifTagGetID(long nTagNo)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030083, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nTagNo);
		return result;
	}
	long IPTCTagGetID(long nTagNo)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030084, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nTagNo);
		return result;
	}
	CString IPTCTagGetValueString(long nTagNo)
	{
		CString result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030085, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, nTagNo);
		return result;
	}
	void CoordObjectToImage(long nObjectX, long nObjectY, long * nImageX, long * nImageY)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x60030086, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nObjectX, nObjectY, nImageX, nImageY);
	}
	void CoordImageToObject(long nImageX, long nImageY, long * nObjectX, long * nObjectY)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x60030087, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nImageX, nImageY, nObjectX, nObjectY);
	}
	long get_ImageAlignment()
	{
		long result;
		InvokeHelper(0x6803000e, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ImageAlignment(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6803000e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ImagePosition()
	{
		long result;
		InvokeHelper(0x6803000d, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ImagePosition(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6803000d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_AnimateGIF()
	{
		BOOL result;
		InvokeHelper(0x6803000c, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_AnimateGIF(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x6803000c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_Appearance()
	{
		long result;
		InvokeHelper(0x6803000b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_Appearance(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6803000b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_BackStyle()
	{
		long result;
		InvokeHelper(0x6803000a, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_BackStyle(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6803000a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void Refresh()
	{
		InvokeHelper(0x60030088, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	BOOL get_ScrollOptimization()
	{
		BOOL result;
		InvokeHelper(0x68030009, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ScrollOptimization(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030009, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	float GetHeightMM()
	{
		float result;
		InvokeHelper(0x60030089, DISPATCH_METHOD, VT_R4, (void*)&result, NULL);
		return result;
	}
	float GetWidthMM()
	{
		float result;
		InvokeHelper(0x6003008a, DISPATCH_METHOD, VT_R4, (void*)&result, NULL);
		return result;
	}
	long get_ViewerQuality()
	{
		long result;
		InvokeHelper(0x68030008, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ViewerQuality(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030008, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_ViewerQualityAuto()
	{
		BOOL result;
		InvokeHelper(0x68030007, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ViewerQualityAuto(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030007, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_LicenseKEY()
	{
		CString result;
		InvokeHelper(0x68030006, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_LicenseKEY(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x68030006, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long GetHBitmap()
	{
		long result;
		InvokeHelper(0x6003008b, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	void DeleteHBitmap(long nHbitmap)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6003008c, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nHbitmap);
	}
	BOOL get_ForcePictureMode()
	{
		BOOL result;
		InvokeHelper(0x68030005, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ForcePictureMode(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030005, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_KeepImagePosition()
	{
		BOOL result;
		InvokeHelper(0x68030004, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_KeepImagePosition(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030004, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_MouseWheelMode()
	{
		long result;
		InvokeHelper(0x68030003, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_MouseWheelMode(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030003, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_ViewerDrop()
	{
		BOOL result;
		InvokeHelper(0x68030002, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ViewerDrop(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030002, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_DisableAutoFocus()
	{
		BOOL result;
		InvokeHelper(0x68030001, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_DisableAutoFocus(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030001, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long GetStat()
	{
		long result;
		InvokeHelper(0x6003008d, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	void SetMouseIcon(LPCTSTR sIconPath)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x6003008e, DISPATCH_METHOD, VT_EMPTY, NULL, parms, sIconPath);
	}
	unsigned long get_ImageMaskColor()
	{
		unsigned long result;
		InvokeHelper(0x68030092, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_ImageMaskColor(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x68030092, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	float get_gamma()
	{
		float result;
		InvokeHelper(0x68030091, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	void put_gamma(float newValue)
	{
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x68030091, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_RectIsEditable()
	{
		BOOL result;
		InvokeHelper(0x68030095, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_RectIsEditable(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030095, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_ContinuousViewMode()
	{
		BOOL result;
		InvokeHelper(0x68030096, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ContinuousViewMode(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x68030096, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_MouseButtonForMouseMode()
	{
		long result;
		InvokeHelper(0x68030097, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_MouseButtonForMouseMode(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030097, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_OptimizeDrawingSpeed()
	{
		BOOL result;
		InvokeHelper(0x6803009e, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_OptimizeDrawingSpeed(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x6803009e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_VScrollVisible()
	{
		BOOL result;
		InvokeHelper(0x6803009d, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_VScrollVisible(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x6803009d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_HScrollVisible()
	{
		BOOL result;
		InvokeHelper(0x6803009c, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_HScrollVisible(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x6803009c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}


};
