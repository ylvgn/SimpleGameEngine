#pragma once

#include <sge_core.h>

namespace sge {

#define RenderPrimitiveType_ENUM_LIST(E) \
	E(None,) \
	E(Points,) \
	E(Lines,) \
	E(Triangles,) \
//----
SGE_ENUM_CLASS(RenderPrimitiveType, u8)

#define RenderGpuBufferType_ENUM_LIST(E) \
	E(None,) \
	E(Vertex,) \
	E(Index,) \
	E(Const,) \
	E(Storage,) \
//----
SGE_ENUM_CLASS(RenderGpuBufferType, u8)

#define RenderDataType_ENUM_LIST(E) \
	E(None,) \
	\
	E(Int8, )	E(Int8x2, )		E(Int8x3, )		E(Int8x4, )		\
	E(Int16, )	E(Int16x2, )	E(Int16x3, )	E(Int16x4, )	\
	E(Int32, )	E(Int32x2, )	E(Int32x3, )	E(Int32x4, )	\
	E(Int64, )	E(Int64x2, )	E(Int64x3, )	E(Int64x4, )	\
	\
	E(Int8Array, )	E(Int8x2Array, )	E(Int8x3Array, )	E(Int8x4Array, )	\
	E(Int16Array, )	E(Int16x2Array, )	E(Int16x3Array, )	E(Int16x4Array, )	\
	E(Int32Array, )	E(Int32x2Array, )	E(Int32x3Array, )	E(Int32x4Array, )	\
	E(Int64Array, )	E(Int64x2Array, )	E(Int64x3Array, )	E(Int64x4Array, )	\
	\
	E(UInt8,)	E(UInt8x2,)	 E(UInt8x3,)	E(UInt8x4,)		\
	E(UInt16,)	E(UInt16x2,) E(UInt16x3,)	E(UInt16x4,)	\
	E(UInt32,)	E(UInt32x2,) E(UInt32x3,)	E(UInt32x4,)	\
	E(UInt64,)	E(UInt64x2,) E(UInt64x3,)	E(UInt64x4,)	\
	\
	E(UInt8Array,)	E(UInt8x2Array,)	E(UInt8x3Array,)	E(UInt8x4Array,)	\
	E(UInt16Array,)	E(UInt16x2Array,)	E(UInt16x3Array,)	E(UInt16x4Array,)	\
	E(UInt32Array,)	E(UInt32x2Array,)	E(UInt32x3Array,)	E(UInt32x4Array,)	\
	E(UInt64Array,)	E(UInt64x2Array,)	E(UInt64x3Array,)	E(UInt64x4Array,)	\
	\
	E(Float16,)	E(Float16x2,) E(Float16x3,)	E(Float16x4,)	\
	E(Float32,)	E(Float32x2,) E(Float32x3,)	E(Float32x4,)	\
	E(Float64,)	E(Float64x2,) E(Float64x3,)	E(Float64x4,)	\
	\
	E(Float16Array,) E(Float16x2Array,)	E(Float16x3Array,) E(Float16x4Array,)	\
	E(Float32Array,) E(Float32x2Array,)	E(Float32x3Array,) E(Float32x4Array,)	\
	E(Float64Array,) E(Float64x2Array,)	E(Float64x3Array,) E(Float64x4Array,)	\
	\
	E(SNorm8,)	E(SNorm8x2,)	E(SNorm8x3,)	E(SNorm8x4,)	\
	E(SNorm16,)	E(SNorm16x2,)	E(SNorm16x3,)	E(SNorm16x4,)	\
	E(SNorm32,)	E(SNorm32x2,)	E(SNorm32x3,)	E(SNorm32x4,)	\
	\
	E(SNorm8Array,)	 E(SNorm8x2Array,)	E(SNorm8x3Array,)	E(SNorm8x4Array,)	\
	E(SNorm16Array,) E(SNorm16x2Array,)	E(SNorm16x3Array,)	E(SNorm16x4Array,)	\
	E(SNorm32Array,) E(SNorm32x2Array,)	E(SNorm32x3Array,)	E(SNorm32x4Array,)	\
	\
	E(UNorm8,)	E(UNorm8x2,)	E(UNorm8x3,)	E(UNorm8x4,)	\
	E(UNorm16,)	E(UNorm16x2,)	E(UNorm16x3,)	E(UNorm16x4,)	\
	E(UNorm32,)	E(UNorm32x2,)	E(UNorm32x3,)	E(UNorm32x4,)	\
	\
	E(UNorm8Array,)	 E(UNorm8x2Array,)	E(UNorm8x3Array,)	E(UNorm8x4Array,)	\
	E(UNorm16Array,) E(UNorm16x2Array,)	E(UNorm16x3Array,)	E(UNorm16x4Array,)	\
	E(UNorm32Array,) E(UNorm32x2Array,)	E(UNorm32x3Array,)	E(UNorm32x4Array,)	\
	\
	E(Float16_2x2, ) E(Float16_3x3,) E(Float16_4x4,) E(Float16_4x3,) \
	E(Float32_2x2, ) E(Float32_3x3,) E(Float32_4x4,) E(Float32_4x3,) \
	E(Float64_2x2, ) E(Float64_3x3,) E(Float64_4x4,) E(Float64_4x3,) \
	\
	E(Bool,)		E(Boolx2,)		 E(Boolx3,)		  E(Boolx4,) \
	E(BoolArray,)	E(Boolx2Array,)  E(Boolx3Array,)  E(Boolx4Array,) \
	\
	E(Texture1D,)		E(Texture2D,)		E(Texture3D,)		E(TextureCube,) \
	E(Texture1DArray,)	E(Texture2DArray,)	E(Texture3DArray,)	E(TextureCubeArray,) \
	\
	E(SamplerState,) \
	\
	E(GpuStorageBuffer,) \
	\
//----
SGE_ENUM_CLASS(RenderDataType, u8)

class RenderDataTypeUtil {
private:
	RenderDataTypeUtil() = delete;
public:
	using DataType = RenderDataType;

	static constexpr u16 sizeInBytes (DataType t);
	static constexpr u16 elementCount(DataType t);

	template<class T>
	static constexpr DataType get();
};
SGE_STATIC_ASSERT_NO_MEMBER_CLASS(RenderDataTypeUtil);

SGE_INLINE constexpr
u16 RenderDataTypeUtil::sizeInBytes(DataType t) {
	switch (t) {
		case DataType::Int8:		return sizeof(i8);
		case DataType::Int8x2:		return sizeof(i8) * 2;
		case DataType::Int8x3:		return sizeof(i8) * 3;
		case DataType::Int8x4:		return sizeof(i8) * 4;
	//---
		case DataType::UInt8:		return sizeof(u8);
		case DataType::UInt8x2:		return sizeof(u8) * 2;
		case DataType::UInt8x3:		return sizeof(u8) * 3;
		case DataType::UInt8x4:		return sizeof(u8) * 4;
	//---
		case DataType::Int16:		return sizeof(i16);
		case DataType::Int16x2:		return sizeof(i16) * 2;
		case DataType::Int16x3:		return sizeof(i16) * 3;
		case DataType::Int16x4:		return sizeof(i16) * 4;
	//---
		case DataType::UInt16:		return sizeof(u16);
		case DataType::UInt16x2:	return sizeof(u16) * 2;
		case DataType::UInt16x3:	return sizeof(u16) * 3;
		case DataType::UInt16x4:	return sizeof(u16) * 4;
	//---
		case DataType::Int32:		return sizeof(i32);
		case DataType::Int32x2:		return sizeof(i32) * 2;
		case DataType::Int32x3:		return sizeof(i32) * 3;
		case DataType::Int32x4:		return sizeof(i32) * 4;
	//---
		case DataType::UInt32:		return sizeof(u32);
		case DataType::UInt32x2:	return sizeof(u32) * 2;
		case DataType::UInt32x3:	return sizeof(u32) * 3;
		case DataType::UInt32x4:	return sizeof(u32) * 4;
	//---
		case DataType::Int64:		return sizeof(i64);
		case DataType::Int64x2:		return sizeof(i64) * 2;
		case DataType::Int64x3:		return sizeof(i64) * 3;
		case DataType::Int64x4:		return sizeof(i64) * 4;
	//---
		case DataType::UInt64:		return sizeof(u64);
		case DataType::UInt64x2:	return sizeof(u64) * 2;
		case DataType::UInt64x3:	return sizeof(u64) * 3;
		case DataType::UInt64x4:	return sizeof(u64) * 4;
	//---
//		case DataType::Float16:		return sizeof(f16);
//		case DataType::Float16x2:	return sizeof(f16) * 2;
//		case DataType::Float16x3:	return sizeof(f16) * 3;
//		case DataType::Float16x4:	return sizeof(f16) * 4;
	//---
		case DataType::Float32:		return sizeof(f32);
		case DataType::Float32x2:	return sizeof(f32) * 2;
		case DataType::Float32x3:	return sizeof(f32) * 3;
		case DataType::Float32x4:	return sizeof(f32) * 4;
	//---
		case DataType::Float64:		return sizeof(f64);
		case DataType::Float64x2:	return sizeof(f64) * 2;
		case DataType::Float64x3:	return sizeof(f64) * 3;
		case DataType::Float64x4:	return sizeof(f64) * 4;
	//---
//		case DataType::Float16_2x2: return sizeof(f16) * 2 * 2;
//		case DataType::Float16_3x3: return sizeof(f16) * 3 * 3;
//		case DataType::Float16_4x4: return sizeof(f16) * 4 * 4;
//		case DataType::Float16_4x3: return sizeof(f16) * 4 * 3;
	//---
		case DataType::Float32_2x2: return sizeof(f32) * 2 * 2;
		case DataType::Float32_3x3: return sizeof(f32) * 3 * 3;
		case DataType::Float32_4x4: return sizeof(f32) * 4 * 4;
		case DataType::Float32_4x3: return sizeof(f32) * 4 * 3;
	//---
		case DataType::Float64_2x2: return sizeof(f64) * 2 * 2;
		case DataType::Float64_3x3: return sizeof(f64) * 3 * 3;
		case DataType::Float64_4x4: return sizeof(f64) * 4 * 4;
		case DataType::Float64_4x3: return sizeof(f64) * 4 * 3;
	//---
		case DataType::SamplerState:	return 0;
		case DataType::Texture1D:		return 0;
		case DataType::Texture2D:		return 0;
		case DataType::Texture3D:		return 0;
		case DataType::TextureCube:		return 0;
		case DataType::GpuStorageBuffer:return 0;
	//---
		case DataType::Int8Array:		return 0;
		case DataType::Int16Array:		return 0;
		case DataType::Int32Array:		return 0;
		case DataType::Int64Array:		return 0;
		case DataType::Int8x2Array:		return 0;
		case DataType::Int16x2Array:	return 0;
		case DataType::Int32x2Array:	return 0;
		case DataType::Int64x2Array:	return 0;
		case DataType::Int8x3Array:		return 0;
		case DataType::Int16x3Array:	return 0;
		case DataType::Int32x3Array:	return 0;
		case DataType::Int64x3Array:	return 0;
		case DataType::Int8x4Array:		return 0;
		case DataType::Int16x4Array:	return 0;
		case DataType::Int32x4Array:	return 0;
		case DataType::Int64x4Array:	return 0;
	//---
		case DataType::UInt8Array:		return 0;
		case DataType::UInt16Array:		return 0;
		case DataType::UInt32Array:		return 0;
		case DataType::UInt64Array:		return 0;
		case DataType::UInt8x2Array:	return 0;
		case DataType::UInt16x2Array:	return 0;
		case DataType::UInt32x2Array:	return 0;
		case DataType::UInt64x2Array:	return 0;
		case DataType::UInt8x3Array:	return 0;
		case DataType::UInt16x3Array:	return 0;
		case DataType::UInt32x3Array:	return 0;
		case DataType::UInt64x3Array:	return 0;
		case DataType::UInt8x4Array:	return 0;
		case DataType::UInt16x4Array:	return 0;
		case DataType::UInt32x4Array:	return 0;
		case DataType::UInt64x4Array:	return 0;
	//---
		case DataType::SNorm8Array:		return 0;
		case DataType::SNorm8x2Array:	return 0;
		case DataType::SNorm8x3Array:	return 0;
		case DataType::SNorm8x4Array:	return 0;
		case DataType::SNorm16Array:	return 0;
		case DataType::SNorm16x2Array:	return 0;
		case DataType::SNorm16x3Array:	return 0;
		case DataType::SNorm16x4Array:	return 0;
		case DataType::SNorm32Array:	return 0;
		case DataType::SNorm32x2Array:	return 0;
		case DataType::SNorm32x3Array:	return 0;
		case DataType::SNorm32x4Array:	return 0;
	//---
		case DataType::UNorm8Array:		return 0;
		case DataType::UNorm8x2Array:	return 0;
		case DataType::UNorm8x3Array:	return 0;
		case DataType::UNorm8x4Array:	return 0;
		case DataType::UNorm16Array:	return 0;
		case DataType::UNorm16x2Array:	return 0;
		case DataType::UNorm16x3Array:	return 0;
		case DataType::UNorm16x4Array:	return 0;
		case DataType::UNorm32Array:	return 0;
		case DataType::UNorm32x2Array:	return 0;
		case DataType::UNorm32x3Array:	return 0;
		case DataType::UNorm32x4Array:	return 0;
	//---
		case DataType::Float16Array:	return 0;
		case DataType::Float16x2Array:	return 0;
		case DataType::Float16x3Array:	return 0;
		case DataType::Float16x4Array:	return 0;
		case DataType::Float32Array:	return 0;
		case DataType::Float32x2Array:	return 0;
		case DataType::Float32x3Array:	return 0;
		case DataType::Float32x4Array:	return 0;
		case DataType::Float64Array:	return 0;
		case DataType::Float64x2Array:	return 0;
		case DataType::Float64x3Array:	return 0;
		case DataType::Float64x4Array:	return 0;
	//---
		case DataType::BoolArray:	return 0;
		case DataType::Boolx2Array:	return 0;
		case DataType::Boolx3Array:	return 0;
		case DataType::Boolx4Array:	return 0;
	//---
		default:
			throw SGE_ERROR("unsupported render data type = {}", t);
	}
}

SGE_INLINE constexpr
u16 RenderDataTypeUtil::elementCount(DataType t) {
	switch (t) {
		case DataType::Bool:
		case DataType::Int8:
		case DataType::Int16:
		case DataType::Int32:
		case DataType::Int64:
		case DataType::UInt8:
		case DataType::UInt16:
		case DataType::UInt32:
		case DataType::UInt64:
		case DataType::Float16:
		case DataType::Float32:
		case DataType::Float64:
			return 1;

		case DataType::Int8x2:
		case DataType::Int16x2:
		case DataType::Int32x2:
		case DataType::Int64x2:
		case DataType::UInt8x2:
		case DataType::UInt16x2:
		case DataType::UInt32x2:
		case DataType::UInt64x2:
		case DataType::Float16x2:
		case DataType::Float32x2:
		case DataType::Float64x2:
			return 2;

		case DataType::Int8x3:
		case DataType::Int16x3:
		case DataType::Int32x3:
		case DataType::Int64x3:
		case DataType::UInt8x3:
		case DataType::UInt16x3:
		case DataType::UInt32x3:
		case DataType::UInt64x3:
		case DataType::Float16x3:
		case DataType::Float32x3:
		case DataType::Float64x3:
			return 3;

		case DataType::Int8x4:
		case DataType::Int16x4:
		case DataType::Int32x4:
		case DataType::Int64x4:
		case DataType::UInt8x4:
		case DataType::UInt16x4:
		case DataType::UInt32x4:
		case DataType::UInt64x4:
		case DataType::Float16x4:
		case DataType::Float32x4:
		case DataType::Float64x4:
			return 4;
	//---
		default:
			throw SGE_ERROR("unsupported render data type = {}", t);
	}
}

template<> static constexpr RenderDataType RenderDataTypeUtil::get<void>()			{ return DataType::None; }

template<> static constexpr RenderDataType RenderDataTypeUtil::get<i8 >()			{ return DataType::Int8; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<i16>()			{ return DataType::Int16; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<i32>()			{ return DataType::Int32; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<i64>()			{ return DataType::Int64; }

template<> static constexpr RenderDataType RenderDataTypeUtil::get<u8 >()			{ return DataType::UInt8; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<u16>()			{ return DataType::UInt16; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<u32>()			{ return DataType::UInt32; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<u64>()			{ return DataType::UInt64; }

template<> static constexpr RenderDataType RenderDataTypeUtil::get<f32>()			{ return DataType::Float32; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<f64>()			{ return DataType::Float64; }

template<> static constexpr RenderDataType RenderDataTypeUtil::get<Tuple2i>()		{ return DataType::Int32x2; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<Tuple2f>()		{ return DataType::Float32x2; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<Tuple2d>()		{ return DataType::Float64x2; }

template<> static constexpr RenderDataType RenderDataTypeUtil::get<Tuple3i>()		{ return DataType::Int32x3; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<Tuple3f>()		{ return DataType::Float32x3; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<Tuple3d>()		{ return DataType::Float64x3; }

template<> static constexpr RenderDataType RenderDataTypeUtil::get<Tuple4i>()		{ return DataType::Int32x4; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<Tuple4f>()		{ return DataType::Float32x4; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<Tuple4d>()		{ return DataType::Float64x4; }

template<> static constexpr RenderDataType RenderDataTypeUtil::get<Mat4f>()			{ return DataType::Float32_4x4; }

template<> static constexpr RenderDataType RenderDataTypeUtil::get<Color4b>()		{ return DataType::UNorm8x4; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<Color4f>()		{ return DataType::Float32x4; }

class Texture1D;
class Texture2D;
class Texture3D;
class TextureCube;
template<> static constexpr RenderDataType RenderDataTypeUtil::get<Texture1D>()		{ return DataType::Texture1D; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<Texture2D>()		{ return DataType::Texture2D; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<Texture3D>()		{ return DataType::Texture3D; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<TextureCube>()	{ return DataType::TextureCube; }

class RenderGpuStorageBuffer;
template<> static constexpr RenderDataType RenderDataTypeUtil::get<RenderGpuStorageBuffer>() { return DataType::GpuStorageBuffer; }

template<class T>
class RenderDataType_Array : public NonCopyable {
	using This		= RenderDataType_Array;
	using DataType	= RenderDataType;
public:
	using ElementType = T;

	static constexpr DataType s_ElementDataType()	 { return RenderDataTypeUtil::get<ElementType>(); }
	static constexpr u16	  s_ElementSizeInBytes() { return RenderDataTypeUtil::sizeInBytes(s_ElementDataType()); }
	static constexpr u16      s_PackingSizeInBytes() { return RenderDataTypeUtil::sizeInBytes(DataType::Float32x4); } // https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules

	This(T* data, size_t dataSize) noexcept : _data(data, dataSize) {}
	This(Span<T>& s)			   noexcept : _data(s) {}

			T& operator[]	(int i)			{ return _data[i]; }
	const	T& operator[]	(int i)	const	{ return _data[i]; }

	size_t size()		 const { return _data.size(); }
		T* data()		 const { return _data.data(); }
	size_t sizeInBytes() const { return _data.size_bytes(); }

	ByteSpan toSpan()		  const { return ByteSpan_make(_data); }

protected:
	This() = delete;
	Span<T> _data;
};

using RenderInt32x2Array	= RenderDataType_Array<Tuple2i>;
using RenderInt32x3Array	= RenderDataType_Array<Tuple3i>;
using RenderInt32x4Array	= RenderDataType_Array<Tuple4i>;

using RenderFloat32x2Array	= RenderDataType_Array<Tuple2f>;
using RenderFloat32x3Array  = RenderDataType_Array<Tuple3f>;
using RenderFloat32x4Array	= RenderDataType_Array<Tuple4f>;

using RenderFloat64x2Array  = RenderDataType_Array<Tuple2d>;
using RenderFloat64x3Array  = RenderDataType_Array<Tuple3d>;
using RenderFloat64x4Array  = RenderDataType_Array<Tuple4d>;

template<> static constexpr RenderDataType RenderDataTypeUtil::get<RenderInt32x2Array>() { return DataType::Int32x2Array; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<RenderInt32x3Array>() { return DataType::Int32x3Array; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<RenderInt32x4Array>() { return DataType::Int32x4Array; }

template<> static constexpr RenderDataType RenderDataTypeUtil::get<RenderFloat32x2Array>() { return DataType::Float32x2Array; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<RenderFloat32x3Array>() { return DataType::Float32x3Array; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<RenderFloat32x4Array>() { return DataType::Float32x4Array; }

template<> static constexpr RenderDataType RenderDataTypeUtil::get<RenderFloat64x2Array>() { return DataType::Float64x2Array; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<RenderFloat64x3Array>() { return DataType::Float64x3Array; }
template<> static constexpr RenderDataType RenderDataTypeUtil::get<RenderFloat64x4Array>() { return DataType::Float64x4Array; }

} // namespace sge
