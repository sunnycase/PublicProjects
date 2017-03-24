#pragma once
#include <Tomato.Core/Tomato.Core.h>
#include <Tomato.Media/MFAsyncCallback.h>
#include <Tomato.Core/OperationQueue.h>
#include <vector>
#include <evr.h>
#include <d3d9.h>
#include <evr9.h>
#include <afxwin.h>

namespace CES
{
	enum class CameraSource
	{
		Camera,
		Scanner
	};

	enum class CameraPipelineOperationKind
	{
		// 无
		None,
		// 开始
		Start,
		// 暂停
		Pause,
		// 停止
		Stop,
		// 设置速率
		SetRate,
		// 请求数据
		RequestData,
		// 到达结尾
		EndOfStream,
		Event
	};

	class CameraPipelineOperation
	{
	public:
		CameraPipelineOperation(CameraPipelineOperationKind kind) noexcept
			: kind(kind)
		{

		}

		virtual ~CameraPipelineOperation()
		{

		}

		CameraPipelineOperation() noexcept
			: CameraPipelineOperation(CameraPipelineOperationKind::None)
		{

		}

		// 获取类型
		CameraPipelineOperationKind GetKind() const noexcept { return kind; }
	private:
		CameraPipelineOperationKind kind;
	};

	template<class THandler>
	class Event
	{
		std::shared_ptr<std::vector<std::function<THandler>>> _functions;
	public:
		template<class TCallable>
		void Subscribe(TCallable f)
		{
			auto newFuncs = std::make_shared<std::vector<std::function<THandler>>>();
			auto oldFuncs = std::atomic_load(&_functions);
			if (oldFuncs)
			{
				newFuncs->reserve(oldFuncs->size() + 1);
				*newFuncs = *oldFuncs;
			}
			newFuncs->emplace_back(std::forward<TCallable>(f));
			std::atomic_store(&_functions, std::move(newFuncs));
		}

		template<class... Param>
		void Publish(Param&&... x)
		{
			auto oldFuncs = std::atomic_load(&_functions);
			if (oldFuncs)
				for (auto f : *oldFuncs) f(std::forward<Param>(x)...);
		}

		void Publish()
		{
			auto oldFuncs = std::atomic_load(&_functions);
			if (oldFuncs)
				for (auto f : *oldFuncs) f();
		}
	};

	class CameraPipeline : public NS_CORE::WeakReferenceBase<CameraPipeline, WRL::RuntimeClassFlags<WRL::ClassicCom>, IUnknown>
	{
	public:
		CameraPipeline();

		void OpenCamera(CameraSource source, HWND videohWnd);
		void Start();
		void TakePicture(CBitmap& bitmap);
		void OnResize(HWND videohWnd);

		Event<void()> DeviceReady;
	private:
		void CreateDeviceDependentResources();
		void CreateDeviceIndependentResources();
		void CreateSession();
		void ConfigureTopology(IMFTopology* topology, IMFMediaSource* source, HWND videohWnd);
		void AddBranchToPartialTopology(IMFTopology * topology, IMFMediaSource * source, IMFPresentationDescriptor* pd, DWORD streamId, HWND hWnd);
		void OnMediaSessionEvent(IMFAsyncResult* pAsyncResult);
		void OnDispatchOperation(std::shared_ptr<CameraPipelineOperation>& op);

		void ProcessMediaSessionEvent(IMFMediaEvent* event);
		void ProcessStart();
		void ProcessSessionTopologyStatus(IMFMediaEvent* event);
	private:
		WRL::ComPtr<IMFMediaSource> _cameraSource;
		WRL::ComPtr<IMFMediaSource> _scannerSource;
		WRL::ComPtr<IMFMediaSession> _mediaSession;
		WRL::ComPtr<IMFVideoDisplayControl> _videoDispCtrl;
		WRL::ComPtr<IMFVideoProcessor> _videoProcessor;
		CameraSource _source;
		WRL::ComPtr<NS_MEDIA::MFAsyncCallbackWithWeakRef<CameraPipeline>> _mediaSessionAsyncCallback;
		std::shared_ptr<NS_CORE::OperationQueue<std::shared_ptr<CameraPipelineOperation>>> _operationQueue;
	};
}