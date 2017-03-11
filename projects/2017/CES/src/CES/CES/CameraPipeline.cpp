#include "stdafx.h"
#include "CameraPipeline.h"
#include <Tomato.Media/MFWorkerQueueProvider.h>

using namespace WRL;
using namespace CES;

CameraPipeline::CameraPipeline()
	:_operationQueue(std::make_shared<NS_CORE::OperationQueue<CamraPipelineOperation>>(
		[weak = AsWeak()](CamraPipelineOperation& op)
{
	if (auto me = weak.Resolve<CameraPipeline>()) me->OnDispatchOperation(op);
}))
{
	ThrowIfFailed(MFStartup(MFSTARTUP_LITE));
	_operationQueue->SetWorkerQueue(NS_MEDIA::MFWorkerQueueProvider::GetAudio());

	CreateSession();
	CreateDeviceDependendResources();
}

void CameraPipeline::OnMediaSessionEvent(IMFAsyncResult * pAsyncResult)
{
	ComPtr<IMFMediaEvent> event;
	ThrowIfFailed(_mediaSession->EndGetEvent(pAsyncResult, &event));

	MediaEventType eventType;
	ThrowIfFailed(event->GetType(&eventType));
	if (eventType != MESessionClosed)
		ThrowIfFailed(_mediaSession->BeginGetEvent(_mediaSessionAsyncCallback.Get(), nullptr));
	_operationQueue->Enqueue(CamraPipelineOperation{ CamraPipelineOperationKind::None });
}

void CameraPipeline::OnDispatchOperation(CamraPipelineOperation & op)
{
}

void CameraPipeline::OpenCamera(CameraSource source, HWND videohWnd)
{
	ComPtr<IMFTopology> topology;
	ThrowIfFailed(MFCreateTopology(&topology));

	IMFMediaSource* mediaSource = source == CameraSource::Camera ? _cameraSource.Get() : _scannerSource.Get();
	if (!mediaSource)
		ThrowAlways(L"����ͷδ���ӣ�������δ��װ��");
	ConfigureTopology(topology.Get(), mediaSource, videohWnd);
	ThrowIfFailed(_mediaSession->SetTopology(0, topology.Get()));
}

void CameraPipeline::CreateDeviceDependendResources()
{
	ComPtr<IMFAttributes> attributes;
	ThrowIfFailed(MFCreateAttributes(&attributes, 1));

	ThrowIfFailed(attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID));

	unique_cotaskmem<ComPtr<IMFActivate>> activators;
	UINT32 count;
	ThrowIfFailed(MFEnumDeviceSources(attributes.Get(), reinterpret_cast<IMFActivate***>(&activators._Myptr()), &count));
	if (count > 0)
		ThrowIfFailed(activators.get()[0]->ActivateObject(IID_PPV_ARGS(&_cameraSource)));
	if (count > 1)
		ThrowIfFailed(activators.get()[1]->ActivateObject(IID_PPV_ARGS(&_scannerSource)));
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

	bool CreateMediaSinkActivator(IMFStreamDescriptor* streamDesc, HWND hWnd, IMFActivate** activate)
	{
		ComPtr<IMFMediaTypeHandler> mtHandler;
		ThrowIfFailed(streamDesc->GetMediaTypeHandler(&mtHandler));

		GUID majorType;
		ThrowIfFailed(mtHandler->GetMajorType(&majorType));
		if (majorType == MFMediaType_Video)
		{
			ThrowIfFailed(MFCreateVideoRendererActivate(hWnd, activate));
			return true;
		}
		return false;
	}

	void AddBranchToPartialTopology(IMFTopology * topology, IMFMediaSource * source, IMFPresentationDescriptor* pd, DWORD streamId, HWND hWnd)
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
