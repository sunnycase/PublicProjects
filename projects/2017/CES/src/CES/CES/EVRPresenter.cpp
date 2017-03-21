#include "stdafx.h"
#include "EVRPresenter.h"
#include <dshow.h>

using namespace WRL;
using namespace CES;

#define LOCK_STATE() auto locker = _stateLock.Lock()

EVRPresenter::EVRPresenter()
	:_state(EVRPresenterState::NotInitialized), _normalizedVideoSrc({ 0, 0, 1, 1 }),
	_scheduler(Make<Scheduler>()), _d3d9Renderer(Make<D3D9VideoRenderer>()),
	_freeSampleCallback(Make<NS_MEDIA::MFAsyncCallbackWithWeakRef<EVRPresenter>>(AsWeak(), &EVRPresenter::OnSampleFree))
{
	_scheduler->SetSamplePresenter(_d3d9Renderer.Get());
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
	try
	{
		LOCK_STATE();

		CheckShutdown();
		// If the rate is changing from zero (scrubbing) to non-zero, cancel the 
		// frame-step operation.
		if (_clockRate == 0 && flRate != 0)
		{
			CancelFrameStep();
			_frameStep.Samples.swap(decltype(_frameStep.Samples)());
		}
		_clockRate = flRate;
		_scheduler->SetClockRate(flRate);
		return S_OK;
	}
	TCATCH_ALL();
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
			Flush();
			break;
		case MFVP_MESSAGE_INVALIDATEMEDIATYPE:
			RenegotiateMediaType();
			break;
		case MFVP_MESSAGE_PROCESSINPUTNOTIFY:
			ProcessInputNotify();
			break;
		case MFVP_MESSAGE_BEGINSTREAMING:
			BeginStreaming();
			break;
		case MFVP_MESSAGE_ENDSTREAMING:
			EndStreaming();
			break;
		case MFVP_MESSAGE_ENDOFSTREAM:
			break;
		case MFVP_MESSAGE_STEP:
			PrepareFrameStep(LODWORD(ulParam));
			break;
		case MFVP_MESSAGE_CANCELSTEP:
			CancelFrameStep();
			break;
		default:
			return E_INVALIDARG;
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
			if (flRate < 0)
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
		auto hr = _d3d9Renderer->GetService(guidService, riid, ppvObject);
		if (FAILED(hr))
			hr = QueryInterface(riid, ppvObject);
		return hr;
	}
	else if (guidService != MR_VIDEO_ACCELERATION_SERVICE)
		return _d3d9Renderer->GetService(guidService, riid, ppvObject);
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
			auto oldDestRect = _d3d9Renderer->GetDestinationRect();
			if (oldDestRect != *prcDest)
			{
				_d3d9Renderer->SetDestinationRect(*prcDest);
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
		*prcDest = _d3d9Renderer->GetDestinationRect();
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
		auto oldhWnd = _d3d9Renderer->GetVideoWindow();
		if (oldhWnd != hwndVideo)
		{
			_d3d9Renderer->SetVideoWindow(hwndVideo);
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
		*phwndVideo = _d3d9Renderer->GetVideoWindow();
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
		_d3d9Renderer->SetFullscreen(fFullscreen);
		return S_OK;
	}
	TCATCH_ALL();
}

HRESULT EVRPresenter::GetFullscreen(BOOL * pfFullscreen)
{
	*pfFullscreen = _d3d9Renderer->GetFullscreen();
	return S_OK;
}

bool EVRPresenter::IsActive() const noexcept
{
	auto state = _state.load(std::memory_order_acquire);
	return state == EVRPresenterState::Started || state == EVRPresenterState::Paused;
}

void EVRPresenter::CheckShutdown() const
{
	if (_state.load(std::memory_order_acquire) == EVRPresenterState::Shutdown)
		ThrowIfFailed(MF_E_SHUTDOWN);
}

void EVRPresenter::ConfigureMixer()
{
	ComPtr<IMFVideoDeviceID> deviceId;
	ThrowIfFailed(_mixer.As(&deviceId));

	IID mixerId;
	ThrowIfFailed(deviceId->GetDeviceID(&mixerId));
	if (mixerId != IID_IDirect3DDevice9)
		ThrowIfFailed(MF_E_INVALIDREQUEST);
	SetMixerSourceRect(_mixer.Get(), _normalizedVideoSrc);
}

void EVRPresenter::Flush()
{
}

namespace
{
	bool AreMediaTypesEqual(IMFMediaType *pType1, IMFMediaType *pType2)
	{
		if ((pType1 == NULL) && (pType2 == NULL))
			return true; // Both are NULL.
		else if ((pType1 == NULL) || (pType2 == NULL))
			return false; // One is NULL.

		DWORD dwFlags = 0;
		HRESULT hr = pType1->IsEqual(pType2, &dwFlags);

		return (hr == S_OK);
	}
}

void EVRPresenter::SetMediaType(IMFMediaType * mediaType)
{
	if (!mediaType)
	{
		_mediaType.Reset();
		return;
	}

	CheckShutdown();
	if (AreMediaTypesEqual(mediaType, _mediaType.Get()))
		return;

	_d3d9Renderer->CreateVideoSamples(mediaType, _samplePool);
	MFRatio fps;
	if (SUCCEEDED(mediaType->GetUINT64(MF_MT_FRAME_RATE, reinterpret_cast<UINT64*>(&fps))) &&
		fps.Denominator && fps.Numerator)
	{

	}
	else
	{

	}
	_mediaType = mediaType;
}

void EVRPresenter::RenegotiateMediaType()
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

bool EVRPresenter::IsMediaTypeSupported(IMFMediaType * mediaType)
{
	return true;
}

WRL::ComPtr<IMFMediaType> EVRPresenter::CreateOptimalVideoType(IMFMediaType * proposedMT)
{
	return proposedMT;
}

void EVRPresenter::NotifyEvent(long EventCode, LONG_PTR Param1, LONG_PTR Param2)
{
	if (auto eventSink = _mediaEventSink)
		ThrowIfFailed(eventSink->Notify(EventCode, Param1, Param2));
}

namespace
{
	void SetDesiredSampleTime(IMFSample* sample, MFTIME hnsSampleTime, MFTIME hnsDuration)
	{
		ComPtr<IMFDesiredSample> desiredSample;
		ThrowIfFailed(sample->QueryInterface(IID_PPV_ARGS(&desiredSample)));
		desiredSample->SetDesiredSampleTimeAndDuration(hnsSampleTime, hnsDuration);
	}
}

void EVRPresenter::ProcessOutput()
{
	if ((_state != EVRPresenterState::Started) &&  // Not running.
		!_needRepaint &&             // Not a repaint request.
		_preRolled               // At least one sample has been presented.
		)
		return;
	if (!_mixer)
		ThrowIfFailed(MF_E_INVALIDREQUEST);

	ComPtr<IMFSample> sample;
	if (!_samplePool.TryGetSample(&sample))
		return;
	MFTIME mixerStartTime = 0, mixerEndTime = 0, systemTime = 0;
	auto repaint = _needRepaint;
	if (_needRepaint)
	{
		SetDesiredSampleTime(sample.Get(), _scheduler->GetLastSampleTime(), _scheduler->GetFrameDuration());
		_needRepaint = false;
	}
	else
	{
		_samplePool.ClearDesiredSampleTime(sample.Get());
		if (auto clock = _clock)
			clock->GetCorrelatedTime(0, &mixerStartTime, &systemTime);
	}

	MFT_OUTPUT_DATA_BUFFER dataBuffer{};
	dataBuffer.dwStreamID = 0;
	dataBuffer.pSample = sample.Get();
	dataBuffer.dwStatus = 0;
	auto fin = make_finalizer([&] {if (dataBuffer.pEvents)dataBuffer.pEvents->Release(); });

	DWORD status;
	auto hr = _mixer->ProcessOutput(0, 1, &dataBuffer, &status);
	if (FAILED(hr))
	{
		_samplePool.ReturnSample(sample.Get());
		switch (hr)
		{
		case MF_E_TRANSFORM_TYPE_NOT_SET:
			RenegotiateMediaType();
			break;
		case MF_E_TRANSFORM_STREAM_CHANGE:
			SetMediaType(nullptr);
			break;
		case MF_E_TRANSFORM_NEED_MORE_INPUT:
			_sampleNotify = false;
			break;
		default:
			break;
		}
	}
	else
	{
		auto clock = _clock;
		if (clock && !repaint)
		{
			clock->GetCorrelatedTime(0, &mixerEndTime, &systemTime);
			auto latencyTime = mixerEndTime - mixerStartTime;
			NotifyEvent(EC_PROCESSING_LATENCY, reinterpret_cast<LONG_PTR>(&latencyTime), 0);
		}
		TrackSample(sample.Get());
		// Schedule the sample.
		if (_frameStep.State == FrameStepState::None || repaint)
			DeliverSample(sample.Get(), repaint);
		else
			DeliverFrameStepSample(sample.Get());
		_preRolled = true;
	}
}

void EVRPresenter::ProcessOutputLoop()
{
	auto hr = S_OK;
	try
	{
		while (SUCCEEDED(hr))
		{
			if (!_sampleNotify)
			{
				hr = MF_E_TRANSFORM_NEED_MORE_INPUT;
				break;
			}
			ProcessOutput();
		}
	}
	CATCH_ALL_WITHHR(hr);
	if (hr == MF_E_TRANSFORM_NEED_MORE_INPUT)
		CheckEndOfStream();
}

float EVRPresenter::GetMaxRate(bool thin)
{
	float   fMaxRate = FLT_MAX;
	MFRatio fps = { 0, 0 };
	UINT    MonitorRateHz = 0;

	if (!thin && _mediaType)
	{
		ThrowIfFailed(_mediaType->GetUINT64(MF_MT_FRAME_RATE, reinterpret_cast<UINT64*>(&fps)));
		MonitorRateHz = _d3d9Renderer->GetRefreshRate();
		
		if (fps.Denominator && fps.Numerator && MonitorRateHz)
		{
			// Max Rate = Refresh Rate / Frame Rate
			fMaxRate = (float)MulDiv(MonitorRateHz, fps.Denominator, fps.Numerator);
		}
	}

	return fMaxRate;
}

void EVRPresenter::PrepareFrameStep(DWORD steps)
{
	_frameStep.Steps += steps;
	_frameStep.State = FrameStepState::WaitingStart;
	// If the clock is are already running, we can start frame-stepping now.
	// Otherwise, we will start when the clock starts.
	if (_state == EVRPresenterState::Started)
		StartFrameStep();
}

void EVRPresenter::StartFrameStep()
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
	else if (_frameStep.State == FrameStepState::None)
	{
		while (!_frameStep.Samples.empty())
		{
			auto sample = std::move(_frameStep.Samples.front());
			_frameStep.Samples.pop();
			DeliverSample(sample.Get(), false);
		}
	}
}

void EVRPresenter::CancelFrameStep()
{
	auto oldState = _frameStep.State;
	_frameStep.State = FrameStepState::None;
	_frameStep.SampleNoRef = nullptr;
	_frameStep.Steps = 0;
	if (oldState > FrameStepState::None && oldState < FrameStepState::Complete)
		NotifyEvent(EC_STEP_COMPLETE, TRUE, 0);
}

void EVRPresenter::CompleteFrameStep(IMFSample * sample)
{
	MFTIME hnsSampleTime = 0;

	_frameStep.SampleNoRef = nullptr;
	_frameStep.State = FrameStepState::Complete;
	NotifyEvent(EC_STEP_COMPLETE, FALSE, 0);
	if (IsScrubbing())
	{
		if (FAILED(sample->GetSampleTime(&hnsSampleTime)))
		{
			if (_clock)
			{
				MFTIME hnsSystemTime;
				_clock->GetCorrelatedTime(0, &hnsSampleTime, &hnsSystemTime);
			}
		}
		NotifyEvent(EC_SCRUB_TIME, LOWORD(hnsSampleTime), HIWORD(hnsSampleTime));
	}
}

namespace
{
	bool IsSampleTimePassed(IMFSample* sample, IMFClock* clock)
	{
		MFTIME hnsTimeNow = 0;
		MFTIME hnsSystemTime = 0;
		MFTIME hnsSampleStart = 0;
		MFTIME hnsSampleDuration = 0;

		if (SUCCEEDED(clock->GetCorrelatedTime(0, &hnsTimeNow, &hnsSystemTime)))
			if (SUCCEEDED(sample->GetSampleTime(&hnsSampleStart)))
				if (SUCCEEDED(sample->GetSampleDuration(&hnsSampleDuration)))
					return hnsSampleStart + hnsSampleDuration < hnsTimeNow;
		return false;
	}
}

void EVRPresenter::DeliverFrameStepSample(IMFSample * sample)
{
	if (IsScrubbing() && _clock && IsSampleTimePassed(sample, _clock.Get()))
		; // Discard this sample.
	else if(_frameStep.State >= FrameStepState::Scheduled)
	{
		// A frame was already submitted. Put this sample on the frame-step queue, 
		// in case we are asked to step to the next frame. If frame-stepping is
		// cancelled, this sample will be processed normally.
		_frameStep.Samples.emplace(sample);
	}
	else
	{
		if (_frameStep.Steps)
			_frameStep.Steps--; 
		if (_frameStep.Steps)
			; // This is not the last step. Discard this sample.
		else if (_frameStep.State == FrameStepState::WaitingStart)
		{
			// This is the right frame, but the clock hasn't started yet. Put the
			// sample on the frame-step queue. When the clock starts, the sample
			// will be processed.
			_frameStep.Samples.emplace(sample);
		}
		else
		{
			DeliverSample(sample, false);
			_frameStep.SampleNoRef = sample;
			_frameStep.State = FrameStepState::Scheduled;
		}
	}
}

void EVRPresenter::DeliverSample(IMFSample * sample, bool repaint)
{
	auto state = D3D9VideoRenderer::DeviceState::Ok;
	auto presentNow = _state != EVRPresenterState::Started || IsScrubbing() || repaint;
	auto deviceState = _d3d9Renderer->GetDeviceState();
	auto hr = deviceState.first;
	if (SUCCEEDED(hr))
		_scheduler->ScheduleSample(sample, presentNow);
	if (FAILED(hr) && hr != MF_E_NOT_INITIALIZED)
		NotifyEvent(EC_ERRORABORT, hr, 0);
	else if(deviceState.second == D3D9VideoRenderer::DeviceState::Reset)
		NotifyEvent(EC_DISPLAY_CHANGED, S_OK, 0);
}

void EVRPresenter::TrackSample(IMFSample * sample)
{
	ComPtr<IMFTrackedSample> trackedSample;
	ThrowIfFailed(sample->QueryInterface(IID_PPV_ARGS(&trackedSample)));
	ThrowIfFailed(trackedSample->SetAllocator(_freeSampleCallback.Get(), nullptr));
}

void EVRPresenter::OnSampleFree(IMFAsyncResult * pAsyncResult)
{
	auto hr = S_OK;
	try
	{
		ComPtr<IUnknown> sampleUnk;
		ThrowIfFailed(pAsyncResult->GetObject(&sampleUnk));
		ComPtr<IMFSample> sample;
		ThrowIfFailed(sampleUnk.As(&sample));

		LOCK_STATE();
		if (_frameStep.State == FrameStepState::Scheduled)
		{
			if (_frameStep.SampleNoRef == sample.Get())
				CompleteFrameStep(sample.Get());
			_samplePool.ReturnSample(sample.Get());
			ProcessOutputLoop();
		}
	}
	CATCH_ALL_WITHHR(hr);
	if (FAILED(hr))
		NotifyEvent(EC_ERRORABORT, hr, 0);
	ThrowIfFailed(hr);
}

void EVRPresenter::ProcessInputNotify()
{
	_sampleNotify = true;
	if (!_mediaType)
		ThrowIfFailed(MF_E_TRANSFORM_TYPE_NOT_SET);
	else
		ProcessOutputLoop();
}

void EVRPresenter::BeginStreaming()
{
	_scheduler->StartScheduler(_clock.Get());
}

void EVRPresenter::EndStreaming()
{
	_scheduler->StopScheduler();
}

void EVRPresenter::CheckEndOfStream()
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
