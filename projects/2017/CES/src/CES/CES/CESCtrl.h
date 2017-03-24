#pragma once
#include "CameraPipeline.h"
#include "VideoBox.h"
#include "ImageBox.h"

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
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	void StartScanning();
	void TakePicture();
	ULONG GetRotation();
	void SetRotation(ULONG newVal);
	FLOAT GetZoom();
	void SetZoom(FLOAT newVal);
};

