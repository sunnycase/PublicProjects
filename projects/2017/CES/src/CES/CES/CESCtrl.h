#pragma once
#include "CameraPipeline.h"

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
	};
private:
	WRL::ComPtr<CES::CameraPipeline> _cameraPipeline;
};

