#pragma once
#include "CameraPipeline.h"
#include "VideoBox.h"
#include "ImageBox.h"
#include "ImageStorage.h"
#include "Uploader.h"

// CESCtrl.h : CCESCtrl ActiveX 控件类的声明。


// CCESCtrl : 有关实现的信息，请参阅 CESCtrl.cpp。

class CCESCtrl : public COleControl
{
	DECLARE_DYNCREATE(CCESCtrl)

// 构造函数
public:
	CCESCtrl();

// 重写
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();

// 实现
protected:
	~CCESCtrl();

	DECLARE_OLECREATE_EX(CCESCtrl)    // 类工厂和 guid
	DECLARE_OLETYPELIB(CCESCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CCESCtrl)     // 属性页 ID
	DECLARE_OLECTLTYPE(CCESCtrl)		// 类型名称和杂项状态

// 消息映射
	DECLARE_MESSAGE_MAP()

// 调度映射
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// 事件映射
	DECLARE_EVENT_MAP()

// 调度和事件 ID
public:
	enum {
		dispidShowPropertyPages = 12L,
		dispidCameraDeviceId = 11,
		dispidScannerDeviceId = 10,
		dispidUploadCurrentPicture = 9L,
		dispidDisplayPicture = 8L,
		dispidSetScanToPath = 7L,
		dispidGetImageStorageTree = 6L,
		dispidInitializeBusiness = 5L,
		dispidZoom = 4,
		dispidRotation = 3,
		dispidTakePicture = 2L,
		dispidStartScanning = 1L
	};
private:
	enum class ViewState
	{
		Video,
		Image
	};

	void SetViewState(ViewState state);
	
private:
	WRL::ComPtr<CES::CameraPipeline> _cameraPipeline;
	CVideoBox _videoBox;
	CImageBox _imageWnd;
	CES::ImageStorage _imageStorage;
	std::wstring _currentPictureFileName;
	CES::Uploader _uploader;
	CES::CameraSource _cameraSource;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	void StartScanning();
	BSTR TakePicture();
	ULONG GetRotation();
	void SetRotation(ULONG newVal);
	FLOAT GetZoom();
	void SetZoom(FLOAT newVal);
	void InitializeBusiness(USHORT busType, LPCTSTR seqId, LPCTSTR workflowId, USHORT storageType, VARIANT_BOOL useZip, LPCTSTR categories, LPCTSTR uploadIp, LPCTSTR uploadUri);
	BSTR GetImageStorageTree();
	void SetScanToPath(LPCTSTR path);
	void DisplayPicture(LPCTSTR uri);
	void UploadCurrentPicture();
	void OnScannerDeviceIdChanged();
	CString m_ScannerDeviceId;
	void OnCameraDeviceIdChanged();
	CString m_CameraDeviceId;
	void ShowPropertyPages();
};

