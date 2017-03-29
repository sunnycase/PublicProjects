// CESCtrl.cpp : CCESCtrl ActiveX 控件类的实现。

#include "stdafx.h"
#include "CES.h"
#include "CESCtrl.h"
#include "CESPropPage.h"
#include "afxdialogex.h"
#include <atlsafe.h>
#include <filesystem>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCESCtrl, COleControl)

// 消息映射

BEGIN_MESSAGE_MAP(CCESCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// 调度映射

BEGIN_DISPATCH_MAP(CCESCtrl, COleControl)
	DISP_FUNCTION_ID(CCESCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CCESCtrl, "StartScanning", dispidStartScanning, StartScanning, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CCESCtrl, "TakePicture", dispidTakePicture, TakePicture, VT_BSTR, VTS_NONE)
	DISP_PROPERTY_EX_ID(CCESCtrl, "Rotation", dispidRotation, GetRotation, SetRotation, VT_UI4)
	DISP_PROPERTY_EX_ID(CCESCtrl, "Zoom", dispidZoom, GetZoom, SetZoom, VT_R4)
	DISP_FUNCTION_ID(CCESCtrl, "InitializeBusiness", dispidInitializeBusiness, InitializeBusiness, VT_EMPTY, VTS_UI2 VTS_BSTR VTS_BSTR VTS_UI2 VTS_BOOL VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CCESCtrl, "GetImageStorageTree", dispidGetImageStorageTree, GetImageStorageTree, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CCESCtrl, "SetScanToPath", dispidSetScanToPath, SetScanToPath, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION_ID(CCESCtrl, "DisplayPicture", dispidDisplayPicture, DisplayPicture, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION_ID(CCESCtrl, "UploadCurrentPicture", dispidUploadCurrentPicture, UploadCurrentPicture, VT_EMPTY, VTS_NONE)
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

	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
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

void CCESCtrl::SetViewState(ViewState state)
{
	if (state == ViewState::Video)
	{
		_videoBox.ShowWindow(SW_SHOW);
		_imageWnd.ShowWindow(SW_HIDE);
	}
	else
	{
		_videoBox.ShowWindow(SW_HIDE);
		_imageWnd.ShowWindow(SW_SHOW);
	}
}

int CCESCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	RECT rect;
	GetClientRect(&rect);
	ThrowIfNot(_videoBox.Create(nullptr, WS_CHILD | WS_VISIBLE, rect, this), L"cannot init window.");
	ThrowIfNot(_imageWnd.Create(nullptr, nullptr, WS_CHILD, rect, this, 65535), L"cannot init window.");
	return 0;
}


void CCESCtrl::StartScanning()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!_cameraPipeline)
	{
		_cameraPipeline = WRL::Make<CES::CameraPipeline>();
		_cameraPipeline->DeviceReady.Subscribe([=] { _cameraPipeline->Start(); });

		_cameraPipeline->OpenCamera(CES::CameraSource::Scanner, _videoBox.GetSafeHwnd());
	}

	SetViewState(ViewState::Video);
}

void CCESCtrl::OnSize(UINT nType, int cx, int cy)
{
	COleControl::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED)
	{
		//需要判断一下，是最小化则退出。
		//如果是最小化，恢复的时候会BUG，因为整数除以0
		return;
	}

	RECT rect;
	GetClientRect(&rect);
	_videoBox.MoveWindow(&rect);
	if (_cameraPipeline)
		_cameraPipeline->OnResize(_videoBox.GetSafeHwnd());
	_imageWnd.MoveWindow(&rect);
}


BSTR CCESCtrl::TakePicture()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!_cameraPipeline)
		ThrowIfFailed(E_NOT_VALID_STATE);

	CBitmap bitmap;
	_cameraPipeline->TakePicture(bitmap);
	_imageWnd.SetPicture(bitmap);

	auto filePath = _imageStorage.GetNextAvailableFileName();
	_imageWnd.SaveAs(filePath);
	_currentPictureFileName = filePath;
	SetViewState(ViewState::Image);
	return _bstr_t(std::experimental::filesystem::path(filePath).filename().c_str()).Detach();
}


ULONG CCESCtrl::GetRotation()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码

	return 0;
}


void CCESCtrl::SetRotation(ULONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_imageWnd.Rotate(newVal);

	SetModifiedFlag();
}


FLOAT CCESCtrl::GetZoom()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码

	return 0;
}


void CCESCtrl::SetZoom(FLOAT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_imageWnd.SetZoom(newVal);

	SetModifiedFlag();
}


void CCESCtrl::InitializeBusiness(USHORT busType, LPCTSTR seqId, LPCTSTR workflowId, USHORT storageType, VARIANT_BOOL useZip, LPCTSTR categories, LPCTSTR uploadIp, LPCTSTR uploadUri)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_imageStorage.Initialize(storageType == 0 ? workflowId : seqId, categories);
}


BSTR CCESCtrl::GetImageStorageTree()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	auto str = _imageStorage.GetStorageJson();
	return _bstr_t(str.c_str()).Detach();
}

void CCESCtrl::SetScanToPath(LPCTSTR path)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	_imageStorage.SetSelectedPath({ path, SysStringLen(const_cast<BSTR>(path)) });
}


void CCESCtrl::DisplayPicture(LPCTSTR uri)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	auto path = _currentPictureFileName = _imageStorage.GetFullPath(uri);
	_imageWnd.SetPicture(path);
	SetViewState(ViewState::Image);
}


void CCESCtrl::UploadCurrentPicture()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	auto stream = _imageWnd.SaveToStream();
	_uploader.Upload(stream.Get(), std::experimental::filesystem::path(_currentPictureFileName));
}
