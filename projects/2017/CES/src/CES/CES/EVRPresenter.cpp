#include "stdafx.h"
#include "EVRPresenter.h"
#include <dshow.h>

using namespace WRL;
using namespace CES;

#define LOCK_STATE() auto locker = _stateLock.Lock()

EVRPresenter::EVRPresenter()
	:_state(EVRPresenterState::NotInitialized), _normalizedVideoSrc({ 0, 0, 1, 1 })
{
}

HRESULT EVRPresenter::GetDeviceID(IID * pDeviceID)
{
	*pDeviceID = IID_IDirect3DDevice9;
	return S_OK;
}

HRESULT EVRPresenter::OnClockStart(MFTIME hnsSystemTime, LONGLONG llClockStartOffset)
{
	try
	{
		LOCK_STATE();
		_state = EVRPresenterState::Started;

		if (IsActive())
		{
			if (llClockStartOffset != PRESENTATION_CURRENT_POSITION)
				Flush();
		}
		else
			StartFrameStep();
		ProcessOutput();
		return S_OK;
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::OnClockStop(MFTIME hnsSystemTime)
{
	return E_NOTIMPL;
}

HRESULT EVRPresenter::OnClockPause(MFTIME hnsSystemTime)
{
	return E_NOTIMPL;
}

HRESULT EVRPresenter::OnClockRestart(MFTIME hnsSystemTime)
{
	return E_NOTIMPL;
}

HRESULT EVRPresenter::OnClockSetRate(MFTIME hnsSystemTime, float flRate)
{
	return E_NOTIMPL;
}

HRESULT EVRPresenter::ProcessMessage(MFVP_MESSAGE_TYPE eMessage, ULONG_PTR ulParam)
{
	try
	{
		LOCK_STATE();

		CheckShutdown();
		switch (eMessage)
		{
		case MFVP_MESSAGE_FLUSH:
			break;
		case MFVP_MESSAGE_INVALIDATEMEDIATYPE:
			RenegotiateMediaType();
			break;
		case MFVP_MESSAGE_PROCESSINPUTNOTIFY:
			break;
		case MFVP_MESSAGE_BEGINSTREAMING:
			break;
		case MFVP_MESSAGE_ENDSTREAMING:
			break;
		case MFVP_MESSAGE_ENDOFSTREAM:
			break;
		case MFVP_MESSAGE_STEP:
			break;
		case MFVP_MESSAGE_CANCELSTEP:
			break;
		default:
			break;
		}
		return S_OK;
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::GetCurrentMediaType(IMFVideoMediaType ** ppMediaType)
{
	try
	{
		LOCK_STATE();

		CheckShutdown();
		if (!_mediaType) return MF_E_NOT_INITIALIZED;
		return _mediaType.CopyTo(ppMediaType);
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::GetSlowestRate(MFRATE_DIRECTION eDirection, BOOL fThin, float * pflRate)
{
	try
	{
		LOCK_STATE();

		CheckShutdown();
		*pflRate = 0;
		return S_OK;
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::GetFastestRate(MFRATE_DIRECTION eDirection, BOOL fThin, float * pflRate)
{
	try
	{
		LOCK_STATE();

		CheckShutdown();

		auto maxRate = GetMaxRate(fThin);
		if (eDirection == MFRATE_REVERSE)
			maxRate = -maxRate;
		*pflRate = maxRate;
		return S_OK;
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::IsRateSupported(BOOL fThin, float flRate, float * pflNearestSupportedRate)
{
	try
	{
		LOCK_STATE();
		CheckShutdown();

		auto hr = S_OK;
		float nearestRate = flRate;
		auto maxRate = GetMaxRate(fThin);
		if (std::abs(flRate) > maxRate)
		{
			if(flRate < 0)
				nearestRate = -maxRate;
			else
				nearestRate = maxRate;
			hr = MF_E_UNSUPPORTED_RATE;
		}
		if (pflNearestSupportedRate)
			*pflNearestSupportedRate = nearestRate;
		return hr;
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::GetService(REFGUID guidService, REFIID riid, LPVOID * ppvObject)
{
	if (guidService == MR_VIDEO_RENDER_SERVICE)
	{
		auto hr = _d3d9Renderer.GetService(guidService, riid, ppvObject);
		if (FAILED(hr))
			hr = QueryInterface(riid, ppvObject);
		return hr;
	}
	else if (guidService != MR_VIDEO_ACCELERATION_SERVICE)
		return _d3d9Renderer.GetService(guidService, riid, ppvObject);
	return MF_E_UNSUPPORTED_SERVICE;
}

HRESULT EVRPresenter::InitServicePointers(IMFTopologyServiceLookup * pLookup)
{
	try
	{
		LOCK_STATE();
		if (IsActive()) return MF_E_INVALIDREQUEST;

		DWORD objectCount = 1;
		// optional
		pLookup->LookupService(MF_SERVICE_LOOKUP_GLOBAL, 0, MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&_clock), &objectCount);

		objectCount = 1;
		ThrowIfFailed(pLookup->LookupService(MF_SERVICE_LOOKUP_GLOBAL, 0, MR_VIDEO_MIXER_SERVICE, IID_PPV_ARGS(&_mixer), &objectCount));
		ConfigureMixer();

		objectCount = 1;
		ThrowIfFailed(pLookup->LookupService(MF_SERVICE_LOOKUP_GLOBAL, 0, MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&_mediaEventSink), &objectCount));

		_state = EVRPresenterState::Stopped;
		return S_OK;
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::ReleaseServicePointers(void)
{
	try
	{
		LOCK_STATE();

		_state = EVRPresenterState::Shutdown;
		Flush();
		SetMediaType(nullptr);
		return S_OK;
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::GetNativeVideoSize(SIZE * pszVideo, SIZE * pszARVideo)
{
	return E_NOTIMPL;
}

HRESULT EVRPresenter::GetIdealVideoSize(SIZE * pszMin, SIZE * pszMax)
{
	return E_NOTIMPL;
}

namespace
{
	void SetMixerSourceRect(IMFTransform* mixer, const MFVideoNormalizedRect& srcRect)
	{
		ComPtr<IMFAttributes> attributes;
		ThrowIfFailed(mixer->GetAttributes(&attributes));
		ThrowIfFailed(attributes->SetBlob(VIDEO_ZOOM_RECT, reinterpret_cast<const UINT8*>(&srcRect), sizeof(srcRect)));
	}

	bool operator==(const RECT& left, const RECT& right) noexcept
	{
		return left.left == right.left && left.top == right.top &&
			left.right == right.right && left.bottom == right.bottom;
	}

	bool operator!=(const RECT& left, const RECT& right) noexcept
	{
		return !(left == right);
	}
}

HRESULT EVRPresenter::SetVideoPosition(const MFVideoNormalizedRect * pnrcSource, const LPRECT prcDest)
{
	ARGUMENT_NOTNULL_HR(pnrcSource || prcDest);
	if (pnrcSource)
	{
		// The source rectangle cannot be flipped.
		if ((pnrcSource->left > pnrcSource->right) ||
			(pnrcSource->top > pnrcSource->bottom))
			return E_INVALIDARG;

		// The source rectangle has range (0..1)
		if ((pnrcSource->left < 0) || (pnrcSource->right > 1) ||
			(pnrcSource->top < 0) || (pnrcSource->bottom > 1))
			return E_INVALIDARG;
	}
	if (prcDest)
	{
		// The destination rectangle cannot be flipped.
		if ((prcDest->left > prcDest->right) ||
			(prcDest->top > prcDest->bottom))
			return E_INVALIDARG;
	}

	try
	{
		LOCK_STATE();
		// Update the source rectangle. Source clipping is performed by the mixer.
		if (pnrcSource)
		{
			_normalizedVideoSrc = *pnrcSource;

			if (_mixer)
				SetMixerSourceRect(_mixer.Get(), _normalizedVideoSrc);
		}
		// Update the destination rectangle.
		if (prcDest)
		{
			auto oldDestRect = _d3d9Renderer.GetDestinationRect();
			if (oldDestRect != *prcDest)
			{
				_d3d9Renderer.SetDestinationRect(*prcDest);
				if (_mixer)
				{
					HRESULT hr = S_OK;
					try
					{
						RenegotiateMediaType();
					}
					CATCH_ALL_WITHHR(hr);
					if (FAILED(hr) && hr != MF_E_TRANSFORM_TYPE_NOT_SET)
						ThrowIfFailed(hr);
					_needRepaint = true;
					try
					{
						ProcessOutput();
					}
					CATCH_ALL_WITHHR(hr);
				}
			}
		}
		return S_OK;
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::GetVideoPosition(MFVideoNormalizedRect * pnrcSource, LPRECT prcDest)
{
	ARGUMENT_NOTNULL_HR(pnrcSource || prcDest);
	try
	{
		LOCK_STATE();

		*pnrcSource = _normalizedVideoSrc;
		*prcDest = _d3d9Renderer.GetDestinationRect();
		return S_OK;
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::SetAspectRatioMode(DWORD dwAspectRatioMode)
{
	return E_NOTIMPL;
}

HRESULT EVRPresenter::GetAspectRatioMode(DWORD * pdwAspectRatioMode)
{
	return E_NOTIMPL;
}

HRESULT EVRPresenter::SetVideoWindow(HWND hwndVideo)
{
	try
	{
		LOCK_STATE();
		auto oldhWnd = _d3d9Renderer.GetVideoWindow();
		if (oldhWnd != hwndVideo)
		{
			_d3d9Renderer.SetVideoWindow(hwndVideo);
			NotifyEvent(EC_DISPLAY_CHANGED, 0, 0);
		}
		return S_OK;
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::GetVideoWindow(HWND * phwndVideo)
{
	try
	{
		LOCK_STATE();
		*phwndVideo = _d3d9Renderer.GetVideoWindow();
		return S_OK;
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::RepaintVideo(void)
{
	return E_NOTIMPL;
}

HRESULT EVRPresenter::GetCurrentImage(BITMAPINFOHEADER * pBih, BYTE ** pDib, DWORD * pcbDib, LONGLONG * pTimeStamp)
{
	return E_NOTIMPL;
}

HRESULT EVRPresenter::SetBorderColor(COLORREF Clr)
{
	return E_NOTIMPL;
}

HRESULT EVRPresenter::GetBorderColor(COLORREF * pClr)
{
	return E_NOTIMPL;
}

HRESULT EVRPresenter::SetRenderingPrefs(DWORD dwRenderFlags)
{
	return E_NOTIMPL;
}

HRESULT EVRPresenter::GetRenderingPrefs(DWORD * pdwRenderFlags)
{
	return E_NOTIMPL;
}

HRESULT EVRPresenter::SetFullscreen(BOOL fFullscreen)
{
	try
	{
		_d3d9Renderer.SetFullscreen(fFullscreen);
		return S_OK;
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::GetFullscreen(BOOL * pfFullscreen)
{
	*pfFullscreen = _d3d9Renderer.GetFullscreen();
	return S_OK;
}

bool CES::EVRPresenter::IsActive() const noexcept
{
	auto state = _state.load(std::memory_order_acquire);
	return state == EVRPresenterState::Started || state == EVRPresenterState::Paused;
}

void CES::EVRPresenter::CheckShutdown() const
{
	if (_state.load(std::memory_order_acquire) == EVRPresenterState::Shutdown)
		ThrowIfFailed(MF_E_SHUTDOWN);
}

void CES::EVRPresenter::ConfigureMixer()
{
	ComPtr<IMFVideoDeviceID> deviceId;
	ThrowIfFailed(_mixer.As(&deviceId));

	IID mixerId;
	ThrowIfFailed(deviceId->GetDeviceID(&mixerId));
	if (mixerId != IID_IDirect3DDevice9)
		ThrowIfFailed(MF_E_INVALIDREQUEST);
	SetMixerSourceRect(_mixer.Get(), _normalizedVideoSrc);
}

void CES::EVRPresenter::Flush()
{
}

void CES::EVRPresenter::SetMediaType(IMFMediaType * mediaType)
{
	if (!mediaType)
	{
		_mediaType.Reset();
	}
	else
	{
		_mediaType = mediaType;
	}
}

void CES::EVRPresenter::RenegotiateMediaType()
{
	if (!_mixer) ThrowIfFailed(MF_E_INVALIDREQUEST);

	HRESULT hr = S_OK;
	DWORD typeIndex = 0;
	while (hr != MF_E_NO_MORE_TYPES)
	{
		ComPtr<IMFMediaType> mixerMT;
		ThrowIfFailed(_mixer->GetOutputAvailableType(0, typeIndex++, &mixerMT));
		if (IsMediaTypeSupported(mixerMT.Get()))
		{
			try
			{
				auto optimalMT = CreateOptimalVideoType(mixerMT.Get());
				ThrowIfFailed(_mixer->SetOutputType(0, optimalMT.Get(), MFT_SET_TYPE_TEST_ONLY));
				SetMediaType(optimalMT.Get());
				ThrowIfFailed(_mixer->SetOutputType(0, optimalMT.Get(), 0));
				break;
			}
			CATCH_ALL_WITHHR(hr);
		}
	}
	if (FAILED(hr))
		SetMediaType(nullptr);
	ThrowIfFailed(hr);
}

bool CES::EVRPresenter::IsMediaTypeSupported(IMFMediaType * mediaType)
{
	return true;
}

WRL::ComPtr<IMFMediaType> CES::EVRPresenter::CreateOptimalVideoType(IMFMediaType * proposedMT)
{
	return proposedMT;
}

void CES::EVRPresenter::NotifyEvent(long EventCode, LONG_PTR Param1, LONG_PTR Param2)
{
	if (auto eventSink = _mediaEventSink)
		ThrowIfFailed(eventSink->Notify(EventCode, Param1, Param2));
}

void CES::EVRPresenter::ProcessOutput()
{
	if ((_state != EVRPresenterState::Started) &&  // Not running.
		!_needRepaint &&             // Not a repaint request.
		_preRolled               // At least one sample has been presented.
		)
		return;
	if (!_mixer)
		ThrowIfFailed(MF_E_INVALIDREQUEST);
}

float EVRPresenter::GetMaxRate(bool thin)
{
	float   fMaxRate = FLT_MAX;
	MFRatio fps = { 0, 0 };
	UINT    MonitorRateHz = 0;

	if (!thin && _mediaType)
	{
		ThrowIfFailed(_mediaType->GetUINT64(MF_MT_FRAME_RATE, reinterpret_cast<UINT64*>(&fps)));
		MonitorRateHz = _d3d9Renderer.GetRefreshRate();

		if (fps.Denominator && fps.Numerator && MonitorRateHz)
		{
			// Max Rate = Refresh Rate / Frame Rate
			fMaxRate = (float)MulDiv(MonitorRateHz, fps.Denominator, fps.Numerator);
		}
	}

	return fMaxRate;
}

void CES::EVRPresenter::StartFrameStep()
{
	if (_frameStep.State == FrameStepState::WaitingStart)
	{
		_frameStep.State = FrameStepState::Pending;
		while (!_frameStep.Samples.empty() && _frameStep.State == FrameStepState::Pending)
		{
			auto sample = std::move(_frameStep.Samples.front());
			_frameStep.Samples.pop();
			DeliverFrameStepSample(sample.Get());
		}
	}
	else if(_frameStep.State == FrameStepState::None)
	{
		while (!_frameStep.Samples.empty())
		{
			auto sample = std::move(_frameStep.Samples.front());
			_frameStep.Samples.pop();
			DeliverSample(sample.Get(), false);
		}
	}
}

void CES::EVRPresenter::DeliverFrameStepSample(IMFSample * sample)
{
}

void CES::EVRPresenter::DeliverSample(IMFSample * sample, bool flag)
{
}

EVRPresenterActivate::EVRPresenterActivate()
{
	ThrowIfFailed(MFCreateAttributes(&_attributes, 1));
}

HRESULT EVRPresenterActivate::GetItem(REFGUID guidKey, PROPVARIANT * pValue)
{
	return _attributes->GetItem(guidKey, pValue);
}

HRESULT EVRPresenterActivate::GetItemType(REFGUID guidKey, MF_ATTRIBUTE_TYPE * pType)
{
	return _attributes->GetItemType(guidKey, pType);
}

HRESULT EVRPresenterActivate::CompareItem(REFGUID guidKey, REFPROPVARIANT Value, BOOL * pbResult)
{
	return _attributes->CompareItem(guidKey, Value, pbResult);
}

HRESULT EVRPresenterActivate::Compare(IMFAttributes * pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL * pbResult)
{
	return _attributes->Compare(pTheirs, MatchType, pbResult);
}

HRESULT EVRPresenterActivate::GetUINT32(REFGUID guidKey, UINT32 * punValue)
{
	return _attributes->GetUINT32(guidKey, punValue);
}

HRESULT EVRPresenterActivate::GetUINT64(REFGUID guidKey, UINT64 * punValue)
{
	return _attributes->GetUINT64(guidKey, punValue);
}

HRESULT EVRPresenterActivate::GetDouble(REFGUID guidKey, double * pfValue)
{
	return _attributes->GetDouble(guidKey, pfValue);
}

HRESULT EVRPresenterActivate::GetGUID(REFGUID guidKey, GUID * pguidValue)
{
	return _attributes->GetGUID(guidKey, pguidValue);
}

HRESULT EVRPresenterActivate::GetStringLength(REFGUID guidKey, UINT32 * pcchLength)
{
	return _attributes->GetStringLength(guidKey, pcchLength);
}

HRESULT EVRPresenterActivate::GetString(REFGUID guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32 * pcchLength)
{
	return _attributes->GetString(guidKey, pwszValue, cchBufSize, pcchLength);
}

HRESULT EVRPresenterActivate::GetAllocatedString(REFGUID guidKey, LPWSTR * ppwszValue, UINT32 * pcchLength)
{
	return _attributes->GetAllocatedString(guidKey, ppwszValue, pcchLength);
}

HRESULT EVRPresenterActivate::GetBlobSize(REFGUID guidKey, UINT32 * pcbBlobSize)
{
	return _attributes->GetBlobSize(guidKey, pcbBlobSize);
}

HRESULT EVRPresenterActivate::GetBlob(REFGUID guidKey, UINT8 * pBuf, UINT32 cbBufSize, UINT32 * pcbBlobSize)
{
	return _attributes->GetBlob(guidKey, pBuf, cbBufSize, pcbBlobSize);
}

HRESULT EVRPresenterActivate::GetAllocatedBlob(REFGUID guidKey, UINT8 ** ppBuf, UINT32 * pcbSize)
{
	return _attributes->GetAllocatedBlob(guidKey, ppBuf, pcbSize);
}

HRESULT EVRPresenterActivate::GetUnknown(REFGUID guidKey, REFIID riid, LPVOID * ppv)
{
	return _attributes->GetUnknown(guidKey, riid, ppv);
}

HRESULT EVRPresenterActivate::SetItem(REFGUID guidKey, REFPROPVARIANT Value)
{
	return _attributes->SetItem(guidKey, Value);
}

HRESULT EVRPresenterActivate::DeleteItem(REFGUID guidKey)
{
	return _attributes->DeleteItem(guidKey);
}

HRESULT EVRPresenterActivate::DeleteAllItems(void)
{
	return _attributes->DeleteAllItems();
}

HRESULT EVRPresenterActivate::SetUINT32(REFGUID guidKey, UINT32 unValue)
{
	return _attributes->SetUINT32(guidKey, unValue);
}

HRESULT EVRPresenterActivate::SetUINT64(REFGUID guidKey, UINT64 unValue)
{
	return _attributes->SetUINT64(guidKey, unValue);
}

HRESULT EVRPresenterActivate::SetDouble(REFGUID guidKey, double fValue)
{
	return _attributes->SetDouble(guidKey, fValue);
}

HRESULT EVRPresenterActivate::SetGUID(REFGUID guidKey, REFGUID guidValue)
{
	return _attributes->SetGUID(guidKey, guidValue);
}

HRESULT EVRPresenterActivate::SetString(REFGUID guidKey, LPCWSTR wszValue)
{
	return _attributes->SetString(guidKey, wszValue);
}

HRESULT EVRPresenterActivate::SetBlob(REFGUID guidKey, const UINT8 * pBuf, UINT32 cbBufSize)
{
	return _attributes->SetBlob(guidKey, pBuf, cbBufSize);
}

HRESULT EVRPresenterActivate::SetUnknown(REFGUID guidKey, IUnknown * pUnknown)
{
	return _attributes->SetUnknown(guidKey, pUnknown);
}

HRESULT EVRPresenterActivate::LockStore(void)
{
	return _attributes->LockStore();
}

HRESULT EVRPresenterActivate::UnlockStore(void)
{
	return _attributes->UnlockStore();
}

HRESULT EVRPresenterActivate::GetCount(UINT32 * pcItems)
{
	return _attributes->GetCount(pcItems);
}

HRESULT EVRPresenterActivate::GetItemByIndex(UINT32 unIndex, GUID * pguidKey, PROPVARIANT * pValue)
{
	return _attributes->GetItemByIndex(unIndex, pguidKey, pValue);
}

HRESULT EVRPresenterActivate::CopyAllItems(IMFAttributes * pDest)
{
	return _attributes->CopyAllItems(pDest);
}

HRESULT EVRPresenterActivate::ActivateObject(REFIID riid, void ** ppv)
{
	try
	{
		if (!_object)
			_object = Make<EVRPresenter>();
		return _object.CopyTo(riid, ppv);
	}
	TCATCH_ALL();
}

HRESULT EVRPresenterActivate::ShutdownObject(void)
{
	_object.Reset();
	return S_OK;
}

HRESULT EVRPresenterActivate::DetachObject(void)
{
	_object.Reset();
	return S_OK;
}
