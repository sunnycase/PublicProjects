#pragma once
#include <Tomato.Core/Tomato.Core.h>
#include <mfidl.h>
#include <evr.h>
#include <d3d9.h>
#include <evr9.h>

namespace CES
{
	class D3D9VideoRenderer
	{
	public:
		D3D9VideoRenderer();

		bool GetFullscreen() const noexcept { return _isFullscreen; }
		void SetFullscreen(bool value);

		HWND GetVideoWindow() const noexcept { return _videoHWnd; }
		void SetVideoWindow(HWND hWnd);

		RECT GetDestinationRect() const noexcept { return _destRect; }
		void SetDestinationRect(const RECT& rect);

		STDMETHODIMP GetService(REFGUID guidService, REFIID riid, LPVOID * ppvObject);
	private:
		void CreateDeviceIndependentResources();
		void CreateDeviceDependentResources();
	private:
		bool _isFullscreen;
		HWND _videoHWnd = nullptr;
		RECT _destRect;
		WRL::ComPtr<IDirect3D9Ex> _d3d9;
		WRL::ComPtr<IDirect3DDevice9Ex> _d3dDevice;
		WRL::ComPtr<IDirect3DDeviceManager9> _deviceManager;
		UINT32 _resetToken;
		WRL::ComPtr<IDirectXVideoProcessorService> _videoService;

		DXVA2_VideoDesc _videoDesc;

		WRL::Wrappers::CriticalSection _stateLock;
	};
}