#include "ShaderCompiler_GL3.h"

#include <fmt/xchar.h>

#include "spirv_cross.hpp"
#include "spirv_glsl.hpp"

namespace sge {
#if 0
#pragma mark ========= ShaderCompiler_GL3_Helper ============
#endif
struct ShaderCompiler_GL3_Helper {
	ShaderCompiler_GL3_Helper() = delete;

	using Compiler			= spirv_cross::CompilerGLSL;
	using CompilerOptions	= spirv_cross::CompilerGLSL::Options;
	using ShaderResources	= spirv_cross::ShaderResources;
	using SPIRType			= spirv_cross::SPIRType;

	using DataType			= RenderDataType;

	static void convert(Compiler& comp, DataType& o, const SPIRType& i, u32 index = 0);

	static void reflect(StrView outFilename, Compiler& comp, ShaderStageMask shaderStage, StrView profile);
private:
	static void _reflect_inputs(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& reflect);
	static void _reflect_constBuffers(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& reflect);
	static void _reflect_textures(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& reflect);
	static void _reflect_samplers(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& reflect);
};

void ShaderCompiler_GL3_Helper::convert(Compiler& comp, DataType& o, const SPIRType& i, u32 index /*= 0*/) {
	// For vectors and matrices, look at SPIRType::vecsize and SPIRType::columns. 1 column means it's a vector.
	using SRC = SPIRType;
	
	const auto& type = i.basetype;
	const auto& vecsize = i.vecsize;
	const auto& columns = i.columns;

	const auto& image = i.image;

	TempString dataType;
	switch (type) {
		case SRC::Boolean:	dataType.append("Bool");	break;
		case SRC::SByte:	dataType.append("Int8");	break;
		case SRC::UByte:	dataType.append("UInt8");	break;
		case SRC::Short:	dataType.append("Int16");	break;
		case SRC::UShort:	dataType.append("UInt16");	break;
		case SRC::Int:		dataType.append("Int32");	break;
		case SRC::UInt:		dataType.append("UInt32");	break;
		case SRC::Int64:	dataType.append("Int64");	break;
		case SRC::UInt64:	dataType.append("UInt64");	break;
		case SRC::Half:		dataType.append("Float32");	break;
		case SRC::Float:	dataType.append("Float32");	break;
		case SRC::Double:	dataType.append("Float64");	break;

//		TODO
//		case SRC::Image:
//		case SRC::SampledImage: dataType.append("Texture"); break;
//		case SRC::AtomicCounter: break;
//		case SRC::Struct:					break;
//		case SRC::Sampler:					break;
//		case SRC::AccelerationStructure:	break;
//		case SRC::RayQuery:					break;
		default:
			throw SGE_ERROR("unsupported SPIRType: {}", static_cast<int>(type));
	}

	if (!i.array.empty()) {
		// Get array stride, e.g. float4 foo[]; Will have array stride of 16 bytes.
		//size_t array_stride = comp.type_struct_member_array_stride(i, index);
		throw SGE_ERROR("unsupported SPIRType array: {}", static_cast<int>(type));
	}

	if (image.type != 0) {
		using Dim				= spv::Dim;
		using ImageFormat		= spv::ImageFormat;
		using AccessQualifier	= spv::AccessQualifier;

		switch (image.dim) {
			case Dim::Dim1D: dataType.append("1D"); break;
			case Dim::Dim2D: dataType.append("2D"); break;
			case Dim::Dim3D: dataType.append("3D"); break;
			case Dim::DimCube: dataType.append("Cube"); break;
		}
		if (image.arrayed) dataType.append("Array");

		// !!<-------------- TODO
		if (image.ms) throw SGE_ERROR("unsupported feature image.ms");
		if (image.sampled) throw SGE_ERROR("unsupported image feature image.sampled={}", image.sampled);
		switch (image.format) {
			case ImageFormat::ImageFormatRgba32f: SGE_DUMP_VAR("TODO"); break;
			default:
				throw SGE_ERROR("unsupported spv ImageFormat: {}", static_cast<int>(image.format));
		};
	}
	else {
		if (vecsize == 1 && columns == 1) { // Scalar
			// do nothing
		} else if (vecsize > 1 && columns == 1) { // Vector
			FmtTo(dataType, "x{}", vecsize);
		} else if (columns > 1) { // Matrix
			FmtTo(dataType, "_{}x{}", columns, vecsize);
		}
	}

	if (!enumTryParse(o, dataType))
		throw SGE_ERROR("cannot parse dataType {}", dataType);

	SGE_ASSERT(o != DataType::None);
}

void ShaderCompiler_GL3_Helper::reflect(StrView outFilename, Compiler& comp, ShaderStageMask shaderStage, StrView profile) {
	ShaderStageInfo outInfo;
	outInfo.profile = profile;
	outInfo.stage = shaderStage;

	{
		// Querying statically accessed resources
		auto active = comp.get_active_interface_variables();
		ShaderResources resources = comp.get_shader_resources(active);
		comp.set_enabled_interface_variables(move(active));

		_reflect_inputs(outInfo, comp, resources);
		_reflect_constBuffers(outInfo, comp, resources);
		_reflect_textures(outInfo, comp, resources);
		_reflect_samplers(outInfo, comp, resources);
	}

	{
		auto jsonFilename = Fmt("{}.json", outFilename);
		JsonUtil::writeFileIfChanged(jsonFilename, outInfo, false);
	}
}

void ShaderCompiler_GL3_Helper::_reflect_inputs(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& reflect) {
	outInfo.inputs.reserve(reflect.stage_inputs.size());

	for (auto& resource : reflect.stage_inputs) {
		auto& outInput = outInfo.inputs.emplace_back();

		using InputSlotType = decltype(outInput.inputSlot);

		// you should not care about Name here unless your backend assigns bindings based on name, or for debugging purposes
		// outInput.name.assign(resource.name.c_str(), resource.name.size());

		// No semantic in OpenGL, just save input slot index
		// outInput.semantic = VertexSemantic::None;
		outInput.inputSlot = static_cast<InputSlotType>(comp.get_decoration(resource.id, spv::DecorationLocation));

		const SPIRType& type	= comp.get_type(resource.base_type_id);
		auto componentCount		= type.vecsize;

		if (componentCount < 1 || componentCount > 4) {
			throw SGE_ERROR("invalid componentCount {}", componentCount);
		}

		convert(comp, outInput.dataType, type);
#if 0
		printf("Input '%s':\tlayout set = %u\tlayout binding = %u\tlayout location= %u\n",
			resource.name.c_str(),
			comp.get_decoration(resource.id, spv::DecorationDescriptorSet),
			comp.get_decoration(resource.id, spv::DecorationBinding),
			comp.get_decoration(resource.id, spv::DecorationLocation));
#endif
	}
}

void ShaderCompiler_GL3_Helper::_reflect_constBuffers(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& reflect) {
	outInfo.constBuffers.reserve(reflect.uniform_buffers.size());

	for (auto& resource : reflect.uniform_buffers) {
		auto& outCB = outInfo.constBuffers.emplace_back();

		using SGE_BindPoint = decltype(outCB.bindPoint);
		using SGE_BindCount = decltype(outCB.bindCount);
		using SGE_DataSize	= decltype(outCB.dataSize);

		auto& type = comp.get_type(resource.base_type_id);
		size_t memberCount = type.member_types.size();

		outCB.bindPoint = static_cast<SGE_BindPoint>(comp.get_decoration(resource.id, spv::DecorationBinding));
		outCB.name.assign(resource.name.c_str(), resource.name.size());
		outCB.dataSize = comp.get_declared_struct_size(type);

		for (int i = 0; i < memberCount; ++i) {
			auto& outVar = outCB.variables.emplace_back();

			using SGE_Offset = decltype(outVar.offset);

			auto& memberType	= comp.get_type(type.member_types[i]);
			const auto& name	= comp.get_member_name(type.self, i);
//			size_t memberSize	= comp.get_declared_struct_member_size(type, i);
			size_t startOffset	= comp.type_struct_member_offset(type, i);

			outVar.name.assign(name.data(), name.size());
			outVar.offset = startOffset;

			convert(comp, outVar.dataType, memberType, i);
#if 0
			printf("name=%s\tvecsize=%u\tcolumns=%u\tmemberSize=%zu\tstartOffset=%zu\n",
				name.c_str(),
				type.vecsize,
				type.columns,
				memberSize,
				startOffset);
#endif
		}
	}
}

void ShaderCompiler_GL3_Helper::_reflect_textures(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& reflect) {
	// TODO
}

void ShaderCompiler_GL3_Helper::_reflect_samplers(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& reflect) {
	// TODO
}

#if 0
#pragma mark ========= ShaderCompiler_GL3 ============
#endif
void ShaderCompiler_GL3::compile(StrView outPath, ShaderStageMask shaderStage, StrView srcFilename, StrView entryFunc) {
	auto profile = Util::getGlStageProfile(shaderStage);

//	TempString	outFilename;
	TempString	spirvOutFilename;
	TempStringW tmpShaderStage;
	switch (shaderStage) {
		case ShaderStageMask::Vertex:
//			outFilename			= Fmt("{}/vs_{}.bin", outPath, profile);
			spirvOutFilename	= Fmt("{}/vs_{}.spv", outPath, profile);
			tmpShaderStage		= L"vertex";
			break;
		case ShaderStageMask::Pixel:
//			outFilename			= Fmt("{}/ps_{}.bin", outPath, profile);
			spirvOutFilename	= Fmt("{}/ps_{}.spv", outPath, profile);
			tmpShaderStage		= L"fragment";
			break;
		default: throw SGE_ERROR("");
	}

	TempString glslOutFilename = Fmt("{}.glsl", spirvOutFilename);

	Directory::create(outPath);

	{ // HLSL -> SPIRV
#if SGE_OS_WINDOWS
		TempStringW tmpEntryPoint;
		UtfUtil::convert(tmpEntryPoint, entryFunc);

		TempStringW tmpOutput;
		UtfUtil::convert(tmpOutput, spirvOutFilename);

		TempStringW tmpSrcFilename;
		UtfUtil::convert(tmpSrcFilename, srcFilename);

		TempStringW tmpCmdParams;
		fmt::format_to(std::back_inserter(tmpCmdParams),
			L"-fshader-stage={} -fentry-point={} -o {} -x hlsl {}"
			, tmpShaderStage.c_str()
			, tmpEntryPoint.c_str()
			, tmpOutput.c_str()
			, tmpSrcFilename.c_str());

//		printf("HLSL->SPIRV : glslc.exe %ws\n\n", tmpCmdParams.c_str());

		SHELLEXECUTEINFO ShExecInfo = {};
		ShExecInfo.cbSize			= sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask			= SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd				= NULL;
		ShExecInfo.lpVerb			= L"open";
		ShExecInfo.lpFile			= L"glslc.exe";
		ShExecInfo.lpParameters		= tmpCmdParams.c_str();
		ShExecInfo.lpDirectory		= NULL;
		ShExecInfo.nShow			= SW_HIDE; // SW_SHOW
		ShExecInfo.hInstApp			= NULL;
		ShellExecuteEx(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
		CloseHandle(ShExecInfo.hProcess);
#endif
	}

	{ // SPIRV -> GLSL source text
		using Compiler			= spirv_cross::CompilerGLSL;
		using CompilerOptions	= spirv_cross::CompilerGLSL::Options;
		using ShaderResources	= spirv_cross::ShaderResources;
		using Helper			= ShaderCompiler_GL3_Helper;

		MemMapFile mm;
		mm.open(spirvOutFilename);

		Span<const u32> bytecode = spanCast<const u32>(mm.span());
		Compiler glsl(bytecode.data(), bytecode.size());

		CompilerOptions options;
		options.es = false;
		options.enable_420pack_extension = false;

		if (!StringUtil::tryParse(profile, options.version)) {
			throw SGE_ERROR("_reflect tryParse error");
		}
		glsl.build_combined_image_samplers();
		glsl.set_common_options(options);

		std::string source = glsl.compile();
		StrView glslSource(source.c_str(), source.size());
		File::writeFileIfChanged(glslOutFilename, glslSource, false);

		// reflection
		Helper::reflect(glslOutFilename, glsl, shaderStage, profile);
	}
}

}