#include "stdafx.h"
#include "CameraPipeline.h"
#include <Tomato.Media/MFWorkerQueueProvider.h>
#include <DirectXMath.h>

using namespace WRL;
using namespace CES;

namespace
{
	class CamraPipelineMediaEventOperation : public CameraPipelineOperation
	{
	public:
		CamraPipelineMediaEventOperation(IMFMediaEvent* event) noexcept
			: CameraPipelineOperation(CameraPipelineOperationKind::Event), _event(event)
		{

		}

		IMFMediaEvent* GetEvent() const noexcept { return _event.Get(); }
	private:
		ComPtr<IMFMediaEvent> _event;
	};
}

CameraPipeline::CameraPipeline()
	:_operationQueue(std::make_shared<NS_CORE::OperationQueue<std::shared_ptr<CameraPipelineOperation>>>(
		[weak = AsWeak()](std::shared_ptr<CameraPipelineOperation>& op)
{
	if (auto me = weak.Resolve<CameraPipeline>()) me->OnDispatchOperation(op);
}))
{
	ThrowIfFailed(MFStartup(MFSTARTUP_LITE));
	_operationQueue->SetWorkerQueue(NS_MEDIA::MFWorkerQueueProvider::GetAudio());

	CreateSession();
	CreateDeviceIndependentResources();
	CreateDeviceDependentResources();
}

void CameraPipeline::OnMediaSessionEvent(IMFAsyncResult * pAsyncResult)
{
	ComPtr<IMFMediaEvent> event;
	ThrowIfFailed(_mediaSession->EndGetEvent(pAsyncResult, &event));

	MediaEventType eventType;
	ThrowIfFailed(event->GetType(&eventType));
	if (eventType != MESessionClosed)
		ThrowIfFailed(_mediaSession->BeginGetEvent(_mediaSessionAsyncCallback.Get(), nullptr));
	_operationQueue->Enqueue(std::make_shared<CamraPipelineMediaEventOperation>(event.Get()));
}

void CameraPipeline::OnDispatchOperation(std::shared_ptr<CameraPipelineOperation>& op)
{
	auto& opRef = *op;
	switch (opRef.GetKind())
	{
	case CameraPipelineOperationKind::Event:
		ProcessMediaSessionEvent(static_cast<CamraPipelineMediaEventOperation&>(opRef).GetEvent());
		break;
	case CameraPipelineOperationKind::Start:
		ProcessStart();
		break;
	default:
		break;
	}
}

void CameraPipeline::ProcessMediaSessionEvent(IMFMediaEvent * event)
{
	MediaEventType eventType;
	ThrowIfFailed(event->GetType(&eventType));

	HRESULT hr;
	ThrowIfFailed(event->GetStatus(&hr));
	ThrowIfFailed(hr);

	switch (eventType)
	{
	case MESessionTopologyStatus:
		ProcessSessionTopologyStatus(event);
		break;
	default:
		break;
	}

}

void CameraPipeline::ProcessStart()
{
	auto session = _mediaSession;
	if (session)
	{
		PROPVARIANT varStart;
		PropVariantInit(&varStart);
		ThrowIfFailed(session->Start(&GUID_NULL, &varStart));
	}
}

void CameraPipeline::ProcessSessionTopologyStatus(IMFMediaEvent * event)
{
	UINT32 status;
	ThrowIfFailed(event->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, &status));
	if (status == MF_TOPOSTATUS_READY)
	{
		ThrowIfFailed(MFGetService(_mediaSession.Get(), MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&_videoDispCtrl)));
		ThrowIfFailed(MFGetService(_mediaSession.Get(), MR_VIDEO_MIXER_SERVICE, IID_PPV_ARGS(&_videoProcessor)));

		DeviceReady.Publish();
	}
	else if(status == MF_TOPOSTATUS_STARTED_SOURCE)
	{
		DXVA2_ValueRange saturationRange;
		ThrowIfFailed(_videoProcessor->GetProcAmpRange(DXVA2_ProcAmp_Saturation, &saturationRange));
		//_videoDispCtrl->GetCurrentImage()
		DXVA2_ProcAmpValues values{};

		// 文件、单据、证件: 黑白
		if (_source == CameraSource::Scanner)
			values.Saturation = saturationRange.MinValue;
		else
			values.Saturation = saturationRange.DefaultValue;
		ThrowIfFailed(_videoProcessor->SetProcAmpValues(DXVA2_ProcAmp_Saturation, &values));
	}
}

void CameraPipeline::OpenCamera(CameraSource source, HWND videohWnd)
{
	IMFMediaSource* mediaSource = source == CameraSource::Camera ? _cameraSource.Get() : _scannerSource.Get();
	if (!mediaSource)
		ThrowAlways(L"摄像头未连接，或驱动未安装。");
	_source = source;

	ComPtr<IMFTopology> topology;
	ThrowIfFailed(MFCreateTopology(&topology));

	ConfigureTopology(topology.Get(), mediaSource, videohWnd);
	ThrowIfFailed(_mediaSession->SetTopology(0, topology.Get()));
}

void CameraPipeline::Start()
{
	_operationQueue->Enqueue(std::make_shared<CameraPipelineOperation>(CameraPipelineOperationKind::Start));
}

#define DIB_WIDTHBYTES(bits) ((((bits) + 31)>>5)<<2)

void CES::CameraPipeline::TakePicture(CBitmap& bitmap)
{
	auto dispCtrl = _videoDispCtrl;
	ThrowIfNot(dispCtrl, L"扫描未开始。");

	BITMAPINFOHEADER biHeader{};
	biHeader.biSize = sizeof(biHeader);
	unique_cotaskmem_arr<BYTE> bitmapData;
	DWORD dataSize;
	LONGLONG timestamp;
	ThrowIfFailed(dispCtrl->GetCurrentImage(&biHeader, &bitmapData._Myptr(), &dataSize, &timestamp));

	bitmap.CreateBitmap(biHeader.biWidth, biHeader.biHeight, biHeader.biPlanes, biHeader.biBitCount, bitmapData.get());
}

void CES::CameraPipeline::OnResize(HWND videohWnd)
{
	if (auto videoDispCtrl = _videoDispCtrl)
	{
		RECT rect;
		ThrowWin32IfNot(GetClientRect(videohWnd, &rect));
		static const MFVideoNormalizedRect nrect{ 0, 0, 1, 1 };
		ThrowIfFailed(videoDispCtrl->SetVideoPosition(&nrect, &rect));
	}
}

void CameraPipeline::CreateDeviceDependentResources()
{
	ComPtr<IMFAttributes> attributes;
	ThrowIfFailed(MFCreateAttributes(&attributes, 1));

	ThrowIfFailed(attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID));

	unique_cotaskmem_arr<ComPtr<IMFActivate>> activators;
	UINT32 count;
	ThrowIfFailed(MFEnumDeviceSources(attributes.Get(), reinterpret_cast<IMFActivate***>(&activators._Myptr()), &count));
	if (count > 0)
		ThrowIfFailed(activators[0]->ActivateObject(IID_PPV_ARGS(&_scannerSource)));
	if (count > 1)
		ThrowIfFailed(activators[1]->ActivateObject(IID_PPV_ARGS(&_cameraSource)));
}

#include <wmcodecdsp.h>

void CameraPipeline::CreateDeviceIndependentResources()
{

}

void CameraPipeline::CreateSession()
{
	ThrowIfFailed(MFCreateMediaSession(nullptr, &_mediaSession));

	_mediaSessionAsyncCallback = Make<NS_MEDIA::MFAsyncCallbackWithWeakRef<CameraPipeline>>(GetWeakContext(), &CameraPipeline::OnMediaSessionEvent);
	ThrowIfFailed(_mediaSession->BeginGetEvent(_mediaSessionAsyncCallback.Get(), nullptr));
}

namespace
{
	void AddSourceNode(IMFTopology * topology, IMFMediaSource * source, IMFPresentationDescriptor* pd, IMFStreamDescriptor* streamDesc, IMFTopologyNode** node)
	{
		ComPtr<IMFTopologyNode> sourceNode;
		ThrowIfFailed(MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &sourceNode));
		ThrowIfFailed(sourceNode->SetUnknown(MF_TOPONODE_SOURCE, source));
		ThrowIfFailed(sourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pd));
		ThrowIfFailed(sourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, streamDesc));
		ThrowIfFailed(topology->AddNode(sourceNode.Get()));
		*node = sourceNode.Detach();
	}

	void AddOutputNode(IMFTopology * topology, IMFActivate * sinkActivate, DWORD streamId, IMFTopologyNode** node)
	{
		ComPtr<IMFTopologyNode> outputNode;
		ThrowIfFailed(MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &outputNode));
		ThrowIfFailed(outputNode->SetObject(sinkActivate));
		ThrowIfFailed(outputNode->SetUINT32(MF_TOPONODE_STREAMID, streamId));
		ThrowIfFailed(outputNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE));
		ThrowIfFailed(topology->AddNode(outputNode.Get()));
		*node = outputNode.Detach();
	}

	void AddTransformNode(IMFTopology * topology, const CLSID& clsid, IMFTopologyNode** node)
	{
		ComPtr<IMFTopologyNode> transformNode;
		ThrowIfFailed(MFCreateTopologyNode(MF_TOPOLOGY_TRANSFORM_NODE, &transformNode));
		ThrowIfFailed(transformNode->SetGUID(MF_TOPONODE_TRANSFORM_OBJECTID, clsid));
		ThrowIfFailed(topology->AddNode(transformNode.Get()));
		*node = transformNode.Detach();
	}

	void AddTransformNode(IMFTopology * topology, IMFTransform* transform, IMFTopologyNode** node)
	{
		ComPtr<IMFTopologyNode> transformNode;
		ThrowIfFailed(MFCreateTopologyNode(MF_TOPOLOGY_TRANSFORM_NODE, &transformNode));
		ThrowIfFailed(transformNode->SetObject(transform));
		ThrowIfFailed(topology->AddNode(transformNode.Get()));
		*node = transformNode.Detach();
	}

	bool CreateMediaSinkActivator(IMFStreamDescriptor* streamDesc, HWND hWnd, IMFActivate** activate)
	{
		ComPtr<IMFMediaTypeHandler> mtHandler;
		ThrowIfFailed(streamDesc->GetMediaTypeHandler(&mtHandler));

		GUID majorType;
		ThrowIfFailed(mtHandler->GetMajorType(&majorType));
		ComPtr<IMFMediaType> mediaType;
		if (majorType == MFMediaType_Video)
		{
			ThrowIfFailed(MFCreateVideoRendererActivate(hWnd, activate));
			return true;
		}
		return false;
	}
}

void CameraPipeline::ConfigureTopology(IMFTopology * topology, IMFMediaSource * source, HWND videohWnd)
{
	ComPtr<IMFPresentationDescriptor> pd;
	ThrowIfFailed(source->CreatePresentationDescriptor(&pd));

	DWORD streamDescCount;
	ThrowIfFailed(pd->GetStreamDescriptorCount(&streamDescCount));
	for (DWORD i = 0; i < streamDescCount; i++)
		AddBranchToPartialTopology(topology, source, pd.Get(), i, videohWnd);
}

void CameraPipeline::AddBranchToPartialTopology(IMFTopology * topology, IMFMediaSource * source, IMFPresentationDescriptor* pd, DWORD streamId, HWND hWnd)
{
	BOOL isSelected;
	ComPtr<IMFStreamDescriptor> streamDesc;
	ThrowIfFailed(pd->GetStreamDescriptorByIndex(streamId, &isSelected, &streamDesc));
	if (isSelected)
	{
		ComPtr<IMFActivate> sinkActivate;
		if (CreateMediaSinkActivator(streamDesc.Get(), hWnd, &sinkActivate))
		{
			ComPtr<IMFTopologyNode> sourceNode;
			AddSourceNode(topology, source, pd, streamDesc.Get(), &sourceNode);

			ComPtr<IMFTopologyNode> outputNode;
			AddOutputNode(topology, sinkActivate.Get(), streamId, &outputNode);
			ThrowIfFailed(sourceNode->ConnectOutput(0, outputNode.Get(), 0));
		}
	}
}