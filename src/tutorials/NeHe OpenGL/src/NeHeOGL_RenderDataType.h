#pragma once

namespace sge {

#define NeHeOGL_RenderDataType_ENUM_LIST(E) \
	E(None,) \
	\
	E(Int8,)	\
	E(Int16,)	\
	E(Int32,)	\
	E(Int64,)	\
	\
	E(UInt8,)	\
	E(UInt16,)	\
	E(UInt32,)	\
	E(UInt64,)	\
	\
	E(Float32,)	\
	E(Float64,)	\
	\
	E(Texture2D,) \
//----
SGE_ENUM_CLASS(NeHeOGL_RenderDataType, u8)

struct NeHeOGL_RenderDataTypeUtil {
	NeHeOGL_RenderDataTypeUtil() = delete;
	
	using Type = NeHeOGL_RenderDataType;

	template<class T> static constexpr Type get();

	template<> static constexpr Type get<void>() { return Type::None;  }

	template<> static constexpr Type get<i8 >() { return Type::Int8;  }
	template<> static constexpr Type get<i16>() { return Type::Int16; }
	template<> static constexpr Type get<i32>() { return Type::Int32; }
	template<> static constexpr Type get<i64>() { return Type::Int64; }

	template<> static constexpr Type get<u8 >() { return Type::UInt8;  }
	template<> static constexpr Type get<u16>() { return Type::UInt16; }
	template<> static constexpr Type get<u32>() { return Type::UInt32; }
	template<> static constexpr Type get<u64>() { return Type::UInt64; }

	template<> static constexpr Type get<f32>() { return Type::Float32; }
	template<> static constexpr Type get<f64>() { return Type::Float64; }
};

} // namespace sge