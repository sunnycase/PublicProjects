#include "stdafx.h"
#include "CameraPipeline.h"
#include <DirectXMath.h>

using namespace WRL;
using namespace CES;

#define REPORT_ERROR(name)																		\
catch (tomato_error& ex) { MessageBox(nullptr, ex.message, name, MB_OK | MB_ICONERROR); throw; }			\
catch (_com_error& ex) { MessageBox(nullptr, ex.ErrorMessage(), name, MB_OK | MB_ICONERROR); throw; }	\
catch (...) { MessageBox(nullptr, L"意外错误", name, MB_OK | MB_ICONERROR); throw; }

CameraPipeline::CameraPipeline()
{
	CreateDeviceIndependentResources();
	CreateDeviceDependentResources();
}

void CameraPipeline::OpenCamera(CameraSource source, HWND videohWnd)
{
	auto mediaSource = (source == CameraSource::Camera) ? _cameraSource.Get() : _scannerSource.Get();
	if (!mediaSource)
		ThrowIfFailed(HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_AVAILABLE));
	_source = source;

	ComPtr<ICaptureGraphBuilder2> captureGraphBuilder;
	ComPtr<IGraphBuilder> graph;
	ComPtr<IBaseFilter> vmrFilter;
	CreateGraph(&captureGraphBuilder, &graph, &vmrFilter);;
	ComPtr<IVMRWindowlessControl9> videoControl;
	ConfigureGraph(captureGraphBuilder.Get(), graph.Get(), mediaSource, vmrFilter.Get(), videohWnd, &videoControl);

	_videoControl = std::move(videoControl);
	ThrowIfFailed(graph.As(&_mediaControl));

	DeviceReady.Publish();
}

void CameraPipeline::Start()
{
	try
	{
		ThrowIfFailed(_mediaControl->Run());
	}
	REPORT_ERROR(L"Start");
}

#define DIB_WIDTHBYTES(bits) ((((bits) + 31)>>5)<<2)

void CameraPipeline::TakePicture(CBitmap& bitmap)
{
	auto videoControl = _videoControl;
	ThrowIfNot(videoControl, L"扫描未开始。");

	unique_cotaskmem_arr<BYTE> bitmapData;
	ThrowIfFailed(videoControl->GetCurrentImage(&bitmapData._Myptr()));

	auto biHeader = reinterpret_cast<const BITMAPINFOHEADER*>(bitmapData.get());
	bitmap.CreateBitmap(biHeader->biWidth, biHeader->biHeight, biHeader->biPlanes, biHeader->biBitCount,
		bitmapData.get() + sizeof(BITMAPINFOHEADER));
}

void CameraPipeline::OnResize(HWND videohWnd)
{
	if (auto videoControl = _videoControl)
	{
		RECT rect;
		ThrowWin32IfNot(GetClientRect(videohWnd, &rect));
		ThrowIfFailed(videoControl->SetVideoPosition(nullptr, &rect));
	}
}

void CameraPipeline::OnPaint(HWND videohWnd, HDC videoDC)
{
	if (auto videoControl = _videoControl)
	{
		ThrowIfFailed(videoControl->RepaintVideo(videohWnd, videoDC));
	}
}

void CameraPipeline::OnDisplayChange()
{
	if (auto videoControl = _videoControl)
	{
		ThrowIfFailed(videoControl->DisplayModeChanged());
	}
}

namespace
{
	ComPtr<IBaseFilter> CraeteDeviceFilter(std::wstring_view desiredDevicePath)
	{
		ComPtr<ICreateDevEnum> devEnum;
		ThrowIfFailed(CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&devEnum)));
		ComPtr<IEnumMoniker> enumMoniker;
		ThrowIfFailed(devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0));

		ComPtr<IBaseFilter> result;
		ComPtr<IMoniker> moniker;
		while (enumMoniker->Next(1, &moniker, nullptr) == S_OK)
		{
			ComPtr<IPropertyBag> propertyBag;
			if (SUCCEEDED((moniker->BindToStorage(nullptr, nullptr, IID_PPV_ARGS(&propertyBag)))))
			{
				variant_t devicePath;
				if (SUCCEEDED((propertyBag->Read(L"DevicePath", &devicePath, nullptr))))
				{
					if (devicePath.bstrVal == desiredDevicePath)
					{
						ThrowIfFailed(moniker->BindToObject(nullptr, nullptr, IID_PPV_ARGS(&result)));
						break;
					}
				}
			}
		}
		return result;
	}
}

void CameraPipeline::InitializeDevice(CameraSource source, LPCWSTR symbolicLink)
{
	auto device = CraeteDeviceFilter(symbolicLink);
	switch (source)
	{
	case CES::CameraSource::Camera:
		_cameraSource = std::move(device);
		break;
	case CES::CameraSource::Scanner:
		_scannerSource = std::move(device);
		break;
	default:
		break;
	}
}

std::vector<CameraDeviceInfo> CES::CameraPipeline::EnumerateDevices()
{
	try
	{
		ComPtr<ICreateDevEnum> devEnum;
		ThrowIfFailed(CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&devEnum)));
		ComPtr<IEnumMoniker> enumMoniker;
		ThrowIfFailed(devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0));

		std::vector<CameraDeviceInfo> result;
		ComPtr<IMoniker> moniker;
		while (enumMoniker->Next(1, &moniker, nullptr) == S_OK)
		{
			ComPtr<IPropertyBag> propertyBag;
			if (SUCCEEDED((moniker->BindToStorage(nullptr, nullptr, IID_PPV_ARGS(&propertyBag)))))
			{
				variant_t friendlyName, devicePath;
				if (SUCCEEDED((propertyBag->Read(L"FriendlyName", &friendlyName, nullptr))) &&
					SUCCEEDED((propertyBag->Read(L"DevicePath", &devicePath, nullptr))))
				{
					result.emplace_back(CameraDeviceInfo{ friendlyName.bstrVal, devicePath.bstrVal });
				}
			}
		}
		return result;
	}
	REPORT_ERROR(L"EnumerateDevices");
}

void CameraPipeline::CreateDeviceDependentResources()
{

}

void CameraPipeline::CreateDeviceIndependentResources()
{

}

void CameraPipeline::CreateGraph(ICaptureGraphBuilder2** captureGraphBuilder, IGraphBuilder** graph, IBaseFilter** vmrFilter)
{
	ThrowIfFailed(CoCreateInstance(CLSID_CaptureGraphBuilder2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(captureGraphBuilder)));
	ThrowIfFailed(CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(graph)));
	ThrowIfFailed((*captureGraphBuilder)->SetFiltergraph(*graph));

	ThrowIfFailed(CoCreateInstance(CLSID_VideoMixingRenderer9, nullptr, CLSCTX_INPROC, IID_PPV_ARGS(vmrFilter)));
}

void CameraPipeline::ConfigureGraph(ICaptureGraphBuilder2* captureGraphBuilder, IGraphBuilder* graph, IBaseFilter* source, IBaseFilter* vmrFilter, HWND videohWnd, IVMRWindowlessControl9** videoControl)
{
	ThrowIfFailed(graph->AddFilter(source, L"Video Capture Filter"));
	ThrowIfFailed(graph->AddFilter(vmrFilter, L"Video Renderer Filter"));

	{
		ComPtr<IVMRFilterConfig9> vmrConfig;
		ThrowIfFailed(vmrFilter->QueryInterface(IID_PPV_ARGS(&vmrConfig)));
		ThrowIfFailed(vmrConfig->SetRenderingMode(VMRMode_Windowless));
		ThrowIfFailed(vmrFilter->QueryInterface(IID_PPV_ARGS(videoControl)));
		ThrowIfFailed((*videoControl)->SetVideoClippingWindow(videohWnd));

		RECT rect;
		ThrowWin32IfNot(GetClientRect(videohWnd, &rect));
		ThrowWin32IfNot((*videoControl)->SetVideoPosition(nullptr, &rect));
	}
	ThrowIfFailed(captureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, source, nullptr, vmrFilter));
}