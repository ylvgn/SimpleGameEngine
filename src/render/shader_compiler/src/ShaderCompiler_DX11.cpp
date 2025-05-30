#include "ShaderCompiler_DX11.h"

#include <d3d11.h>
#include <d3d11_4.h>
#include <dxgi1_4.h>

#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace sge {
#if 0
#pragma mark ========= ShaderCompiler_DX11_ID3DInclude ============
#endif
class ShaderCompiler_DX11_ID3DInclude : public ID3DInclude {
	using Util = DX11Util;

	class Chunk : public NonCopyable {
	public:
		Chunk(StrView filename)
			: _filename(filename)
		{
			MemMapFile mm;
			mm.open(filename);

			_bufSize = mm.size();
			_buf = reinterpret_cast<u8*>(malloc(_bufSize * sizeof(u8)));
			if (!_buf) return;
			memcpy(_buf, mm.data(), _bufSize);
		}

		~Chunk() {
			if (_buf) {
				free(_buf);
				_buf = nullptr;
			}
			_bufSize = 0;
		}

		STDMETHOD (map) (LPCVOID*& ppData, UINT*& pBytes) const {
			if (!_buf)
				return E_OUTOFMEMORY;

			*pBytes = static_cast<UINT>(_bufSize);
			*ppData = _buf;
			return S_OK;
		}

		ByteSpan	span() const { return ByteSpan(_buf, _bufSize); }
		StrView filename() const { return _filename; }

	private:
		u8*		_buf = nullptr;
		size_t  _bufSize = 0;
		String  _filename;
	}; // Chunk

public:
	ShaderCompiler_DX11_ID3DInclude(StrView srcFilename, Vector<String>& include_dirs) noexcept
		: _filename(srcFilename)
		, _include_dirs(include_dirs)
	{}

	~ShaderCompiler_DX11_ID3DInclude() {
		_chunks.clear();
	}

	/*HRESULT*/
	STDMETHOD(Open) (THIS_ D3D_INCLUDE_TYPE IncludeType,
					 LPCSTR					pFileName,
					 LPCVOID				pParentData,
					 LPCVOID*				ppData,
					 UINT*					pBytes) override
	{
		TempString tmpName;
		StrView filename(pFileName);
		StrView dirname;

		if (!pParentData) {
			dirname = FilePath::dirname(_filename);
		} else {
			auto* chunk = find(pParentData);
			if (!chunk) {
				return HRESULT_FROM_WIN32(ERROR_INVALID_ACCESS);
			}
			dirname = FilePath::dirname(chunk->filename());
		}

		switch (IncludeType) {
			case D3D_INCLUDE_LOCAL: {
				FilePath::combineTo(tmpName, dirname, filename);
			} break;
			case D3D_INCLUDE_SYSTEM: {
				for (auto& d : _include_dirs) {
					FilePath::combineTo(tmpName, d, filename);
					if (File::exists(tmpName)) {
						break;
					}
//					SGE_DUMP_VAR(FilePath::realpath(tmpName));
					tmpName.clear();
				}
				if (tmpName.empty()) {
					return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
				}
			} break;
			default: return E_NOTIMPL;
		}

		if (_filename == tmpName) {
			SGE_LOG("cycle include file error: {}", filename);
			return E_INVALIDARG;
		}

		auto it = _chunks.find(tmpName.c_str());
		if (it != _chunks.end()) {
			auto& chunk = it->second;
			for (auto& c : _stack) {
				if (c->filename() == tmpName) {
					SGE_LOG("cycle include file error: {}", filename);
					return E_INVALIDARG;
				}
			}
			_stack.emplace_back(chunk.get());
			return chunk->map(ppData, pBytes);
		}

		auto newChunk = UPtr_make<Chunk>(tmpName);
		_chunks[tmpName.c_str()] = eastl::move(newChunk);

		auto& chunk = _chunks[tmpName.c_str()];
		_stack.emplace_back(chunk.get());
		return chunk->map(ppData, pBytes);
	}

	/*HRESULT*/
	STDMETHOD(Close)(THIS_ LPCVOID pData) override {
		if (!_stack.empty()) {
			auto& chunk = _stack.back();
			auto span = chunk->span();
			if (span.data() != pData)
				return E_FAIL;
			_stack.pop_back();
		}
		return S_OK;
	}

	void writeDepFile(StrView outBinFilename) {
		TempString outDepFilename = Fmt("{}.dep", outBinFilename);

		String o;
		o.append(outBinFilename);
		o.append(":\\");

		for (const auto& chunk : _chunks) {
			o.append("\n\t");
			o.append(chunk.first);
			o.append("\\");
		}
		o.append("\n#-----\n");

		File::writeFileIfChanged(outDepFilename, o, false);
	}

	const Chunk* find(LPCVOID pData) const {
		for (auto* chunk : _stack) {
			auto span = chunk->span();
			if (span.data() == pData)
				return chunk;
		}
		return nullptr;
	}

private:
	StringMap< UPtr<Chunk> > _chunks;
	Vector<const Chunk*, 64> _stack;
	Vector<String>&			 _include_dirs;
	String					 _filename;
}; // ShaderCompiler_DX11_ID3DInclude


#if 0
#pragma mark ========= ShaderCompiler_DX11 ============
#endif
void ShaderCompiler_DX11::compile(StrView outFilename, ShaderStageMask shaderStage, StrView profile, StrView srcFilename, StrView entryFunc, Vector<String>& include_dirs) {
	if (profile.empty()) {
		profile = Util::getDxStageProfile(shaderStage);
	}

	auto outPath = FilePath::dirname(outFilename);
	Directory::create(outPath);

	TempStringA entryPoint = entryFunc;

	MemMapFile memmap;
	memmap.open(srcFilename);

	auto hlsl = memmap.span();

	UINT flags1 = 0; // D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
	UINT flags2 = 0;

#if _DEBUG
	flags1 |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<DX11_ID3DBlob>	bytecode;
	ComPtr<DX11_ID3DBlob>	errorMsg;

	// TODO not use now
	D3D_SHADER_MACRO macros[] = {
#if SGE_OS_WINDOWS
		"SGE_OS_WINDOWS", "1",
#elif SGE_OS_MACOSX
		"SGE_OS_MACOSX", "1",
#endif
		NULL, NULL
	};

	ShaderCompiler_DX11_ID3DInclude include(srcFilename, include_dirs);

	TempString profileA = profile;
	auto hr = ::D3DCompile2(hlsl.data(), hlsl.size(), memmap.filename().c_str(),
							macros, &include,
							entryPoint.c_str(),
							profileA.c_str(),
							flags1, flags2, 0, nullptr, 0,
							bytecode.ptrForInit(),
							errorMsg.ptrForInit());

	if (FAILED(hr)) {
		throw SGE_ERROR("HRESULT={}\n Error Message: {}", hr, Util::toStrView(errorMsg));
	}

	include.writeDepFile(outFilename);

	auto bytecodeSpan = Util::toSpan(bytecode);
	File::writeFileIfChanged(outFilename, bytecodeSpan, false);
	_reflect(outFilename, bytecodeSpan, shaderStage, profile);
}

void ShaderCompiler_DX11::_reflect(StrView outFilename, ByteSpan bytecode, ShaderStageMask stage, StrView profile) {
	ComPtr<DX11_ID3DShaderReflection> reflect;
	auto hr = ::D3DReflect(bytecode.data(), bytecode.size(), IID_PPV_ARGS(reflect.ptrForInit()));
	Util::throwIfError(hr);

	::D3D11_SHADER_DESC desc;
	hr = reflect->GetDesc(&desc);
	Util::throwIfError(hr);

	ShaderStageInfo outInfo;
	outInfo.profile = profile;
	outInfo.stage   = stage;

	{
		_reflect_inputs			(outInfo, reflect, desc);
		_reflect_constBuffers	(outInfo, reflect, desc);
		_reflect_textures		(outInfo, reflect, desc);
		_reflect_samplers		(outInfo, reflect, desc);
		_reflect_storageBuffers (outInfo, reflect, desc);
	}

	{
		auto jsonFilename = Fmt("{}.json", outFilename);
		JsonUtil::writeFileIfChanged(jsonFilename, outInfo, false);
	}
}

void ShaderCompiler_DX11::_reflect_inputs(ShaderStageInfo& outInfo, DX11_ID3DShaderReflection* reflect, D3D11_SHADER_DESC& desc) {
	::HRESULT hr;
	outInfo.inputs.reserve(desc.InputParameters);

	for (UINT i = 0; i < desc.InputParameters; ++i) {
		auto& dst = outInfo.inputs.emplace_back();

		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		hr = reflect->GetInputParameterDesc(i, &paramDesc);
		Util::throwIfError(hr);

		VertexSemanticType semanticType;
		semanticType = Util::parseDxSemanticName(StrView_c_str(paramDesc.SemanticName));

		dst.semantic = VertexSemanticUtil::make(semanticType, static_cast<VertexSemanticIndex>(paramDesc.SemanticIndex));

		TempString  semantic = enumStr(dst.semantic);
		if (!semantic.size()) {
			throw SGE_ERROR("unsupported sematic name {}", paramDesc.SemanticName);
		}

		TempString dataType;

		switch (paramDesc.ComponentType) {
			case D3D_REGISTER_COMPONENT_UINT32:		dataType.append("UInt8");	break;
			case D3D_REGISTER_COMPONENT_SINT32:		dataType.append("Int32");	break;
			case D3D_REGISTER_COMPONENT_FLOAT32:	dataType.append("Float32");	break;
			default: throw SGE_ERROR("unsupported component type {}", static_cast<int>(paramDesc.ComponentType));
		}

		auto componentCount = BitUtil::count1(paramDesc.Mask);
		if (componentCount < 1 || componentCount > 4) {
			throw SGE_ERROR("invalid componentCount {}", componentCount);
		}

		if (componentCount > 1) {
			FmtTo(dataType, "x{}", componentCount);
		}

		if (!enumTryParse(dst.dataType, dataType)) {
			throw SGE_ERROR("cannot parse dataType enum {}", dataType);
		}
	}
}

void ShaderCompiler_DX11::_reflect_constBuffers(ShaderStageInfo& outInfo, DX11_ID3DShaderReflection* reflect, D3D11_SHADER_DESC& desc) {
	::HRESULT hr;
	outInfo.constBuffers.reserve(desc.BoundResources);

	for (UINT i = 0; i < desc.BoundResources; ++i) {
		D3D11_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		Util::throwIfError(hr);

		if (resDesc.Type != D3D_SIT_CBUFFER) continue;

		auto& outCB = outInfo.constBuffers.emplace_back();

		D3D11_SHADER_BUFFER_DESC bufDesc;
		auto cb = reflect->GetConstantBufferByName(resDesc.Name);
		hr = cb->GetDesc(&bufDesc);
		Util::throwIfError(hr);

		outCB.name		= bufDesc.Name;
		outCB.bindPoint	= static_cast<i16>(resDesc.BindPoint);
		outCB.bindCount	= static_cast<i16>(resDesc.BindCount);
		outCB.dataSize	= bufDesc.Size;

		{ // variables
			outCB.variables.reserve(bufDesc.Variables);
			for (UINT j = 0; j < bufDesc.Variables; j++) {
				auto cbv = cb->GetVariableByIndex(j);
				::D3D11_SHADER_VARIABLE_DESC varDesc;
				hr = cbv->GetDesc(&varDesc);
				Util::throwIfError(hr);

				::D3D11_SHADER_TYPE_DESC varType;
				hr = cbv->GetType()->GetDesc(&varType);
				Util::throwIfError(hr);

				if (0 == (varDesc.uFlags & D3D_SVF_USED))
					continue;

				auto& outVar			= outCB.variables.emplace_back();

				using SGE_Offset		= decltype(outVar.offset);
				using SGE_ElementCount  = decltype(outVar.elementCount);
				using SGE_DataSize		= decltype(outVar.dataSize);

				const auto& varDataSize		= varDesc.Size;
				const auto& varElementCount = varType.Elements;

				outVar.name		= varDesc.Name;
				outVar.offset	= static_cast<SGE_Offset>(varDesc.StartOffset);
				outVar.dataSize	= static_cast<SGE_DataSize>(varDataSize);

				if (varType.Offset > 0) {
					// TODO: custom structure
				} else {
					outVar.elementCount = static_cast<SGE_ElementCount>(varElementCount);
				}

				TempString dataType;
				switch (varType.Type) {
					case D3D_SVT_BOOL:	 dataType.append("Bool");		break;
					case D3D_SVT_INT:	 dataType.append("Int32");		break;
					case D3D_SVT_UINT:	 dataType.append("UInt32");		break;
					case D3D_SVT_UINT8:	 dataType.append("UInt8");		break;
					case D3D_SVT_FLOAT:  dataType.append("Float32");	break;
					case D3D_SVT_DOUBLE: dataType.append("Float64");	break;
				//---
					default:
						throw SGE_ERROR("unsupported type {}", static_cast<int>(varType.Type));
				}

				switch (varType.Class) {
					case D3D_SVC_SCALAR: break; // e.g. ${dataType}
					case D3D_SVC_VECTOR:
						FmtTo(dataType, "x{}",	varType.Columns); // e.g. ${dataType}x4
						break;
					case D3D_SVC_MATRIX_COLUMNS:
						FmtTo(dataType, "_{}x{}", varType.Rows, varType.Columns); // e.g. ${dataType}_4x4
						outVar.rowMajor = false;
						break;
					case D3D_SVC_MATRIX_ROWS:
						FmtTo(dataType, "_{}x{}", varType.Rows, varType.Columns); // e.g. ${dataType}_4x4
						outVar.rowMajor = true;
						break;
				//---
					default:
						throw SGE_ERROR("unsupported Class {}", static_cast<int>(varType.Class));
				}

				if (varType.Elements > 0) {
					dataType.append("Array");
				}

				if (!enumTryParse(outVar.dataType, dataType)) {
					throw SGE_ERROR("cannot parse dataType {}", dataType);
				}

				if (outVar.dataType == DataType::None) {
					throw SGE_ERROR("dataType is None");
				}
			}
		}
	}

}

void ShaderCompiler_DX11::_reflect_textures(ShaderStageInfo& outInfo, DX11_ID3DShaderReflection* reflect, D3D11_SHADER_DESC& desc) {
	::HRESULT hr;
	outInfo.textures.reserve(desc.BoundResources);

	for (UINT i = 0; i < desc.BoundResources; ++i) {
		D3D11_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		Util::throwIfError(hr);

		if (resDesc.Type != D3D_SIT_TEXTURE) continue;

		auto& outTex = outInfo.textures.emplace_back();
		outTex.name			= resDesc.Name;
		outTex.bindPoint	= static_cast<i16>(resDesc.BindPoint);
		outTex.bindCount	= static_cast<i16>(resDesc.BindCount);

		switch (resDesc.Dimension) {
			case D3D_SRV_DIMENSION_TEXTURE1D:		outTex.dataType = DataType::Texture1D;   break;
			case D3D_SRV_DIMENSION_TEXTURE2D:		outTex.dataType = DataType::Texture2D;   break;
			case D3D_SRV_DIMENSION_TEXTURE3D:		outTex.dataType = DataType::Texture3D;   break;
			case D3D_SRV_DIMENSION_TEXTURECUBE:		outTex.dataType = DataType::TextureCube; break;
		//----
			case D3D_SRV_DIMENSION_TEXTURE1DARRAY:	outTex.dataType = DataType::Texture1DArray;   break;
			case D3D_SRV_DIMENSION_TEXTURE2DARRAY:	outTex.dataType = DataType::Texture2DArray;   break;
			case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:outTex.dataType = DataType::TextureCubeArray; break;
		//----
			default:
				throw SGE_ERROR("unsupported texture dimension {}", static_cast<int>(resDesc.Dimension));
		}
	}
}

void ShaderCompiler_DX11::_reflect_samplers(ShaderStageInfo& outInfo, DX11_ID3DShaderReflection* reflect, D3D11_SHADER_DESC& desc) {
	::HRESULT hr;
	outInfo.samplers.reserve(desc.BoundResources);

	for (UINT i = 0; i < desc.BoundResources; ++i) {
		::D3D11_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		Util::throwIfError(hr);

		if (resDesc.Type != D3D_SIT_SAMPLER) continue;

		auto& outSampler = outInfo.samplers.emplace_back();
		outSampler.name			= resDesc.Name;
		outSampler.bindPoint	= static_cast<i16>(resDesc.BindPoint);
		outSampler.bindCount	= static_cast<i16>(resDesc.BindCount);
	}
}

void ShaderCompiler_DX11::_reflect_storageBuffers(ShaderStageInfo& outInfo, DX11_ID3DShaderReflection* reflect, D3D11_SHADER_DESC& desc) {
	::HRESULT hr;
	outInfo.storageBuffers.reserve(desc.BoundResources);

	for (UINT i = 0; i < desc.BoundResources; ++i) {
		::D3D11_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		Util::throwIfError(hr);

		if (resDesc.Type != D3D_SIT_STRUCTURED			 // struct
			&& resDesc.Type != D3D_SIT_UAV_RWSTRUCTURED  // RWStructuredBuffer<T>
			&& resDesc.Type != D3D_SIT_UAV_RWBYTEADDRESS // RWByteAddressBuffer
		)
		continue;

		auto& sbuf = outInfo.storageBuffers.emplace_back();
		sbuf.name = resDesc.Name;
		sbuf.bindPoint = static_cast<i16>(resDesc.BindPoint);
		sbuf.bindCount = static_cast<i16>(resDesc.BindCount);
		sbuf.rawUAV	   = (resDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS);
	}
}

} // namespace sge