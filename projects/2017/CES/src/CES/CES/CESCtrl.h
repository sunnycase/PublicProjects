#pragma once
#include "CameraPipeline.h"
#include "VideoBox.h"
#include "ImageBox.h"
#include "ImageStorage.h"
#include "Uploader.h"

// CESCtrl.h : CCESCtrl ActiveX �ؼ����������


// CCESCtrl : �й�ʵ�ֵ���Ϣ������� CESCtrl.cpp��

class CCESCtrl : public COleControl
{
	DECLARE_DYNCREATE(CCESCtrl)

// ���캯��
public:
	CCESCtrl();

// ��д
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();

// ʵ��
protected:
	~CCESCtrl();

	DECLARE_OLECREATE_EX(CCESCtrl)    // �๤���� guid
	DECLARE_OLETYPELIB(CCESCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CCESCtrl)     // ����ҳ ID
	DECLARE_OLECTLTYPE(CCESCtrl)		// �������ƺ�����״̬

// ��Ϣӳ��
	DECLARE_MESSAGE_MAP()

// ����ӳ��
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// �¼�ӳ��
	DECLARE_EVENT_MAP()

// ���Ⱥ��¼� ID
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

