#pragma once
#include <Tomato.Core/Tomato.Core.h>
#include <Tomato.Media/MFAsyncCallback.h>
#include <Tomato.Core/OperationQueue.h>
#include <vector>

namespace CES
{
	enum class CameraSource
	{
		Camera,
		Scanner
	};

	enum class CamraPipelineOperationKind
	{
		// ��
		None,
		// ��ʼ
		Start,
		// ��ͣ
		Pause,
		// ֹͣ
		Stop,
		// ��������
		SetRate,
		// ��������
		RequestData,
		// �����β
		EndOfStream
	};

	class CamraPipelineOperation
	{
	public:
		CamraPipelineOperation(CamraPipelineOperationKind kind) noexcept
			: kind(kind)
		{

		}

		virtual ~CamraPipelineOperation()
		{

		}

		CamraPipelineOperation() noexcept
			: CamraPipelineOperation(CamraPipelineOperationKind::None)
		{

		}

		// ��ȡ����
		CamraPipelineOperationKind GetKind() const noexcept { return kind; }
	private:
		CamraPipelineOperationKind kind;
	};

	class CameraPipeline : public NS_CORE::WeakReferenceBase<CameraPipeline, WRL::RuntimeClassFlags<WRL::ClassicCom>, IUnknown>
	{
	public:
		CameraPipeline();

		void OpenCamera(CameraSource source, HWND videohWnd);
	private:
		void CreateDeviceDependendResources();
		void CreateSession();
		void ConfigureTopology(IMFTopology* topology, IMFMediaSource* source, HWND videohWnd);
		void OnMediaSessionEvent(IMFAsyncResult* pAsyncResult);
		void OnDispatchOperation(CamraPipelineOperation& op);
	private:
		WRL::ComPtr<IMFMediaSource> _cameraSource;
		WRL::ComPtr<IMFMediaSource> _scannerSource;
		WRL::ComPtr<IMFMediaSession> _mediaSession;
		WRL::ComPtr<NS_MEDIA::MFAsyncCallbackWithWeakRef<CameraPipeline>> _mediaSessionAsyncCallback;
		std::shared_ptr<NS_CORE::OperationQueue<CamraPipelineOperation>> _operationQueue;
	};
}