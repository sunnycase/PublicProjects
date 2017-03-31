// CESPropPage.cpp : CCESPropPage 属性页类的实现。

#include "stdafx.h"
#include "CES.h"
#include "CESPropPage.h"
#include "CameraPipeline.h"

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
	, m_selectedPrimCamIndex(-1)
	, m_selectedSecndCamIndex(-1)
{
	
}

// CCESPropPage::DoDataExchange - 在页和属性间移动数据

void CCESPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_CB_PRIMCAM, m_cbPrimCam);
	DDX_Control(pDX, IDC_CB_SECNDCAM, m_cbSecndCam);
	DDX_CBIndex(pDX, IDC_CB_PRIMCAM, m_selectedPrimCamIndex);
	DDX_CBIndex(pDX, IDC_CB_SECNDCAM, m_selectedSecndCamIndex);

	CString scannerId, cameraId;
	if (!_indexToSymbolicLink.empty())
	{
		if (m_selectedPrimCamIndex != -1)
			scannerId = _indexToSymbolicLink[m_selectedPrimCamIndex];
		if (m_selectedSecndCamIndex != -1)
			cameraId = _indexToSymbolicLink[m_selectedSecndCamIndex];
	}
	DDP_Text(pDX, IDC_CB_PRIMCAM, scannerId, L"ScannerDeviceId");
	DDP_Text(pDX, IDC_CB_SECNDCAM, cameraId, L"CameraDeviceId");
	if (!_symbolicToIndexLink.empty())
	{
		auto primIt = _symbolicToIndexLink.find(scannerId.GetBuffer());
		if (primIt != _symbolicToIndexLink.end())
			m_selectedPrimCamIndex = primIt->second;

		auto secndIt = _symbolicToIndexLink.find(cameraId.GetBuffer());
		if (secndIt != _symbolicToIndexLink.end())
			m_selectedSecndCamIndex = secndIt->second;
	}
	DDX_CBIndex(pDX, IDC_CB_PRIMCAM, m_selectedPrimCamIndex);
	DDX_CBIndex(pDX, IDC_CB_SECNDCAM, m_selectedSecndCamIndex);

	DDP_PostProcessing(pDX);
}

// CCESPropPage 消息处理程序


BOOL CCESPropPage::OnInitDialog()
{
	COlePropertyPage::OnInitDialog();
	m_cbPrimCam.Clear();
	m_cbSecndCam.Clear();
	_indexToSymbolicLink.clear();
	_symbolicToIndexLink.clear();

	const auto devices = CES::CameraPipeline::EnumerateDevices();

	for (auto&& device : devices)
	{
		auto index = m_cbPrimCam.AddString(device.FriendlyName.c_str());
		m_cbSecndCam.AddString(device.FriendlyName.c_str());
		_indexToSymbolicLink.emplace(index, device.SymbolicLink.c_str());
		_symbolicToIndexLink.emplace(device.SymbolicLink, index);
	}

	return TRUE;
}


void CCESPropPage::OnOK()
{
	auto scannerDeviceIdx = m_cbPrimCam.GetCurSel();
	auto cameraDeviceIdx = m_cbSecndCam.GetCurSel();
	if (scannerDeviceIdx == -1 || cameraDeviceIdx == -1)
	{
		MessageBox(L"必须选择正确的设备。", nullptr, MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	else
	{
		COlePropertyPage::OnOK();
	}
}
