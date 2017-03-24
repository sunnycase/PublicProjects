#pragma once
#include "CameraPipeline.h"
#include "VideoBox.h"
#include "ImageBox.h"

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

