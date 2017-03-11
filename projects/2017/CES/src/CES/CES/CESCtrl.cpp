// CESCtrl.cpp : CCESCtrl ActiveX 控件类的实现。

#include "stdafx.h"
#include "CES.h"
#include "CESCtrl.h"
#include "CESPropPage.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCESCtrl, COleControl)

// 消息映射

BEGIN_MESSAGE_MAP(CCESCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

// 调度映射

BEGIN_DISPATCH_MAP(CCESCtrl, COleControl)
	DISP_FUNCTION_ID(CCESCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

// 事件映射

BEGIN_EVENT_MAP(CCESCtrl, COleControl)
END_EVENT_MAP()

// 属性页

// TODO: 根据需要添加更多属性页。请记住增加计数!
BEGIN_PROPPAGEIDS(CCESCtrl, 1)
	PROPPAGEID(CCESPropPage::guid)
END_PROPPAGEIDS(CCESCtrl)

// 初始化类工厂和 guid

IMPLEMENT_OLECREATE_EX(CCESCtrl, "CES.CESCtrl.1",
	0x360fb522, 0xfe3e, 0x42cc, 0xb5, 0x6d, 0xca, 0x76, 0x7f, 0x41, 0x99, 0x87)

// 键入库 ID 和版本

IMPLEMENT_OLETYPELIB(CCESCtrl, _tlid, _wVerMajor, _wVerMinor)

// 接口 ID

const IID IID_DCES = { 0x799CD45C, 0x8E8A, 0x4743, { 0x86, 0xF3, 0xDA, 0x47, 0xA5, 0x83, 0x77, 0x2F } };
const IID IID_DCESEvents = { 0x730372C9, 0xD823, 0x4353, { 0x9C, 0x30, 0x23, 0xD1, 0xE6, 0xA4, 0x56, 0xD9 } };

// 控件类型信息

static const DWORD _dwCESOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CCESCtrl, IDS_CES, _dwCESOleMisc)

// CCESCtrl::CCESCtrlFactory::UpdateRegistry -
// 添加或移除 CCESCtrl 的系统注册表项

BOOL CCESCtrl::CCESCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO:  验证您的控件是否符合单元模型线程处理规则。
	// 有关更多信息，请参考 MFC 技术说明 64。
	// 如果您的控件不符合单元模型规则，则
	// 必须修改如下代码，将第六个参数从
	// afxRegInsertable | afxRegApartmentThreading 改为 afxRegInsertable。

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


// CCESCtrl::CCESCtrl - 构造函数

CCESCtrl::CCESCtrl()
{
	InitializeIIDs(&IID_DCES, &IID_DCESEvents);
	//__debugbreak();
	// TODO:  在此初始化控件的实例数据。
	_cameraPipeline = WRL::Make<CES::CameraPipeline>();
}

// CCESCtrl::~CCESCtrl - 析构函数

CCESCtrl::~CCESCtrl()
{
	// TODO:  在此清理控件的实例数据。
}

// CCESCtrl::OnDraw - 绘图函数

void CCESCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& /* rcInvalid */)
{
	if (!pdc)
		return;

	// TODO:  用您自己的绘图代码替换下面的代码。
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
	pdc->Ellipse(rcBounds);
	static bool loaded = false;
	if (!loaded)
	{
		_cameraPipeline->OpenCamera(CES::CameraSource::Camera, m_hWnd);
		loaded = true;
	}
}

// CCESCtrl::DoPropExchange - 持久性支持

void CCESCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: 为每个持久的自定义属性调用 PX_ 函数。
}


// CCESCtrl::GetControlFlags -
// 自定义 MFC 的 ActiveX 控件实现的标志。
//
DWORD CCESCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// 在活动和不活动状态之间进行转换时，
	// 不会重新绘制控件。
	dwFlags |= noFlickerActivate;
	return dwFlags;
}


// CCESCtrl::OnResetState - 将控件重置为默认状态

void CCESCtrl::OnResetState()
{
	COleControl::OnResetState();  // 重置 DoPropExchange 中找到的默认值

	// TODO:  在此重置任意其他控件状态。
}


// CCESCtrl::AboutBox - 向用户显示“关于”框

void CCESCtrl::AboutBox()
{
	CDialogEx dlgAbout(IDD_ABOUTBOX_CES);
	dlgAbout.DoModal();
}


// CCESCtrl 消息处理程序
