#pragma once
#include <Tomato.Core/Tomato.Core.h>
#include <Tomato.Media/MFAsyncCallback.h>
#include <Tomato.Core/OperationQueue.h>
#include <vector>
#include <afxwin.h>
#include <d3d9.h>
#include <vmr9.h>

namespace CES
{
	enum class CameraSource
	{
		Camera,
		Scanner
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

	struct CameraDeviceInfo
	{
		std::wstring FriendlyName;
		std::wstring SymbolicLink;
	};

	class CameraPipeline : public NS_CORE::WeakReferenceBase<CameraPipeline, WRL::RuntimeClassFlags<WRL::ClassicCom>, IUnknown>
	{
	public:
		CameraPipeline();

		void OpenCamera(CameraSource source, HWND videohWnd);
		void Start();
		void TakePicture(CBitmap& bitmap);
		void OnResize(HWND videohWnd);
		void OnPaint(HWND videohWnd, HDC videoDC);
		void OnDisplayChange();
		void InitializeDevice(CameraSource source, LPCWSTR symbolicLink);

		static std::vector<CameraDeviceInfo> EnumerateDevices();

		Event<void()> DeviceReady;
	private:
		void CreateDeviceDependentResources();
		void CreateDeviceIndependentResources();
		void CreateGraph(ICaptureGraphBuilder2** captureGraphBuilder, IGraphBuilder** graph, IBaseFilter** vmrFilter);
		void ConfigureGraph(ICaptureGraphBuilder2* captureGraphBuilder, IGraphBuilder* graph, IBaseFilter* source, IBaseFilter* vmrFilter, HWND videohWnd, IVMRWindowlessControl9** videoControl);
	private:
		WRL::ComPtr<ICaptureGraphBuilder2> _captureGraphBuilder;
		WRL::ComPtr<IGraphBuilder> _graphBuilder;
		CameraSource _source;
		WRL::ComPtr<IBaseFilter> _cameraSource;
		WRL::ComPtr<IBaseFilter> _scannerSource;
		WRL::ComPtr<IVMRWindowlessControl9> _videoControl;
		WRL::ComPtr<IMediaControl> _mediaControl;
	};
}