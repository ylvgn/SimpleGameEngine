#include "ShaderCompiler_GL.h"

#include <fmt/xchar.h>

namespace sge {

void ShaderCompiler_GL::compile(StrView outPath, ShaderStageMask shaderStage, StrView srcFilename, StrView entryFunc) {
#if 0
	TempString profile;
	// TODO
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	FmtTo(profile, "{}{}0", major, minor);
#else
	TempString profile = "460";
#endif

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
		using Param = CommandLine::Param;
		Vector<Param, 4> params;
		params.emplace_back(Param(tmpShaderStage));
		params.emplace_back(Param(tmpEntryPoint));
		params.emplace_back(Param(tmpOutput));
		params.emplace_back(Param(tmpSrcFilename));
		CommandLine::runShell("sge_glslc.bat", params);
#endif

#endif
	}

	{ // SPIRV -> GLSL source text
		MemMapFile mm;
		mm.open(spirvOutFilename);

		Span<const u32> bytecode = spanCast<const u32>(mm.span());
		Compiler comp(bytecode.data(), bytecode.size());

		_beforeGLSLCompile(comp, shaderStage, profile);

		CompilerOptions options;
		options.es = false;
		options.enable_420pack_extension = false;

		if (!StringUtil::tryParse(profile, options.version)) {
			throw SGE_ERROR("tryParse error");
		}
		comp.build_combined_image_samplers();
		comp.set_common_options(options);

		std::string GLSLSource = comp.compile();
		StrView source(GLSLSource.c_str(), GLSLSource.size());
		File::writeFileIfChanged(outFilename, source, false);

		_reflect(outFilename, comp, shaderStage, profile);
	}
}

void ShaderCompiler_GL::_convert(Compiler& comp, DataType& o, const SPIRType& i, u32 memberIndex /*= 0*/) {	
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

		case SRC::Image:
		case SRC::SampledImage: dataType.append("Texture"); break;
//		case SRC::AtomicCounter: break;
//		case SRC::Struct:					break;
		case SRC::Sampler: dataType.append("SamplerState"); break;
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
			default: throw SGE_ERROR("invalid texture dimension");
		}

		if (image.arrayed) dataType.append("Array");

#if 0 // !!<-------------- TODO
		if (image.ms) throw SGE_ERROR("unsupported feature image.ms");
		if (image.sampled) throw SGE_ERROR("unsupported image feature image.sampled={}", image.sampled);
		switch (image.format) {
			case ImageFormat::ImageFormatRgba32f: SGE_DUMP_VAR("TODO"); break;
			default:
				throw SGE_ERROR("unsupported spv ImageFormat: {}", static_cast<int>(image.format));
		};
#endif
	} else {
		if (vecsize == 1 && columns == 1) {					// scalar
			// do nothing
		} else if (vecsize > 1 && columns == 1) {
			FmtTo(dataType, "x{}", vecsize);				// vector
		} else if (columns > 1) {
			FmtTo(dataType, "_{}x{}", columns, vecsize);	// matrix
		}
	}

	if (!enumTryParse(o, dataType))
		throw SGE_ERROR("cannot parse dataType {}", dataType);

	SGE_ASSERT(o != DataType::None);
}

void ShaderCompiler_GL::_reflect(StrView outFilename, Compiler& comp, ShaderStageMask shaderStage, StrView profile) {
	ShaderStageInfo outInfo;
	outInfo.profile = profile;
	outInfo.stage	= shaderStage;

	{
		auto active = comp.get_active_interface_variables();
		ShaderResources resources = comp.get_shader_resources(active);
		comp.set_enabled_interface_variables(move(active));

#if 0
		for (const auto& resource : resources.stage_inputs) {
			printf("Input '%s':\tlayout set = %u\tlayout binding = %u\tlayout location= %u\n",
				resource.name.c_str(),
				comp.get_decoration(resource.id, spv::DecorationDescriptorSet),
				comp.get_decoration(resource.id, spv::DecorationBinding),
				comp.get_decoration(resource.id, spv::DecorationLocation));
		}

		for (const auto& resource : resources.stage_outputs) {
			printf("Output '%s':\tlayout set = %u\tlayout binding = %u\tlayout location= %u\n",
				resource.name.c_str(),
				comp.get_decoration(resource.id, spv::DecorationDescriptorSet),
				comp.get_decoration(resource.id, spv::DecorationBinding),
				comp.get_decoration(resource.id, spv::DecorationLocation));
		}

		for (const auto& resource : resources.uniform_buffers) {
			auto& uniform_type = comp.get_type(resource.base_type_id);

			int i = 0;
			for (const auto& member_type_id : uniform_type.member_types) {
				auto& member_type = comp.get_type(member_type_id);
				printf("uniform_member_name=%s\tvecsize=%u\tcolumns=%u\tmemberSize=%zu\tstartOffset=%u\n",
						comp.get_member_name(uniform_type.self, i).c_str(),
						member_type.vecsize,
						member_type.columns,
						comp.get_declared_struct_member_size(uniform_type, i),
						comp.type_struct_member_offset(uniform_type, i)
				);
				++i;
			}
		}
#endif

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

void ShaderCompiler_GL::_reflect_inputs(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources) {
	outInfo.inputs.reserve(resources.stage_inputs.size());

	for (const auto& resource : resources.stage_inputs) {
		auto& outInput = outInfo.inputs.emplace_back();

		using Slot = decltype(outInput.slot);

		auto resId = resource.id;
		StrView name(resource.name.data(), resource.name.size());

		outInput.slot = static_cast<Slot>(comp.get_decoration(resource.id, spv::DecorationLocation));

//		you should not care about Name here unless your backend assigns bindings based on name, or for debugging purposes
//		outInput.name.assign(name.data(), name.size());

		Util::convert(outInput.semantic, _findLastNameWithoutUnderscore(name));

		const auto& type	= comp.get_type(resource.base_type_id);
		auto componentCount	= type.vecsize;

		if (componentCount < 1 || componentCount > 4) {
			throw SGE_ERROR("invalid componentCount {}", componentCount);
		}

		_convert(comp, outInput.dataType, type);
	}
}

void ShaderCompiler_GL::_reflect_constBuffers(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources) {
	outInfo.constBuffers.reserve(resources.uniform_buffers.size());

	for (const auto& resource : resources.uniform_buffers) {
		auto& outCB = outInfo.constBuffers.emplace_back();

		using BindPoint = decltype(outCB.bindPoint);
		using BindCount = decltype(outCB.bindCount);
		using DataSize	= decltype(outCB.dataSize);

		auto  resId = resource.id;
		const auto& uniform_type = comp.get_type(resource.base_type_id);

		outCB.bindPoint = static_cast<BindPoint>(comp.get_decoration(resId, spv::DecorationBinding));

		if (outCB.bindPoint >= GL_MAX_UNIFORM_BUFFER_BINDINGS)
			throw SGE_ERROR("invalid bindPoint out of bound: {} >= {}", outCB.bindPoint, GL_MAX_UNIFORM_BUFFER_BINDINGS);

		outCB.name.assign(resource.name.c_str(), resource.name.size());
		outCB.dataSize = static_cast<DataSize>(comp.get_declared_struct_size(uniform_type));
		outCB.bindCount = Math::max(BindCount(uniform_type.array.size()), BindCount(1));

		int i = 0;
		for (const auto& member_type_id : uniform_type.member_types) {
			auto& member_type = comp.get_type(member_type_id);

			auto& outVar	  = outCB.variables.emplace_back();
			using SGE_Offset  = decltype(outVar.offset);

			const auto& member_name   = comp.get_member_name(uniform_type.self, i);
			const auto& member_offset = comp.type_struct_member_offset(uniform_type, i);

			outVar.name.assign(member_name.data(), member_name.size());
			outVar.offset = static_cast<SGE_Offset>(member_offset);

			if (comp.get_member_decoration(resId, i, spv::Decoration::DecorationRowMajor)) {
				outVar.rowMajor = true;
			}

			_convert(comp, outVar.dataType, member_type, i);

			++i;
		}
	}
}

void ShaderCompiler_GL::_reflect_textures(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources) {
	outInfo.textures.reserve(resources.separate_images.size());

	for (const auto& resource : resources.separate_images) {
		auto& outTex = outInfo.textures.emplace_back();

		using BindPoint = decltype(outTex.bindPoint);
		using BindCount = decltype(outTex.bindCount);
		using BindSet	= decltype(outTex.bindSet);

		auto  resId = resource.id;
		const auto& type = comp.get_type(resource.type_id);

		outTex.name.assign(resource.name.c_str(), resource.name.size());
		outTex.bindPoint = static_cast<BindPoint>(comp.get_decoration(resId, spv::DecorationBinding));
		outTex.bindCount = Math::max(BindCount(type.array.size()), BindCount(1));
		outTex.bindSet	 = static_cast<BindSet>(comp.get_decoration(resId, spv::DecorationDescriptorSet));

		_convert(comp, outTex.dataType, type);
	}
}

void ShaderCompiler_GL::_reflect_samplers(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources) {
	outInfo.samplers.reserve(resources.separate_samplers.size());

	for (const auto& resource : resources.separate_samplers) {
		auto& outSampler = outInfo.samplers.emplace_back();

		using BindPoint	= decltype(outSampler.bindPoint);
		using BindCount	= decltype(outSampler.bindCount);
		using BindSet	= decltype(outSampler.bindSet);		

		auto  resId = resource.id;
		const auto& type = comp.get_type(resource.type_id);

		outSampler.name.assign(resource.name.c_str(), resource.name.size());
		outSampler.bindPoint	= static_cast<BindPoint>(comp.get_decoration(resId, spv::DecorationBinding));
		outSampler.bindCount	= static_cast<BindSet>(type.array.empty() ? 1 : type.array[0]);
		outSampler.bindSet		= static_cast<BindSet>(comp.get_decoration(resId, spv::DecorationDescriptorSet));

		_convert(comp, outSampler.dataType, type);
	}
}

StrView ShaderCompiler_GL::_findLastNameWithoutUnderscore(StrView s) {
	auto pair = StringUtil::splitByChar(s, '_');
	while (!pair.second.empty()) {
		pair = StringUtil::splitByChar(pair.second, '_');
	}
	return pair.first;
}

void ShaderCompiler_GL::_beforeGLSLCompile(Compiler& comp, ShaderStageMask shaderStage, StrView profile) {
	ShaderResources resources = comp.get_shader_resources();
	
	switch (shaderStage) {
		case ShaderStageMask::Vertex: {
			_vsOutputLocation2VarName.resize(resources.stage_outputs.size());

			for (auto& resource : resources.stage_outputs) {
				const auto resId	= resource.id;
				const auto loc		= comp.get_decoration(resId, spv::DecorationLocation);

				auto& resName		= resource.name;
				auto& attribName	= _vsOutputLocation2VarName[loc];
				
				StrView view(resName.data(), resName.size());
				auto* p = StringUtil::findCharFromEnd(view, "_.", false);
				if (!p)
					throw SGE_ERROR("unexpected attrib name {}", resName.c_str());
				++p; // ignore "-."

				attribName.assign(p, view.end() - p);

//				SGE_LOG("[M] VA: (location = {}) {} -> {}", loc, resName.c_str(), attribName.c_str());

				resName.assign(attribName.data(), attribName.size());
				comp.set_name(resId, resName);
			}
		} break;
		case ShaderStageMask::Pixel: {
			for (auto& resource : resources.stage_inputs) {
				const auto resId	= resource.id;
				const auto loc		= comp.get_decoration(resId, spv::DecorationLocation);
				auto& resName		= resource.name;

				SGE_ASSERT(_vsOutputLocation2VarName.size() > loc);
				auto& attribName = _vsOutputLocation2VarName[loc];

//				SGE_LOG("[M] PA: (location = {}) {} -> {}", loc, resName.c_str(), attribName.c_str());

				resName.assign(attribName.data(), attribName.size());
				comp.set_name(resId, resName);
			}
		} break;
		default: throw SGE_ERROR("unexpeted");
	}
}

}