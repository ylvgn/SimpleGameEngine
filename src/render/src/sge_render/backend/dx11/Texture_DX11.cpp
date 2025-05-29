#include "Texture_DX11.h"
#include "Renderer_DX11.h"

namespace sge {

#if 0
#pragma mark ========= Texture2D_DX11 ============
#endif
Texture2D_DX11::Texture2D_DX11(CreateDesc& desc)
	: Base(desc)
{
	auto* renderer	= Renderer_DX11::instance();
	auto* dev		= renderer->d3dDevice();

	::D3D11_TEXTURE2D_DESC dxDesc = {};
	dxDesc.Width				  = desc.size.x;
	dxDesc.Height				  = desc.size.y;
	dxDesc.MipLevels			  = desc.mipmapCount;
	dxDesc.ArraySize			  = 1;
	dxDesc.Format				  = Util::getDxColorType(desc.colorType);
	dxDesc.SampleDesc.Count		  = 1;
	dxDesc.SampleDesc.Quality	  = 0;
	dxDesc.Usage				  = D3D11_USAGE_DEFAULT;
	dxDesc.BindFlags			  = D3D11_BIND_SHADER_RESOURCE; // D3D11_BIND_RENDER_TARGET
	dxDesc.CPUAccessFlags		  = 0; // D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
	dxDesc.MiscFlags			  = 0; // D3D11_RESOURCE_MISC_GENERATE_MIPS

	::D3D11_SUBRESOURCE_DATA initData = {};

	if (desc.uploadRequest) {
		const auto& info		= desc.uploadRequest->imageInfo;
		const auto& pixelData	= desc.uploadRequest->pixelData;

		SGE_ASSERT(info.colorType != ColorType::None);
		SGE_ASSERT(info.strideInBytes >= 0);

		initData.pSysMem			= pixelData.data();
		initData.SysMemPitch		= info.strideInBytes;
		initData.SysMemSlicePitch	= 0;
	}

	auto hr = dev->CreateTexture2D(&dxDesc, desc.uploadRequest? &initData : nullptr, _tex.ptrForInit());
	Util::throwIfError(hr);

//----
	::D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter				 = Util::getDxTextureFilter(desc.samplerState.filter);
	samplerDesc.AddressU			 = Util::getDxTextureWrap(desc.samplerState.wrapU);
	samplerDesc.AddressV			 = Util::getDxTextureWrap(desc.samplerState.wrapV);
	samplerDesc.AddressW			 = Util::getDxTextureWrap(desc.samplerState.wrapW);
	samplerDesc.MipLODBias			 = 0;
	samplerDesc.MaxAnisotropy		 = static_cast< decltype(D3D11_SAMPLER_DESC::MaxAnisotropy) >(desc.samplerState.maxAnisotrory);
	samplerDesc.ComparisonFunc		 = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD				 = desc.samplerState.minLOD;
	samplerDesc.MaxLOD				 = desc.samplerState.maxLOD;

	hr = dev->CreateSamplerState(&samplerDesc, _samplerState.ptrForInit());
	Util::throwIfError(hr);

//----
	::D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc = {};
	rvDesc.Format							 = dxDesc.Format;
	rvDesc.ViewDimension					 = D3D11_SRV_DIMENSION_TEXTURE2D;
	rvDesc.Texture2D.MipLevels				 = dxDesc.MipLevels ? dxDesc.MipLevels : -1;
	rvDesc.Texture2D.MostDetailedMip		 = 0;

	hr = dev->CreateShaderResourceView(_tex, &rvDesc, _resourceView.ptrForInit());
	Util::throwIfError(hr);
}


#if 0
#pragma mark ========= TextureCube_DX11 ============
#endif
TextureCube_DX11::TextureCube_DX11(CreateDesc& desc)
	: Base(desc)
{
	auto* renderer	= Renderer_DX11::instance();
	auto* dev		= renderer->d3dDevice();

	static constexpr int kFaceMaxCount = UploadRequest::kFaceMaxCount;

	::D3D11_TEXTURE2D_DESC dxDesc = {};
	dxDesc.Width				  = desc.size.x;
	dxDesc.Height				  = desc.size.y;
	dxDesc.MipLevels			  = desc.mipmapCount;
	dxDesc.ArraySize			  = kFaceMaxCount;
	dxDesc.Format				  = Util::getDxColorType(desc.colorType);
	dxDesc.SampleDesc.Count		  = 1;
	dxDesc.SampleDesc.Quality	  = 0;
	dxDesc.Usage				  = D3D11_USAGE_DEFAULT;
	dxDesc.BindFlags			  = D3D11_BIND_SHADER_RESOURCE;
	dxDesc.CPUAccessFlags		  = 0;
	dxDesc.MiscFlags			  = D3D11_RESOURCE_MISC_TEXTURECUBE;

	
	::D3D11_SUBRESOURCE_DATA initDatas[kFaceMaxCount] = {}; // Value Initialization

	if (desc.uploadRequest) {
		auto& reqs = desc.uploadRequest->reqs;
		for (int i = 0; i < reqs.size(); ++i) {
			auto& initData			= initDatas[i];
			const auto& req			= reqs[i];
			const auto& info		= req.imageInfo;
			const auto& pixelData	= req.pixelData;

			SGE_ASSERT(info.colorType != ColorType::None);
			SGE_ASSERT(info.strideInBytes >= 0);

			initData.pSysMem			= pixelData.data();
			initData.SysMemPitch		= info.strideInBytes;
			initData.SysMemSlicePitch	= 0;
		}
	}

	auto hr = dev->CreateTexture2D(&dxDesc, desc.uploadRequest ? initDatas : nullptr, _tex.ptrForInit());
	Util::throwIfError(hr);

//----
	::D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter				 = Util::getDxTextureFilter(desc.samplerState.filter);
	samplerDesc.AddressU			 = Util::getDxTextureWrap(desc.samplerState.wrapU);
	samplerDesc.AddressV			 = Util::getDxTextureWrap(desc.samplerState.wrapV);
	samplerDesc.AddressW			 = Util::getDxTextureWrap(desc.samplerState.wrapW);
	samplerDesc.MipLODBias			 = 0;
	samplerDesc.MaxAnisotropy		 = static_cast<decltype(D3D11_SAMPLER_DESC::MaxAnisotropy)>(desc.samplerState.maxAnisotrory);
	samplerDesc.ComparisonFunc		 = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD				 = desc.samplerState.minLOD;
	samplerDesc.MaxLOD				 = desc.samplerState.maxLOD;

	hr = dev->CreateSamplerState(&samplerDesc, _samplerState.ptrForInit());
	Util::throwIfError(hr);

//----
	::D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc = {};
	rvDesc.Format							 = dxDesc.Format;
	rvDesc.ViewDimension					 = D3D11_SRV_DIMENSION_TEXTURECUBE;
	rvDesc.Texture2D.MipLevels				 = dxDesc.MipLevels ? dxDesc.MipLevels : -1;
	rvDesc.Texture2D.MostDetailedMip		 = 0;

	hr = dev->CreateShaderResourceView(_tex, &rvDesc, _resourceView.ptrForInit());
	Util::throwIfError(hr);
}

} // namespace sge