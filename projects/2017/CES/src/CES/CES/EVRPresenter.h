#pragma once
#include <Tomato.Core/Tomato.Core.h>
#include <mfidl.h>
#include <evr.h>
#include <d3d9.h>
#include <evr9.h>
#include <atomic>
#include <queue>
#include "D3D9VideoRenderer.h"

namespace CES
{
	enum class EVRPresenterState
	{
		NotInitialized,
		Started,
		Paused,
		Stopped,
		Shutdown
	};

	class EVRPresenter : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>,
		IMFVideoDeviceID, IMFVideoPresenter, IMFRateSupport, IMFGetService, IMFTopologyServiceLookupClient, IMFVideoDisplayControl>
	{
	public:
		EVRPresenter();

		// 通过 RuntimeClass 继承
		STDMETHODIMP GetDeviceID(IID * pDeviceID) override;
		STDMETHODIMP OnClockStart(MFTIME hnsSystemTime, LONGLONG llClockStartOffset) override;
		STDMETHODIMP OnClockStop(MFTIME hnsSystemTime) override;
		STDMETHODIMP OnClockPause(MFTIME hnsSystemTime) override;
		STDMETHODIMP OnClockRestart(MFTIME hnsSystemTime) override;
		STDMETHODIMP OnClockSetRate(MFTIME hnsSystemTime, float flRate) override;
		STDMETHODIMP ProcessMessage(MFVP_MESSAGE_TYPE eMessage, ULONG_PTR ulParam) override;
		STDMETHODIMP GetCurrentMediaType(IMFVideoMediaType ** ppMediaType) override;
		STDMETHODIMP GetSlowestRate(MFRATE_DIRECTION eDirection, BOOL fThin, float * pflRate) override;
		STDMETHODIMP GetFastestRate(MFRATE_DIRECTION eDirection, BOOL fThin, float * pflRate) override;
		STDMETHODIMP IsRateSupported(BOOL fThin, float flRate, float * pflNearestSupportedRate) override;
		STDMETHODIMP GetService(REFGUID guidService, REFIID riid, LPVOID * ppvObject) override;
		STDMETHODIMP InitServicePointers(IMFTopologyServiceLookup * pLookup) override;
		STDMETHODIMP ReleaseServicePointers(void) override;
		STDMETHODIMP GetNativeVideoSize(SIZE * pszVideo, SIZE * pszARVideo) override;
		STDMETHODIMP GetIdealVideoSize(SIZE * pszMin, SIZE * pszMax) override;
		STDMETHODIMP SetVideoPosition(const MFVideoNormalizedRect * pnrcSource, const LPRECT prcDest) override;
		STDMETHODIMP GetVideoPosition(MFVideoNormalizedRect * pnrcSource, LPRECT prcDest) override;
		STDMETHODIMP SetAspectRatioMode(DWORD dwAspectRatioMode) override;
		STDMETHODIMP GetAspectRatioMode(DWORD * pdwAspectRatioMode) override;
		STDMETHODIMP SetVideoWindow(HWND hwndVideo) override;
		STDMETHODIMP GetVideoWindow(HWND * phwndVideo) override;
		STDMETHODIMP RepaintVideo(void) override;
		STDMETHODIMP GetCurrentImage(BITMAPINFOHEADER * pBih, BYTE ** pDib, DWORD * pcbDib, LONGLONG * pTimeStamp) override;
		STDMETHODIMP SetBorderColor(COLORREF Clr) override;
		STDMETHODIMP GetBorderColor(COLORREF * pClr) override;
		STDMETHODIMP SetRenderingPrefs(DWORD dwRenderFlags) override;
		STDMETHODIMP GetRenderingPrefs(DWORD * pdwRenderFlags) override;
		STDMETHODIMP SetFullscreen(BOOL fFullscreen) override;
		STDMETHODIMP GetFullscreen(BOOL * pfFullscreen) override;
	private:
		enum class FrameStepState
		{
			None,
			WaitingStart,
			Pending,
			Scheduled,
			Complete
		};
		
		struct FrameStep
		{
			FrameStep() : State(FrameStepState::None), Steps(0), pSampleNoRef(NULL)
			{
			}

			FrameStepState     State;          // Current frame-step state.
			std::queue<WRL::ComPtr<IMFSample>>     Samples;        // List of pending samples for frame-stepping.
			DWORD               Steps;          // Number of steps left.
			DWORD_PTR           pSampleNoRef;   // Identifies the frame-step sample.
		};

		bool IsActive() const noexcept;
		void CheckShutdown() const;

		void ConfigureMixer();
		void Flush();
		void SetMediaType(IMFMediaType* mediaType);
		void RenegotiateMediaType();
		bool IsMediaTypeSupported(IMFMediaType* mediaType);
		WRL::ComPtr<IMFMediaType> CreateOptimalVideoType(IMFMediaType* proposedMT);
		void NotifyEvent(long EventCode, LONG_PTR Param1, LONG_PTR Param2);
		void ProcessOutput();

		float GetMaxRate(bool thin);
		void StartFrameStep();
		void DeliverFrameStepSample(IMFSample* sample);
		void DeliverSample(IMFSample* sample, bool flag);
	private:
		D3D9VideoRenderer _d3d9Renderer;

		std::atomic<EVRPresenterState> _state;
		WRL::Wrappers::CriticalSection _stateLock;
		WRL::ComPtr<IMFClock> _clock;
		WRL::ComPtr<IMFTransform> _mixer;
		WRL::ComPtr<IMediaEventSink> _mediaEventSink;
		WRL::ComPtr<IMFMediaType> _mediaType;
		MFVideoNormalizedRect _normalizedVideoSrc;
		bool _needRepaint = false;
		bool _preRolled = false;
		FrameStep _frameStep;
	};

	class EVRPresenterActivate : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IMFActivate>
	{
	public:
		EVRPresenterActivate();

		// 通过 RuntimeClass 继承
		STDMETHODIMP GetItem(REFGUID guidKey, PROPVARIANT * pValue) override;
		STDMETHODIMP GetItemType(REFGUID guidKey, MF_ATTRIBUTE_TYPE * pType) override;
		STDMETHODIMP CompareItem(REFGUID guidKey, REFPROPVARIANT Value, BOOL * pbResult) override;
		STDMETHODIMP Compare(IMFAttributes * pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL * pbResult) override;
		STDMETHODIMP GetUINT32(REFGUID guidKey, UINT32 * punValue) override;
		STDMETHODIMP GetUINT64(REFGUID guidKey, UINT64 * punValue) override;
		STDMETHODIMP GetDouble(REFGUID guidKey, double * pfValue) override;
		STDMETHODIMP GetGUID(REFGUID guidKey, GUID * pguidValue) override;
		STDMETHODIMP GetStringLength(REFGUID guidKey, UINT32 * pcchLength) override;
		STDMETHODIMP GetString(REFGUID guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32 * pcchLength) override;
		STDMETHODIMP GetAllocatedString(REFGUID guidKey, LPWSTR * ppwszValue, UINT32 * pcchLength) override;
		STDMETHODIMP GetBlobSize(REFGUID guidKey, UINT32 * pcbBlobSize) override;
		STDMETHODIMP GetBlob(REFGUID guidKey, UINT8 * pBuf, UINT32 cbBufSize, UINT32 * pcbBlobSize) override;
		STDMETHODIMP GetAllocatedBlob(REFGUID guidKey, UINT8 ** ppBuf, UINT32 * pcbSize) override;
		STDMETHODIMP GetUnknown(REFGUID guidKey, REFIID riid, LPVOID * ppv) override;
		STDMETHODIMP SetItem(REFGUID guidKey, REFPROPVARIANT Value) override;
		STDMETHODIMP DeleteItem(REFGUID guidKey) override;
		STDMETHODIMP DeleteAllItems(void) override;
		STDMETHODIMP SetUINT32(REFGUID guidKey, UINT32 unValue) override;
		STDMETHODIMP SetUINT64(REFGUID guidKey, UINT64 unValue) override;
		STDMETHODIMP SetDouble(REFGUID guidKey, double fValue) override;
		STDMETHODIMP SetGUID(REFGUID guidKey, REFGUID guidValue) override;
		STDMETHODIMP SetString(REFGUID guidKey, LPCWSTR wszValue) override;
		STDMETHODIMP SetBlob(REFGUID guidKey, const UINT8 * pBuf, UINT32 cbBufSize) override;
		STDMETHODIMP SetUnknown(REFGUID guidKey, IUnknown * pUnknown) override;
		STDMETHODIMP LockStore(void) override;
		STDMETHODIMP UnlockStore(void) override;
		STDMETHODIMP GetCount(UINT32 * pcItems) override;
		STDMETHODIMP GetItemByIndex(UINT32 unIndex, GUID * pguidKey, PROPVARIANT * pValue) override;
		STDMETHODIMP CopyAllItems(IMFAttributes * pDest) override;

		STDMETHODIMP ActivateObject(REFIID riid, void ** ppv) override;
		STDMETHODIMP ShutdownObject(void) override;
		STDMETHODIMP DetachObject(void) override;
	private:
		WRL::ComPtr<IMFAttributes> _attributes;
		WRL::ComPtr<IMFVideoPresenter> _object;
	};
}