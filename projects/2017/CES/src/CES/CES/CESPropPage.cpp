// CESPropPage.cpp : CCESPropPage ����ҳ���ʵ�֡�

#include "stdafx.h"
#include "CES.h"
#include "CESPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCESPropPage, COlePropertyPage)

// ��Ϣӳ��

BEGIN_MESSAGE_MAP(CCESPropPage, COlePropertyPage)
END_MESSAGE_MAP()

// ��ʼ���๤���� guid

IMPLEMENT_OLECREATE_EX(CCESPropPage, "CES.CESPropPage.1",
	0x25c3824e, 0x7641, 0x45cf, 0x87, 0xe4, 0x2e, 0xb6, 0x4b, 0x61, 0x7a, 0xd7)

// CCESPropPage::CCESPropPageFactory::UpdateRegistry -
// ��ӻ��Ƴ� CCESPropPage ��ϵͳע�����

BOOL CCESPropPage::CCESPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_CES_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}

// CCESPropPage::CCESPropPage - ���캯��

CCESPropPage::CCESPropPage() :
	COlePropertyPage(IDD, IDS_CES_PPG_CAPTION)
{
}

// CCESPropPage::DoDataExchange - ��ҳ�����Լ��ƶ�����

void CCESPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}

// CCESPropPage ��Ϣ�������
