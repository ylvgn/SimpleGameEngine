#include "ShaderCompiler_GL.h"

#include <fmt/xchar.h>

namespace sge {

struct ShaderCompiler_GL_Helper { // TODO need removed!!
	ShaderCompiler_GL_Helper() = delete;

	using Resource = spirv_cross::Resource;

	static void remameTo(String& out, const Resource& res) {
		out.clear();
		auto& name = res.name;
		StrView view(name.data(), name.size());
		auto* p = StringUtil::findCharFromEnd(view, "_.", false);
		if (!p)
			throw SGE_ERROR("unexpected attr name error: {}", name.c_str());
		++p; // ignore "-."

		out.assign(p, view.end() - p);
	}

	static String rename(const Resource& res) { String o; remameTo(o, res); return o; }

}; // ShaderCompiler_GL_Helper

void ShaderCompiler_GL::compile(StrView outFilename, ShaderStageMask shaderStage, StrView profile, StrView srcFilename, StrView entryFunc, Vector<String>& include_dirs) {
	if (profile.empty()) {
		profile = Util::getGlStageProfile(shaderStage);
	}

	TempString spirvOutFilename = Fmt("{}.spv", outFilename);

	{ // HLSL -> SPIRV
		if (!File::exists(spirvOutFilename)) {
			auto outPath = FilePath::dirname(spirvOutFilename);
			Directory::create(outPath);

			TempString glslcBin;
			NativeUIApp::current()->executableDirPathRelativeTo(glslcBin, "glslc");

			{
				using Param = CommandLine::Param;
				using Param_Assignment = Param::Assignment;

				Vector<Param, 6> params;
				params.emplace_back(Param("-fhlsl-functionality1"));
				params.emplace_back(Param("-fhlsl-iomap"));

				auto fshader_stage = Param("-fshader-stage", FileExtension::get(shaderStage));
				fshader_stage.opAssignment = Param_Assignment::Equals;
				params.emplace_back(fshader_stage);

				auto fentry_point = Param("-fentry-point", entryFunc);
				fentry_point.opAssignment = Param_Assignment::Equals;
				params.emplace_back(fentry_point);

				for (const auto& inc : include_dirs) {
					TempString tmpIncludeDir = Fmt("-I\"{}\"", inc);
					params.emplace_back(tmpIncludeDir);
				}

				params.emplace_back(Param("-o", spirvOutFilename));
				params.emplace_back(Param("-x hlsl", srcFilename));

				CommandLine::runShell(glslcBin, params); // glslc supposed already in executable folder
			}
		}
	}

	{ // SPIRV -> GLSL
		MemMapFile mm;
		mm.open(spirvOutFilename);

		Span<const u32> bytecode = spanCast<const u32>(mm.span());
		Compiler comp(bytecode.data(), bytecode.size());

		_beforeCompileSPIRVToGLSL(comp, shaderStage, profile);

		CompilerOptions options;
		options.es = false;										// --no-es
		options.enable_420pack_extension = false;				// --no-420pack-extension
		if (!StringUtil::tryParse(profile, options.version)) {	// --version <glsl_profile>
			throw SGE_ERROR("invalid GLSL profile error: {}", profile);
		}

		comp.build_combined_image_samplers();
		comp.set_common_options(options);

		auto GLSLSource = comp.compile();
		StrView source(GLSLSource.c_str(), GLSLSource.size());
		File::writeFileIfChanged(outFilename, source, false);	// --output <glsl filename>
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
		case SRC::Half:		dataType.append("Float16");	break;
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

	if (image.type != 0) {
		using Dim				= spv::Dim;
		using ImageFormat		= spv::ImageFormat;
		using AccessQualifier	= spv::AccessQualifier;

		switch (image.dim) {
			case Dim::Dim1D: dataType.append("1D"); break;
			case Dim::Dim2D: dataType.append("2D"); break;
			case Dim::Dim3D: dataType.append("3D"); break;
			case Dim::DimCube: dataType.append("Cube"); break;
		//---
			default:
				throw SGE_ERROR("invalid texture dimension");
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

		if (!i.array.empty()) {
			dataType.append("Array");
		}
	}

	if (!enumTryParse(o, dataType)) {
		throw SGE_ERROR("cannot parse dataType {}", dataType);
	}

	if (o == DataType::None) {
		throw SGE_ERROR("dataType is None");
	}
}

void ShaderCompiler_GL::_reflect(StrView outFilename, Compiler& comp, ShaderStageMask shaderStage, StrView profile) {
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
		_reflect_storageBuffers (outInfo, comp, resources);
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

		using SGE_BindPoint = decltype(outCB.bindPoint);
		using SGE_BindCount = decltype(outCB.bindCount);
		using SGE_DataSize	= decltype(outCB.dataSize);

		auto  resId = resource.id;
		const auto& uniform_type = comp.get_type(resource.base_type_id);

		outCB.bindPoint = static_cast<SGE_BindPoint>(comp.get_decoration(resId, spv::DecorationBinding));

		if (outCB.bindPoint >= GL_MAX_UNIFORM_BUFFER_BINDINGS)
			throw SGE_ERROR("invalid bindPoint out of bound: {} >= {}", outCB.bindPoint, GL_MAX_UNIFORM_BUFFER_BINDINGS);

		outCB.name.assign(resource.name.c_str(), resource.name.size());
		outCB.dataSize = static_cast<SGE_DataSize>(comp.get_declared_struct_size(uniform_type));
		outCB.bindCount = Math::max(SGE_BindCount(uniform_type.array.size()), SGE_BindCount(1));

		int i = 0;
		for (const auto& member_type_id : uniform_type.member_types) {
			auto& outVar = outCB.variables.emplace_back();

			using SGE_Offset	    = decltype(outVar.offset);
			using SGE_ElementCount  = decltype(outVar.elementCount);
			using SGE_DataSize		= decltype(outVar.dataSize);

			auto& member_type				= comp.get_type(member_type_id);
			const auto& member_name			= comp.get_member_name(uniform_type.self, i);
			const auto& member_offset		= comp.type_struct_member_offset(uniform_type, i);
			const auto& struct_member_size  = comp.get_declared_struct_member_size(uniform_type, i);

			outVar.name.assign(member_name.data(), member_name.size());
			outVar.offset   = static_cast<SGE_Offset>(member_offset);
			outVar.dataSize = static_cast<SGE_DataSize>(struct_member_size);

			if (!member_type.array.empty()) { // array
				const auto& element_count = member_type.array[0];
				if (element_count > 0) {
					outVar.elementCount = static_cast<SGE_ElementCount>(element_count);
				} else {
					// TODO array runtime size
				}
			}

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
//		using BindSet	= decltype(outTex.bindSet);

		auto  resId		 = resource.id;
		const auto& type = comp.get_type(resource.type_id);

		outTex.name.assign(resource.name.c_str(), resource.name.size());
		outTex.bindPoint = static_cast<BindPoint>(comp.get_decoration(resId, spv::DecorationBinding));
		outTex.bindCount = Math::max(BindCount(type.array.size()), BindCount(1));
//		outTex.bindSet	 = static_cast<BindSet>(comp.get_decoration(resId, spv::DecorationDescriptorSet));

		_convert(comp, outTex.dataType, type);
	}
}

void ShaderCompiler_GL::_reflect_samplers(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources) {
	outInfo.samplers.reserve(resources.separate_samplers.size());

	for (const auto& resource : resources.separate_samplers) {
		auto& outSampler = outInfo.samplers.emplace_back();

		using BindPoint	= decltype(outSampler.bindPoint);
		using BindCount	= decltype(outSampler.bindCount);
//		using BindSet	= decltype(outSampler.bindSet);		

		auto  resId = resource.id;
		const auto& type = comp.get_type(resource.type_id);

		outSampler.name.assign(resource.name.c_str(), resource.name.size());
		outSampler.bindPoint = static_cast<BindPoint>(comp.get_decoration(resId, spv::DecorationBinding));
		outSampler.bindCount = static_cast<BindCount>(type.array.empty() ? 1 : type.array[0]);
//		outSampler.bindSet	 = static_cast<BindSet>(comp.get_decoration(resId, spv::DecorationDescriptorSet));

		_convert(comp, outSampler.dataType, type);
	}
}

void ShaderCompiler_GL::_reflect_storageBuffers(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources) {
	outInfo.storageBuffers.reserve(resources.storage_buffers.size());

	for (const auto& resource : resources.storage_buffers) {
		auto& outStorageBuffer = outInfo.storageBuffers.emplace_back();

		using BindPoint	= decltype(outStorageBuffer.bindPoint);
		using BindCount	= decltype(outStorageBuffer.bindCount);
//		using BindSet	= decltype(outStorageBuffer.rawUAV);

		auto  resId = resource.id;
		const auto& type = comp.get_type(resource.base_type_id);

		outStorageBuffer.name.assign(resource.name.c_str(), resource.name.size());
		outStorageBuffer.bindPoint = static_cast<BindPoint>(comp.get_decoration(resId, spv::DecorationBinding));
		outStorageBuffer.bindCount = static_cast<BindCount>(type.array.empty() ? 1 : type.array[0]);
//		outStorageBuffer.bindSet   = static_cast<BindSet>(comp.get_decoration(resId, spv::DecorationDescriptorSet));

		if (type.basetype == SPIRType::Struct) {
			if (!type.member_types.empty()) {
				const auto& member_type = comp.get_type(type.member_types[0]);

				if (!member_type.array.empty()) {
					if (member_type.array[0] == 0) {
						outStorageBuffer.rawUAV = true;
					}
					else {
						// TODO Fixed-size array
					}
				}

#if 0 // no need atm
				// Check member base type
				switch (member_type.basetype) {
					case SPIRType::UInt: SGE_DUMP_VAR("Member Base type: uint"); break;
					default: break;
				}
#endif
			}
		}
	}
}

StrView ShaderCompiler_GL::_findLastNameWithoutUnderscore(StrView s) { // TODO need removed
	auto pair = StringUtil::splitByChar(s, '_');
	while (!pair.second.empty()) {
		pair = StringUtil::splitByChar(pair.second, '_');
	}
	return pair.first;
}

void ShaderCompiler_GL::_beforeCompileSPIRVToGLSL(Compiler& comp, ShaderStageMask shaderStage, StrView profile) {
	using Helper = ShaderCompiler_GL_Helper;

	ShaderResources resources = comp.get_shader_resources();

// $(spirv-cross) --rename-interface-variable <in|out> <location> <new_variable_name>
	switch (shaderStage) {
		case ShaderStageMask::Vertex: {
			for (auto& resource : resources.stage_outputs) {
				const auto resId	= resource.id;
				const auto loc		= comp.get_decoration(resId, spv::DecorationLocation);

				TempString attrName = Helper::rename(resource);
//				SGE_LOG("[M] VA: (location = {}) {} -> {}", loc, resource.name.c_str(), attrName.c_str());
				SGE_UNUSED(loc);

				resource.name.assign(attrName.data(), attrName.size());
				comp.set_name(resId, resource.name); // --rename-interface-variable in <loc> <new_variable_name>
			}
		} break;
		case ShaderStageMask::Pixel: {
			for (auto& resource : resources.stage_inputs) {
				const auto resId	= resource.id;
				const auto loc		= comp.get_decoration(resId, spv::DecorationLocation);

				TempString attrName = Helper::rename(resource);
//				SGE_LOG("[M] PA: (location = {}) {} -> {}", loc, resource.name.c_str(), attrName.c_str());
				SGE_UNUSED(loc);

				resource.name.assign(attrName.data(), attrName.size());
				comp.set_name(resId, resource.name); // --rename-interface-variable out <loc> <new_variable_name>
			}
		} break;
		case ShaderStageMask::Compute: {
			// do nothing
		} break;
	//---
		default:
			throw SGE_ERROR("ShaderCompiler_GL: unsupported ShaderStageMask '{}'", shaderStage);
	}
}

} // namespace sge