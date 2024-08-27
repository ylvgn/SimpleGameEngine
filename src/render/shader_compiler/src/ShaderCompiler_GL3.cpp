#include "ShaderCompiler_GL3.h"

#include <fmt/xchar.h>

namespace sge {

void ShaderCompiler_GL3::compile(StrView outPath, ShaderStageMask shaderStage, StrView srcFilename, StrView entryFunc) {
	auto profile = Util::getGlStageProfile(shaderStage);

	TempString	spirvOutFilename;
	TempStringW tmpShaderStage;
	switch (shaderStage) {
		case ShaderStageMask::Vertex:
			spirvOutFilename	= Fmt("{}/vs_{}.spv", outPath, profile);
			tmpShaderStage		= L"vertex";
			break;
		case ShaderStageMask::Pixel:
			spirvOutFilename	= Fmt("{}/ps_{}.spv", outPath, profile);
			tmpShaderStage		= L"fragment";
			break;
		default: throw SGE_ERROR("");
	}

	TempString outFilename = Fmt("{}.glsl", spirvOutFilename);

	Directory::create(outPath);

	{ // HLSL -> SPIRV
#if SGE_OS_WINDOWS
		TempStringW tmpEntryPoint;
		UtfUtil::convert(tmpEntryPoint, entryFunc);

		TempStringW tmpOutput;
		UtfUtil::convert(tmpOutput, spirvOutFilename);

		TempStringW tmpSrcFilename;
		UtfUtil::convert(tmpSrcFilename, srcFilename);

#if 0
		TempStringW tmpCmdParams;
		fmt::format_to(std::back_inserter(tmpCmdParams),
			L"-fshader-stage={} -fentry-point={} -o {} -x hlsl {}"
			, tmpShaderStage.c_str()
			, tmpEntryPoint.c_str()
			, tmpOutput.c_str()
			, tmpSrcFilename.c_str());

		wprintf(L"HLSL->SPIRV : glslc.exe %ls\n\n", tmpCmdParams.c_str());

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
#else
		TempStringW tmpCmdParams;
		tmpCmdParams.append(tmpShaderStage.c_str());	tmpCmdParams.append(L" ");
		tmpCmdParams.append(tmpEntryPoint.c_str());		tmpCmdParams.append(L" ");
		tmpCmdParams.append(tmpOutput.c_str());			tmpCmdParams.append(L" ");
		tmpCmdParams.append(tmpSrcFilename.c_str());

		wprintf(L"HLSL->SPIRV : sge_glslc.bat %ls\n\n", tmpCmdParams.c_str());

		SHELLEXECUTEINFO ShExecInfo = {};
		ShExecInfo.cbSize			= sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask			= SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd				= NULL;
		ShExecInfo.lpVerb			= L"open";
		ShExecInfo.lpFile			= L"sge_glslc.bat";
		ShExecInfo.lpParameters		= tmpCmdParams.c_str();
		ShExecInfo.lpDirectory		= NULL;
		ShExecInfo.nShow			= SW_HIDE; // SW_SHOW
		ShExecInfo.hInstApp			= NULL;
		ShellExecuteEx(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
		CloseHandle(ShExecInfo.hProcess);
#endif

#endif
	}

	{ // SPIRV -> GLSL source text
		MemMapFile mm;
		mm.open(spirvOutFilename);

		Span<const u32> bytecode = spanCast<const u32>(mm.span());
		Compiler comp(bytecode.data(), bytecode.size());

		CompilerOptions options;
		options.es = false;
		options.enable_420pack_extension = false;

		if (!StringUtil::tryParse(profile, options.version)) {
			throw SGE_ERROR("_reflect tryParse error");
		}
		comp.build_combined_image_samplers();
		comp.set_common_options(options);

		std::string GLSLSource = comp.compile();
		StrView source(GLSLSource.c_str(), GLSLSource.size());
		File::writeFileIfChanged(outFilename, source, false);

		_reflect(outFilename, comp, shaderStage, profile);
	}
}

void ShaderCompiler_GL3::_convert(Compiler& comp, DataType& o, const SPIRType& i, u32 memberIndex /*= 0*/) {	
	const auto& type	= i.basetype;
	const auto& vecsize = i.vecsize;
	const auto& columns = i.columns;
	const auto& image	= i.image;

	using SRC = SPIRType;
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
		// size_t array_stride = comp.type_struct_member_array_stride(i, memberIndex);
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
		if (vecsize == 1 && columns == 1) { // scalar
			// do nothing
		} else if (vecsize > 1 && columns == 1) {
			FmtTo(dataType, "x{}", vecsize); // vector
		} else if (columns > 1) {
			FmtTo(dataType, "_{}x{}", columns, vecsize); // matrix
		}
	}

	if (!enumTryParse(o, dataType))
		throw SGE_ERROR("cannot parse dataType {}", dataType);

	SGE_ASSERT(o != DataType::None);
}

void ShaderCompiler_GL3::_reflect(StrView outFilename, Compiler& comp, ShaderStageMask shaderStage, StrView profile) {
	ShaderStageInfo outInfo;
	outInfo.profile = profile;
	outInfo.stage	= shaderStage;

	{
		auto active = comp.get_active_interface_variables();
		ShaderResources resources = comp.get_shader_resources(active);
		comp.set_enabled_interface_variables(move(active));

		_reflect_inputs			(outInfo, comp, resources);
		_reflect_constBuffers	(outInfo, comp, resources);
		_reflect_textures		(outInfo, comp, resources);
		_reflect_samplers		(outInfo, comp, resources);
	}

	{
		auto jsonFilename = Fmt("{}.json", outFilename);
		JsonUtil::writeFileIfChanged(jsonFilename, outInfo, false);
	}
}

void ShaderCompiler_GL3::_reflect_inputs(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources) {
	outInfo.inputs.reserve(resources.stage_inputs.size());

	for (auto& resource : resources.stage_inputs) {
		auto& outInput = outInfo.inputs.emplace_back();

		auto resId = resource.id;
		StrView name(resource.name.c_str(), resource.name.size());

//		you should not care about Name here unless your backend assigns bindings based on name, or for debugging purposes
//		outInput.name.assign(resource.name.c_str(), resource.name.size());

		// No semantic in OpenGL, just save input slot index
		outInput.semantic = GL3Util::parseGlSemanticName(name);

		const SPIRType& type	= comp.get_type(resource.base_type_id);
		auto componentCount		= type.vecsize;

		if (componentCount < 1 || componentCount > 4) {
			throw SGE_ERROR("invalid componentCount {}", componentCount);
		}

		_convert(comp, outInput.dataType, type);
#if 0
		printf("Input '%s':\tlayout set = %u\tlayout binding = %u\tlayout location= %u\n",
			resource.name.c_str(),
			comp.get_decoration(resId, spv::DecorationDescriptorSet),
			comp.get_decoration(resId, spv::DecorationBinding),
			comp.get_decoration(resId, spv::DecorationLocation));
#endif
	}
}

void ShaderCompiler_GL3::_reflect_constBuffers(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources) {
	outInfo.constBuffers.reserve(resources.uniform_buffers.size());

	for (auto& resource : resources.uniform_buffers) {
		auto& outCB = outInfo.constBuffers.emplace_back();

		auto resId = resource.id;

		using SGE_BindPoint = decltype(outCB.bindPoint);
		using SGE_BindCount = decltype(outCB.bindCount);
		using SGE_DataSize	= decltype(outCB.dataSize);

		auto&	type = comp.get_type(resource.base_type_id);
		size_t	memberCount = type.member_types.size();

		outCB.bindPoint = static_cast<SGE_BindPoint>(comp.get_decoration(resId, spv::DecorationBinding));
		outCB.name.assign(resource.name.c_str(), resource.name.size());
		outCB.dataSize = comp.get_declared_struct_size(type);

		for (int i = 0; i < memberCount; ++i) {
			auto& outVar = outCB.variables.emplace_back();

			using SGE_Offset = decltype(outVar.offset);

			auto& memberType	= comp.get_type(type.member_types[i]);
			const auto& name	= comp.get_member_name(type.self, i);
			size_t startOffset	= comp.type_struct_member_offset(type, i);
//			size_t memberSize	= comp.get_declared_struct_member_size(type, i);

			outVar.name.assign(name.data(), name.size());
			outVar.offset = startOffset;

			if (comp.get_member_decoration(resId, i, spv::Decoration::DecorationRowMajor)) {
				outVar.rowMajor = true;
			}

			_convert(comp, outVar.dataType, memberType, i);
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

void ShaderCompiler_GL3::_reflect_textures(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources) {
	// TODO
}

void ShaderCompiler_GL3::_reflect_samplers(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources) {
	// TODO
}

}