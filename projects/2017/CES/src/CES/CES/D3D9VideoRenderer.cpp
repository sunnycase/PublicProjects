#include "stdafx.h"
#include "D3D9VideoRenderer.h"
#include <optional>

using namespace WRL;
using namespace CES;

#define LOCK_STATE() auto locker = _stateLock.Lock()

CES::D3D9VideoRenderer::D3D9VideoRenderer()
{
	ThrowIfFailed(Direct3DCreate9Ex(D3D_SDK_VERSION, &_d3d9));
	ThrowIfFailed(DXVA2CreateDirect3DDeviceManager9(&_resetToken, &_deviceManager));

	CreateDeviceIndependentResources();
	CreateDeviceDependentResources();
}

void CES::D3D9VideoRenderer::SetFullscreen(bool value)
{
	_isFullscreen = value;
}

void CES::D3D9VideoRenderer::SetVideoWindow(HWND hWnd)
{
	_videoHWnd = hWnd;
}

void CES::D3D9VideoRenderer::SetDestinationRect(const RECT & rect)
{
	_destRect = rect;
}

STDMETHODIMP CES::D3D9VideoRenderer::GetService(REFGUID guidService, REFIID riid, LPVOID * ppvObject)
{
	return _deviceManager.CopyTo(riid, ppvObject);
}

namespace
{
	UINT SelectAdapter(IDirect3D9Ex* d3d, HMONITOR monitor)
	{
		std::optional<UINT> adapterId;
		auto adapterCount = d3d->GetAdapterCount();
		for (UINT i = 0; i < adapterCount; i++)
		{
			auto adapterMonitor = d3d->GetAdapterMonitor(i);
			if (adapterMonitor == nullptr)
				break;
			if (adapterMonitor == monitor)
			{
				adapterId.emplace(i);
				break;
			}
		}
		if (adapterId)
			return adapterId.value();
		ThrowAlways(L"Cannot find appropriate adapter.");
	}

	const D3DFORMAT VIDEO_MAIN_FORMAT = D3DFMT_YUY2;
	const D3DFORMAT VIDEO_SUB_FORMAT = D3DFORMAT('VUYA'); // AYUV
	const DWORD DXVA_RENDER_TARGET = DXVA2_VideoProcessorRenderTarget;
}

void CES::D3D9VideoRenderer::CreateDeviceIndependentResources()
{
	DXVA2_AYUVSample16 color = { 0x8000, 0x8000, 0x1000, 0xffff };

	static const DXVA2_ExtendedFormat format = { 
		DXVA2_SampleProgressiveFrame,           // SampleFormat
		DXVA2_VideoChromaSubsampling_MPEG2,     // VideoChromaSubsampling
		DXVA2_NominalRange_Normal,              // NominalRange
		DXVA2_VideoTransferMatrix_BT709,        // VideoTransferMatrix
		DXVA2_VideoLighting_dim,                // VideoLighting
		DXVA2_VideoPrimaries_BT709,             // VideoPrimaries
		DXVA2_VideoTransFunc_709                // VideoTransferFunction            
	};

	_videoDesc.SampleFormat = format;
	_videoDesc.SampleWidth = 256;
	_videoDesc.SampleHeight = 256;
	_videoDesc.InputSampleFreq.Numerator = 60;
	_videoDesc.InputSampleFreq.Denominator = 1;
	_videoDesc.OutputFrameFreq.Numerator = 60;
	_videoDesc.OutputFrameFreq.Denominator = 1;
	_videoDesc.Format = VIDEO_MAIN_FORMAT;
}

void CES::D3D9VideoRenderer::CreateDeviceDependentResources()
{
	LOCK_STATE();

	UINT adapterId = D3DADAPTER_DEFAULT;
	auto hWnd = _videoHWnd ? _videoHWnd : GetDesktopWindow();
	if (_videoHWnd)
	{
		auto monitor = MonitorFromWindow(_videoHWnd, MONITOR_DEFAULTTONEAREST);
		adapterId = SelectAdapter(_d3d9.Get(), monitor);
	}
	D3DDISPLAYMODE displayMode;
	ThrowIfFailed(_d3d9->GetAdapterDisplayMode(adapterId, &displayMode));
#if _DEBUG

	D3DADAPTER_IDENTIFIER9 adapterIdentifier;
	TCHAR strGUID[50];
	char adapterName[256]{};

	if (_d3d9->GetAdapterIdentifier(adapterId, 0, &adapterIdentifier) == S_OK) {
		if ((::StringFromGUID2(adapterIdentifier.DeviceIdentifier, strGUID, 50) > 0)) {
			strncpy_s(adapterName, adapterIdentifier.Description, MAX_DEVICE_IDENTIFIER_STRING);
			TRACE((L"Adapter Found: %S", adapterName));
		}
	}
#endif
	D3DCAPS9 deviceCaps;
	ThrowIfFailed(_d3d9->GetDeviceCaps(adapterId, D3DDEVTYPE_HAL, &deviceCaps));
	DWORD vp;
	if (deviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS pparams{};
	pparams.Windowed = TRUE;
	pparams.hDeviceWindow = hWnd;
	pparams.SwapEffect = D3DSWAPEFFECT_COPY;
	pparams.BackBufferCount = 1;
	pparams.Flags = D3DPRESENTFLAG_VIDEO;
	pparams.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	pparams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	ThrowIfFailed(_d3d9->CreateDeviceEx(adapterId, D3DDEVTYPE_HAL, hWnd, vp | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
		&pparams, nullptr, &_d3dDevice));
	ThrowIfFailed(_d3dDevice->ResetEx(&pparams, nullptr));
	ThrowIfFailed(_deviceManager->ResetDevice(_d3dDevice.Get(), _resetToken));

	ThrowIfFailed(DXVA2CreateVideoService(_d3dDevice.Get(), IID_PPV_ARGS(&_videoService)));

	unique_cotaskmem<GUID> deviceGuids;
	UINT deviceGuidCount;
	ThrowIfFailed(_videoService->GetVideoProcessorDeviceGuids(&_videoDesc, &deviceGuidCount, &deviceGuids._Myptr()));
}
