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

auto D3D9VideoRenderer::GetDeviceState() -> std::pair<HRESULT, DeviceState>
{
	LOCK_STATE();
	auto hr = _d3dDevice->CheckDeviceState(_videoHWnd);
	auto state = DeviceState::Ok;
	switch (hr)
	{
	case S_OK:
	case S_PRESENT_OCCLUDED:
	case S_PRESENT_MODE_CHANGED:
		hr = S_OK;
		break; 
	case D3DERR_DEVICELOST:
	case D3DERR_DEVICEHUNG:
		CreateDeviceDependentResources();
		state = DeviceState::Reset;
		hr = S_OK;
		break;
	case D3DERR_DEVICEREMOVED:
		state = DeviceState::Removed;
		break;
	default:
		hr = S_OK;
		break;
	}
	return std::make_pair(hr, state);
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
	
	const D3DFORMAT VIDEO_RENDER_TARGET_FORMAT = D3DFMT_X8R8G8B8;
	const UINT SUB_STREAM_COUNT = 1;
	const UINT DWM_BUFFER_COUNT = 4;
	const BYTE DEFAULT_PLANAR_ALPHA_VALUE = 0xFF;

	const UINT VIDEO_REQUIED_OP = DXVA2_VideoProcess_YUV2RGB |
		DXVA2_VideoProcess_StretchX |
		DXVA2_VideoProcess_StretchY |
		DXVA2_VideoProcess_SubRects |
		DXVA2_VideoProcess_SubStreams;

	const DWORD PRESENTER_BUFFER_COUNT = 3;
}

void CES::D3D9VideoRenderer::CreateVideoSamples(IMFMediaType * mediaType, SamplePool& samplePool)
{
	ThrowIfNot(_videoHWnd, L"Vidoe window is not set.");

	LOCK_STATE();
	UINT32 width, height;
	ThrowIfFailed(MFGetAttributeSize(mediaType, MF_MT_FRAME_SIZE, &width, &height));

	_mixerSurfaces.clear();
	_mixerSurfaces.resize(PRESENTER_BUFFER_COUNT);
	ThrowIfFailed(_videoService->CreateSurface(width, height, PRESENTER_BUFFER_COUNT - 1, VIDEO_RENDER_TARGET_FORMAT,
		_videoProcessorCaps.InputPool, 0, DXVA_RENDER_TARGET, reinterpret_cast<IDirect3DSurface9**>(_mixerSurfaces.data()), nullptr));

	static const auto blackColor = D3DCOLOR_ARGB(0xFF, 0x00, 0x00, 0x00);
	std::vector<WRL::ComPtr<IMFSample>> samples;
	samples.reserve(PRESENTER_BUFFER_COUNT);
	for (auto& surface : _mixerSurfaces)
	{
		ThrowIfFailed(_d3dDevice->ColorFill(surface.Get(), nullptr, blackColor));
		ComPtr<IMFSample> sample;
		ThrowIfFailed(MFCreateVideoSampleFromSurface(surface.Get(), &sample));
		ThrowIfFailed(sample->SetUINT32(MFSampleExtension_CleanPoint, 0));
		samples.emplace_back(std::move(sample));
	}
	samplePool.AssignSamples(samples.begin(), samples.end());
}

void CES::D3D9VideoRenderer::PresentSample(IMFSample * sample, MFTIME hnsTarget, MFTIME hnsTimeDelta, size_t remainingInQueue, MFTIME hnsFrameDurationDiv4)
{
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
	ThrowIfFailed(_d3d9->GetAdapterDisplayMode(adapterId, &_displayMode));
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

	unique_cotaskmem_arr<GUID> deviceGuids;
	UINT deviceGuidCount;
	ThrowIfFailed(_videoService->GetVideoProcessorDeviceGuids(&_videoDesc, &deviceGuidCount, &deviceGuids._Myptr()));

	if (deviceGuidCount)
	{
		ThrowIfFailed(_videoService->CreateVideoProcessor(deviceGuids[0], &_videoDesc, VIDEO_RENDER_TARGET_FORMAT,
			SUB_STREAM_COUNT, &_dxVideoProcessor));
		ThrowIfFailed(_videoService->GetVideoProcessorCaps(deviceGuids[0], &_videoDesc, VIDEO_RENDER_TARGET_FORMAT,
			&_videoProcessorCaps));

		{
			UINT rtCount;
			unique_cotaskmem_arr<D3DFORMAT> formats;
			ThrowIfFailed(_videoService->GetVideoProcessorRenderTargets(deviceGuids[0], &_videoDesc, &rtCount, &formats._Myptr()));
			ThrowIfNot(std::find(formats.get(), formats.get() + rtCount, VIDEO_RENDER_TARGET_FORMAT) != formats.get() + rtCount,
				L"GetVideoProcessorRenderTargets doesn't support that format.");
		}
		{
			UINT rtCount;
			unique_cotaskmem_arr<D3DFORMAT> formats;
			ThrowIfFailed(_videoService->GetVideoProcessorSubStreamFormats(deviceGuids[0], &_videoDesc, 
				VIDEO_RENDER_TARGET_FORMAT, &rtCount, &formats._Myptr()));
			ThrowIfNot(std::find(formats.get(), formats.get() + rtCount, VIDEO_SUB_FORMAT) != formats.get() + rtCount,
				L"GetVideoProcessorRenderTargets doesn't support that format.");
			_videoSubFormat = D3DFMT_A8R8G8B8;
		}
	}
	else
	{
		ThrowIfFailed(_videoService->CreateVideoProcessor(DXVA2_VideoProcProgressiveDevice, &_videoDesc,
			VIDEO_RENDER_TARGET_FORMAT, SUB_STREAM_COUNT, &_dxVideoProcessor));
		ThrowIfFailed(_videoService->GetVideoProcessorCaps(DXVA2_VideoProcProgressiveDevice, &_videoDesc,
			VIDEO_RENDER_TARGET_FORMAT, &_videoProcessorCaps));
	}
	ThrowIfNot((_videoProcessorCaps.VideoProcessorOperations & VIDEO_REQUIED_OP) == VIDEO_REQUIED_OP,
		L"The DXVA2 device doesn't support the VP operations.");
}
