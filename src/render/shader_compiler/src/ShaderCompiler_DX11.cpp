#include "ShaderCompiler_DX11.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <nlohmann/json.hpp>
#include <sge_core/base/Error.h>
#include <sge_core/file/File.h>

namespace sge {

void ShaderCompiler_DX11::compile(StrView outFilename, ShaderStage shaderStage, StrView srcFilename, StrView entryFunc) {
	HRESULT hr;
	ComPtr<DX11_ID3DBlob> errorMsg;
	ComPtr<DX11_ID3DBlob> byteCode;
	ComPtr<DX11_ID3DShaderReflection> reflect;

	auto file = UtfUtil::toStringW(srcFilename);
	SGE_LOG("file={}", file);

	MemMapFile memmap;
	memmap.open(srcFilename);
	auto sp = memmap.span();
	
	SGE_LOG("SZ=={}", sp.size());

	// compile shader
	if (shaderStage == ShaderStage::Vertex) {
		hr = D3DCompileFromFile(file.c_str(), 0, 0, entryFunc.data(), "vs_4_0", 0, 0, byteCode.ptrForInit(), errorMsg.ptrForInit());
	} else {
		hr = D3DCompileFromFile(file.c_str(), 0, 0, entryFunc.data(), "ps_4_0", 0, 0, byteCode.ptrForInit(), errorMsg.ptrForInit());
	}
	Util::throwIfError(hr);

	// reflect shader
	hr = D3DReflect(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), IID_PPV_ARGS(reflect.ptrForInit()));
	Util::throwIfError(hr);

	// just for test ===============================
	// https://zhuanlan.zhihu.com/p/425391931

	D3D11_SHADER_DESC desc;
	hr = reflect->GetDesc(&desc);
	Util::throwIfError(hr);

	SGE_LOG("COMPILE========> {}", enumStr(shaderStage));

	nlohmann::json json;
	json["CBUFFER"] = nlohmann::json::array();
	json["TEXTURE"] = nlohmann::json::array();
	json["SAMPLER"] = nlohmann::json::array();

	for (UINT i = 0; i < desc.BoundResources; ++i)
	{
		// https://www.cnblogs.com/X-Jun/p/12447486.html#_lab2_2_1
		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		hr = reflect->GetResourceBindingDesc(i, &resourceDesc);
		Util::throwIfError(hr);

		//SGE_DUMP_VAR(resourceDesc.Name, resourceDesc.Type, resourceDesc.BindCount, resourceDesc.BindPoint);

		if (resourceDesc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER) {
			nlohmann::json tmp;
			tmp["Name"] = resourceDesc.Name;
			tmp["Type"] = resourceDesc.Type;
			tmp["BindPoint"] = resourceDesc.BindPoint;
			tmp["BindCount"] = resourceDesc.BindCount;
			tmp["Variables"] = nlohmann::json::array();

			ID3D11ShaderReflectionConstantBuffer* cb = reflect->GetConstantBufferByName(resourceDesc.Name);
			D3D11_SHADER_BUFFER_DESC cbDesc;
			hr = cb->GetDesc(&cbDesc);
			Util::throwIfError(hr);

			for (UINT j = 0; j < cbDesc.Variables; ++j)
			{
				ID3D11ShaderReflectionVariable* variable = cb->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC variableDesc;
				hr = variable->GetDesc(&variableDesc);
				Util::throwIfError(hr);

				ID3D11ShaderReflectionType* reflectType = variable->GetType();
				D3D11_SHADER_TYPE_DESC typeDesc;
				hr = reflectType->GetDesc(&typeDesc);
				Util::throwIfError(hr);

				// https://www.cnblogs.com/X-Jun/p/12447486.html#_lab2_2_5
				nlohmann::json var;
				
				var["row"] = typeDesc.Rows;
				var["col"] = typeDesc.Columns;

				switch (typeDesc.Class)
				{
				case D3D_SHADER_VARIABLE_CLASS::D3D_SVC_VECTOR:         var["Type"] = "VECTOR"; break;
				case D3D_SHADER_VARIABLE_CLASS::D3D_SVC_SCALAR:         var["Type"] = "SCALAR"; break;
				case D3D_SHADER_VARIABLE_CLASS::D3D_SVC_MATRIX_ROWS:    var["Type"] = "MATRIX"; break;
				case D3D_SHADER_VARIABLE_CLASS::D3D_SVC_MATRIX_COLUMNS: var["Type"] = "MATRIX"; break;
				default:
					throw SGE_ERROR("unknown Variables Class");
				}

				switch (typeDesc.Type) {
				case D3D_SHADER_VARIABLE_TYPE::D3D_SVT_BOOL:  var["Type"]= "BOOL";	 break;
				case D3D_SHADER_VARIABLE_TYPE::D3D_SVT_INT:	  var["Type"]= "INT";	 break;
				case D3D_SHADER_VARIABLE_TYPE::D3D_SVT_UINT:  var["Type"]= "UINT";	 break;
				case D3D_SHADER_VARIABLE_TYPE::D3D_SVT_UINT8: var["Type"]= "UINT8";	 break;
				case D3D_SHADER_VARIABLE_TYPE::D3D_SVT_FLOAT: var["Type"]= "FLOAT";	 break;
				case D3D_SHADER_VARIABLE_TYPE::D3D_SVT_DOUBLE:var["Type"]= "DOUBLE"; break;
				default:
					throw SGE_ERROR("unknown Variables Type");
				}
				tmp["Variables"].push_back(var);
			}
			json["CBUFFER"].push_back(tmp);
		}
		else if (resourceDesc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED
			|| resourceDesc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE)
		{
			nlohmann::json tmp;
			tmp["Name"] = resourceDesc.Name;
			tmp["Type"] = resourceDesc.Type;
			tmp["BindPoint"] = resourceDesc.BindPoint;
			tmp["BindCount"] = resourceDesc.BindCount;
			json["TEXTURE"].push_back(tmp);
		}
		else if (resourceDesc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER)
		{
			nlohmann::json tmp;
			tmp["Name"] = resourceDesc.Name;
			tmp["Type"] = resourceDesc.Type;
			tmp["BindPoint"] = resourceDesc.BindPoint;
			tmp["BindCount"] = resourceDesc.BindCount;
			json["SAMPLER"].push_back(tmp);
		}
	}

	auto str = json.dump();
	StrView strView = str.c_str();
	auto jsonFilename = Fmt("{}.json", outFilename);
	File::writeFileIfChanged(jsonFilename, strView, true);
	SGE_LOG("===================================== END");
}

} // namespace