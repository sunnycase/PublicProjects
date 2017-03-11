// CESPropPage.cpp : CCESPropPage 属性页类的实现。

#include "stdafx.h"
#include "CES.h"
#include "CESPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCESPropPage, COlePropertyPage)

// 消息映射

BEGIN_MESSAGE_MAP(CCESPropPage, COlePropertyPage)
END_MESSAGE_MAP()

// 初始化类工厂和 guid

IMPLEMENT_OLECREATE_EX(CCESPropPage, "CES.CESPropPage.1",
	0x25c3824e, 0x7641, 0x45cf, 0x87, 0xe4, 0x2e, 0xb6, 0x4b, 0x61, 0x7a, 0xd7)

// CCESPropPage::CCESPropPageFactory::UpdateRegistry -
// 添加或移除 CCESPropPage 的系统注册表项

BOOL CCESPropPage::CCESPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_CES_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}

// CCESPropPage::CCESPropPage - 构造函数

CCESPropPage::CCESPropPage() :
	COlePropertyPage(IDD, IDS_CES_PPG_CAPTION)
{
}

// CCESPropPage::DoDataExchange - 在页和属性间移动数据

void CCESPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}

// CCESPropPage 消息处理程序
