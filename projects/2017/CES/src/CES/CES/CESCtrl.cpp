// CESCtrl.cpp : CCESCtrl ActiveX �ؼ����ʵ�֡�

#include "stdafx.h"
#include "CES.h"
#include "CESCtrl.h"
#include "CESPropPage.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCESCtrl, COleControl)

// ��Ϣӳ��

BEGIN_MESSAGE_MAP(CCESCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

// ����ӳ��

BEGIN_DISPATCH_MAP(CCESCtrl, COleControl)
	DISP_FUNCTION_ID(CCESCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

// �¼�ӳ��

BEGIN_EVENT_MAP(CCESCtrl, COleControl)
END_EVENT_MAP()

// ����ҳ

// TODO: ������Ҫ���Ӹ�������ҳ�����ס���Ӽ���!
BEGIN_PROPPAGEIDS(CCESCtrl, 1)
	PROPPAGEID(CCESPropPage::guid)
END_PROPPAGEIDS(CCESCtrl)

// ��ʼ���๤���� guid

IMPLEMENT_OLECREATE_EX(CCESCtrl, "CES.CESCtrl.1",
	0x360fb522, 0xfe3e, 0x42cc, 0xb5, 0x6d, 0xca, 0x76, 0x7f, 0x41, 0x99, 0x87)

// ����� ID �Ͱ汾

IMPLEMENT_OLETYPELIB(CCESCtrl, _tlid, _wVerMajor, _wVerMinor)

// �ӿ� ID

const IID IID_DCES = { 0x799CD45C, 0x8E8A, 0x4743, { 0x86, 0xF3, 0xDA, 0x47, 0xA5, 0x83, 0x77, 0x2F } };
const IID IID_DCESEvents = { 0x730372C9, 0xD823, 0x4353, { 0x9C, 0x30, 0x23, 0xD1, 0xE6, 0xA4, 0x56, 0xD9 } };

// �ؼ�������Ϣ

static const DWORD _dwCESOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CCESCtrl, IDS_CES, _dwCESOleMisc)

// CCESCtrl::CCESCtrlFactory::UpdateRegistry -
// ���ӻ��Ƴ� CCESCtrl ��ϵͳע�����

BOOL CCESCtrl::CCESCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO:  ��֤���Ŀؼ��Ƿ���ϵ�Ԫģ���̴߳�������
	// �йظ�����Ϣ����ο� MFC ����˵�� 64��
	// ������Ŀؼ������ϵ�Ԫģ�͹�����
	// �����޸����´��룬��������������
	// afxRegInsertable | afxRegApartmentThreading ��Ϊ afxRegInsertable��

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_CES,
			IDB_CES,
			afxRegInsertable | afxRegApartmentThreading,
			_dwCESOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


// CCESCtrl::CCESCtrl - ���캯��

CCESCtrl::CCESCtrl()
{
	InitializeIIDs(&IID_DCES, &IID_DCESEvents);
	//__debugbreak();
	// TODO:  �ڴ˳�ʼ���ؼ���ʵ�����ݡ�
	_cameraPipeline = WRL::Make<CES::CameraPipeline>();
}

// CCESCtrl::~CCESCtrl - ��������

CCESCtrl::~CCESCtrl()
{
	// TODO:  �ڴ������ؼ���ʵ�����ݡ�
}

// CCESCtrl::OnDraw - ��ͼ����

void CCESCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& /* rcInvalid */)
{
	if (!pdc)
		return;

	// TODO:  �����Լ��Ļ�ͼ�����滻����Ĵ��롣
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
	pdc->Ellipse(rcBounds);
	static bool loaded = false;
	if (!loaded)
	{
		_cameraPipeline->OpenCamera(CES::CameraSource::Camera, m_hWnd);
		loaded = true;
	}
}

// CCESCtrl::DoPropExchange - �־���֧��

void CCESCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Ϊÿ���־õ��Զ������Ե��� PX_ ������
}


// CCESCtrl::GetControlFlags -
// �Զ��� MFC �� ActiveX �ؼ�ʵ�ֵı�־��
//
DWORD CCESCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// �ڻ�Ͳ��״̬֮�����ת��ʱ��
	// �������»��ƿؼ���
	dwFlags |= noFlickerActivate;
	return dwFlags;
}


// CCESCtrl::OnResetState - ���ؼ�����ΪĬ��״̬

void CCESCtrl::OnResetState()
{
	COleControl::OnResetState();  // ���� DoPropExchange ���ҵ���Ĭ��ֵ

	// TODO:  �ڴ��������������ؼ�״̬��
}


// CCESCtrl::AboutBox - ���û���ʾ�����ڡ���

void CCESCtrl::AboutBox()
{
	CDialogEx dlgAbout(IDD_ABOUTBOX_CES);
	dlgAbout.DoModal();
}


// CCESCtrl ��Ϣ��������