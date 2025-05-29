#pragma once

#include <sge_render/RenderDataType.h>
#include <sge_render/vertex/Vertex.h>
#include <sge_core/serializer/json/JsonUtil.h>
#include "RenderState.h"

namespace sge {

#define ShaderStageMask_ENUM_LIST(E) \
	E(None,    = 0) \
	E(Vertex,  = 1 << 0) \
	E(Pixel,   = 1 << 1) \
	E(Compute, = 1 << 2) \
//----
SGE_ENUM_CLASS(ShaderStageMask, u8)
SGE_ENUM_ALL_OPERATOR(ShaderStageMask)

#define ShaderPropType_ENUM_LIST(E) \
	E(None,) \
	E(Int,) \
	E(Float,) \
	E(Vec2f,) \
	E(Vec3f,) \
	E(Vec4f,) \
	E(Color4f,) \
	E(Texture2D,) \
//----
SGE_ENUM_CLASS(ShaderPropType, u8)

#if 0
#pragma mark ========= ShaderPropTypeUtil ============
#endif
struct ShaderPropTypeUtil {	
	ShaderPropTypeUtil() = delete;

	using Type = ShaderPropType;

	template<class T> static constexpr Type get();

	template<> static constexpr	Type get<i32>()		{ return Type::Int; }
	template<> static constexpr	Type get<f32>()		{ return Type::Float; }
	template<> static constexpr	Type get<Vec2f>()	{ return Type::Vec2f; }
	template<> static constexpr	Type get<Vec3f>()	{ return Type::Vec3f; }
	template<> static constexpr	Type get<Vec4f>()	{ return Type::Vec4f; }
	template<> static constexpr	Type get<Color4f>()	{ return Type::Color4f; }
}; // ShaderPropTypeUtil
SGE_STATIC_ASSERT_NO_MEMBER_CLASS(ShaderPropTypeUtil);

#if 0
#pragma mark ========= ShaderInfo ============
#endif
struct ShaderInfo {

	struct Prop {
		ShaderPropType	propType = ShaderPropType::None;
		String			name;
		String			displayName;
		String			defaultValue;

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, name);
			SGE_NAMED_IO(se, displayName);
			SGE_NAMED_IO(se, defaultValue);
		}
	};

	struct Pass {
		String			name;
		String			vsFunc;
		String			psFunc;
		String			csFunc;
		RenderState		renderState;

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, name);
			SGE_NAMED_IO(se, vsFunc);
			SGE_NAMED_IO(se, psFunc);
			SGE_NAMED_IO(se, csFunc);
			SGE_NAMED_IO(se, renderState);
		}
	};

	Vector<Prop, 8>	props;
	Vector<Pass, 1>	passes;

	String name;

	void clear();

	void onFormat(fmt::format_context& ctx) const;

	template<class SE>
	void onJson(SE& se) {
		SGE_NAMED_IO(se, props);
		SGE_NAMED_IO(se, passes);
	}
}; // ShaderInfo
SGE_FORMATTER(ShaderInfo)


#if 0
#pragma mark ========= ShaderStageProfile ============
#endif
class ShaderStageProfile {
public:
	constexpr static StrLiteral DX11_VS = "vs_5_0";
	constexpr static StrLiteral DX11_PS = "ps_5_0";
	constexpr static StrLiteral DX11_CS = "cs_5_0";

	constexpr static StrLiteral GLSL_VS = "vs_330";
	constexpr static StrLiteral GLSL_PS = "ps_330";
	constexpr static StrLiteral GLSL_CS = "cs_430";

	constexpr static StrLiteral SPIRV_VS = "vs_1_1";
	constexpr static StrLiteral SPIRV_PS = "ps_1_1";
	constexpr static StrLiteral SPIRV_CS = "cs_1_1";
}; // ShaderStageProfile


#if 0
#pragma mark ========= ShaderStageFileExtension ============
#endif
class ShaderStageFileExtension {
public:
	constexpr static StrLiteral VS   = "vertex";
	constexpr static StrLiteral PS   = "fragment";
	constexpr static StrLiteral TESC = "tesscontrol";
	constexpr static StrLiteral TESE = "tesseval";
	constexpr static StrLiteral GS   = "geometry";
	constexpr static StrLiteral CS   = "compute";

	static constexpr StrLiteral get(ShaderStageMask s) {
		using SRC = ShaderStageMask;

		switch (s) {
			case SRC::Vertex:	return VS;
			case SRC::Pixel:	return PS;
			case SRC::Compute:	return CS;
		//---
			default: throw SGE_ERROR("unsupported ShaderStageMask '{}'", s);
		}
	}
}; // ShaderStageFileExtension
SGE_STATIC_ASSERT_NO_MEMBER_CLASS(ShaderStageFileExtension);

#if 0
#pragma mark ========= ShaderStageInfo ============
#endif
class ShaderStageInfo {
public:
	using DataType = RenderDataType;

	void loadFile(StrView filename_) {
		filename = filename_;
		JsonUtil::readFile(filename_, *this);
	}

	String			filename;
	String			profile;
	ShaderStageMask	stage = ShaderStageMask::None;

//----------
	class Param {
	public:
		String	name;
		DataType dataType;
		i16		 bindPoint = 0;
		i16		 bindCount = 0;

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, name);
			SGE_NAMED_IO(se, dataType);
			SGE_NAMED_IO(se, bindPoint);
			SGE_NAMED_IO(se, bindCount);
		}
	};

//----------
	class Input {
	public:
		String			name;
		i16				slot = 0; // OpenGL location
		VertexSemantic	semantic = VertexSemantic::None; // DirectX
		RenderDataType	dataType = RenderDataType::None;

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, name);
			SGE_NAMED_IO(se, slot);
			SGE_NAMED_IO(se, semantic);
			SGE_NAMED_IO(se, dataType);
		}
	};

//----------
	class Variable {
	public:
		String		name;
		size_t		offset		  = 0;
		size_t		dataSize	  = 0;
		u16			elementCount  = 0;
		DataType	dataType	  = DataType::None;
		bool		rowMajor	  = true;

		bool isArray() const { return elementCount > 0; }

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, name);
			SGE_NAMED_IO(se, offset);
			SGE_NAMED_IO(se, dataSize);
			SGE_NAMED_IO(se, elementCount);
			SGE_NAMED_IO(se, dataType);
			SGE_NAMED_IO(se, rowMajor);
		}
	};

//----------
	class ConstBuffer {
	public:
		String	name;
		size_t	dataSize  = 0;
		i16		bindPoint = 0;
		i16		bindCount = 0;
		Vector<Variable, 4>	variables;

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, name);
			SGE_NAMED_IO(se, dataSize);
			SGE_NAMED_IO(se, bindPoint);
			SGE_NAMED_IO(se, bindCount);
			SGE_NAMED_IO(se, variables);
		}

		const Variable* findVariable(StrView propName) const {
			for (auto& v : variables) {
				if (v.name == propName) return &v;
			}
			return nullptr;
		}
	};

	Vector<Input, 8>		inputs;
	Vector<Param, 8>		params;
	Vector<ConstBuffer, 4>	constBuffers;

//----------
	class Texture {
	public:
		String		name;
		i16			bindPoint	= 0;
		i16			bindCount	= 0;
		DataType	dataType	= DataType::None;

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, name);
			SGE_NAMED_IO(se, bindPoint);
			SGE_NAMED_IO(se, bindCount);
			SGE_NAMED_IO(se, dataType);
		}
	};
	Vector<Texture, 8> textures;

//----------
	class Sampler {
	public:
		String			name;
		i16				bindPoint	= 0;
		i16				bindCount	= 0;
		RenderDataType	dataType	= RenderDataType::None;

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, name);
			SGE_NAMED_IO(se, bindPoint);
			SGE_NAMED_IO(se, bindCount);
			SGE_NAMED_IO(se, dataType);
		}
	};
	Vector<Sampler, 8> samplers;

//----------
	class StorageBuffer {
	public:
		String	name;
		i16		bindPoint = 0;
		i16		bindCount = 0;
		bool	rawUAV	  = false;

		template<class SE>
		void onJson(SE & se) {
			SGE_NAMED_IO(se, name);
			SGE_NAMED_IO(se, bindPoint);
			SGE_NAMED_IO(se, bindCount);
			SGE_NAMED_IO(se, rawUAV);
		}
	};
	Vector<StorageBuffer, 8> storageBuffers;

//----------
	template<class SE>
	void onJson(SE& se) {
		SGE_NAMED_IO(se, profile);
		SGE_NAMED_IO(se, inputs);
		SGE_NAMED_IO(se, params);
		SGE_NAMED_IO(se, constBuffers);
		SGE_NAMED_IO(se, textures);
		SGE_NAMED_IO(se, samplers);
		SGE_NAMED_IO(se, storageBuffers);
	}
}; // ShaderStageInfo

} // namespace sge
