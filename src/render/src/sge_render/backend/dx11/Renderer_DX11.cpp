#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"

namespace sge {
#if 0
	Renderer_DX11::Renderer_DX11(CreateDesc& desc)
	{
		D3D_FEATURE_LEVEL featureLevel;
		UINT createDeviceFlags = 0;

#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		HRESULT hr;

		{ // d3dDevice, d3dDeviceContext
			ComPtr<ID3D11Device> d3dDevice;
			ComPtr<ID3D11DeviceContext> d3dDeviceContext;

			hr = D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				createDeviceFlags,
				nullptr,
				0,
				D3D11_SDK_VERSION,
				d3dDevice.ptrForInit(),
				nullptr,
				d3dDeviceContext.ptrForInit());

			Util::throwIfError(hr);
			//This method returns DXGI_ERROR_SDK_COMPONENT_MISSING if you specify D3D11_CREATE_DEVICE_DEBUG in Flags
			//and the incorrect version of the debug layer is installed on your computer. Install the latest Windows SDK to get the correct version.
	
			// d3dDevice			
			hr = d3dDevice->QueryInterface(IID_PPV_ARGS(_d3dDevice.ptrForInit()));
			Util::throwIfError(hr);

			// d3dDeviceContext
			hr = d3dDeviceContext->QueryInterface(IID_PPV_ARGS(_d3dDeviceContext.ptrForInit()));
			Util::throwIfError(hr);
		}

		{ // d3dDevice -> dxgiDevice : https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nn-dxgi-idxgidevice
			hr = _d3dDevice->QueryInterface(IID_PPV_ARGS(_dxgiDevice.ptrForInit()));
			Util::throwIfError(hr);
		}

		{ // dxgiDevice -> dxgiAdapter
			ComPtr<IDXGIAdapter> dxgiAdapter;
			hr = _dxgiDevice->GetAdapter(dxgiAdapter.ptrForInit());
			Util::throwIfError(hr);

			hr = dxgiAdapter->QueryInterface(IID_PPV_ARGS(_dxgiAdapter.ptrForInit()));
			Util::throwIfError(hr);
		}

		{ // dxgiAdapter -> dxgiFactory
			hr = _dxgiAdapter->GetParent(IID_PPV_ARGS(_dxgiFactory.ptrForInit()));
			Util::throwIfError(hr);
		}

		{ // d3dDevice -> d3dDebug
			if (createDeviceFlags & D3D11_CREATE_DEVICE_DEBUG) {
				hr = _d3dDevice->QueryInterface(_d3dDebug.ptrForInit());
				Util::throwIfError(hr);
			}
		}

		// ----------------- init adapter info

		{ // multithread?
			D3D11_FEATURE_DATA_THREADING s = {};
			hr = _d3dDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &s, sizeof(s));
			Util::throwIfError(hr);
			_adapterInfo.multithread = s.DriverConcurrentCreates && s.DriverCommandLists;
			if (!_adapterInfo.multithread)
				desc.multithread = false;
		}

		{ // compute shader?
			//D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS s = {};
			//hr = _d3dDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &s, sizeof(s));
			//Util::throwIfError(hr);
			//_adapterInfo.computeShader = s.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x;
			_adapterInfo.hasComputeShader = true;
		}

		{ // double?
			D3D11_FEATURE_DATA_DOUBLES s = {};
			hr = _d3dDevice->CheckFeatureSupport(D3D11_FEATURE_DOUBLES, &s, sizeof(s));
			Util::throwIfError(hr);
			_adapterInfo.shaderHasFloat64 = s.DoublePrecisionFloatShaderOps;
		}

		{ // adapter desc
			DXGI_ADAPTER_DESC ad;
			hr = _dxgiAdapter->GetDesc(&ad);
			Util::throwIfError(hr);

			_adapterInfo.adapterName = UtfUtil::toString(ad.Description);
			_adapterInfo.memorySize = ad.DedicatedVideoMemory;

			SGE_LOG("Render Adapter\n  name={}\n  mem={}G"
				, _adapterInfo.adapterName
				, Math::ByteToG(_adapterInfo.memorySize));
		}
	}
#endif
	Renderer_DX11::Renderer_DX11(CreateDesc& desc) {
		D3D_FEATURE_LEVEL featureLevel;
		UINT createDeviceFlags = 0;

#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		ComPtr<ID3D11Device>			d3dDevice;
		ComPtr<ID3D11DeviceContext>		d3dDeviceContext;

		HRESULT hr;
		hr = D3D11CreateDevice(nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createDeviceFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			d3dDevice.ptrForInit(),
			&featureLevel,
			d3dDeviceContext.ptrForInit());
		Util::throwIfError(hr);
		//This method returns DXGI_ERROR_SDK_COMPONENT_MISSING if you specify D3D11_CREATE_DEVICE_DEBUG in Flags
		//and the incorrect version of the debug layer is installed on your computer. Install the latest Windows SDK to get the correct version.

		hr = d3dDevice->QueryInterface(IID_PPV_ARGS(_d3dDevice.ptrForInit()));
		Util::throwIfError(hr);

		hr = d3dDeviceContext->QueryInterface(IID_PPV_ARGS(_d3dDeviceContext.ptrForInit()));
		Util::throwIfError(hr);

		hr = _d3dDevice->QueryInterface(IID_PPV_ARGS(_dxgiDevice.ptrForInit()));
		Util::throwIfError(hr);

		{
			ComPtr<IDXGIAdapter> adapter;
			hr = _dxgiDevice->GetAdapter(adapter.ptrForInit());
			Util::throwIfError(hr);

			hr = adapter->QueryInterface(IID_PPV_ARGS(_dxgiAdapter.ptrForInit()));
			Util::throwIfError(hr);
		}

		{
			D3D11_FEATURE_DATA_THREADING s = {};
			hr = d3dDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &s, sizeof(s));
			Util::throwIfError(hr);
			_adapterInfo.multithread = s.DriverConcurrentCreates && s.DriverCommandLists;
			if (!_adapterInfo.multithread)
				desc.multithread = false;
		}

		//{
		//	D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS s = {};
		//	hr = d3dDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &s, sizeof(s));
		//	Util::throwIfError(hr);
		//	_adapterInfo.computeShader = s.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x;
		//}
		_adapterInfo.hasComputeShader = true;

		{
			D3D11_FEATURE_DATA_DOUBLES s = {};
			hr = d3dDevice->CheckFeatureSupport(D3D11_FEATURE_DOUBLES, &s, sizeof(s));
			Util::throwIfError(hr);
			_adapterInfo.shaderHasFloat64 = s.DoublePrecisionFloatShaderOps;
		}

		{
			DXGI_ADAPTER_DESC ad;
			hr = _dxgiAdapter->GetDesc(&ad);
			Util::throwIfError(hr);

			_adapterInfo.adapterName = UtfUtil::toString(ad.Description);
			_adapterInfo.memorySize = ad.DedicatedVideoMemory;

			SGE_LOG("Render Adapter\n  name={}\n  mem={}G"
				, _adapterInfo.adapterName
				, Math::ByteToG(_adapterInfo.memorySize));
		}

		hr = _dxgiAdapter->GetParent(IID_PPV_ARGS(_dxgiFactory.ptrForInit()));
		Util::throwIfError(hr);

		if (createDeviceFlags & D3D11_CREATE_DEVICE_DEBUG) {
			hr = _d3dDevice->QueryInterface(_d3dDebug.ptrForInit());
			Util::throwIfError(hr);
		}

	}

	RenderContext* Renderer_DX11::onCreateRenderContext(RenderContext_CreateDesc& desc) {
		return new RenderContext_DX11(desc);
	}
	
} // namespace