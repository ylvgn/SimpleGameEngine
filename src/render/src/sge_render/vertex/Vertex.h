#pragma once

#include "../Render_Common.h"
#include "../RenderDataType.h"

namespace sge {

//    : size bit(from start bit)
// posType			: 8 bit(from 0)
// colorType		: 8 bit(from 8)
// colorCount		: 2 bit(from 16), colorType
// uvType			: 8 bit(from 18)
// uvCount			: 8 bit(from 26), uvType
// normalType		: 8 bit(from 34)
// normalCount		: 2 bit(from 42), normalType
// tangentCount		: 2 bit(from 44), normalType
// binormalCount	: 2 bit(from 46), normalType
// xxxxxxxxxxxxxx   : ? bit(from 48)
enum class VertexType : u64 { None = 0 }; // VertexType = VertexSemantic|RenderDataType

struct VertexTypeUtil {
#define LSR(V, N) (static_cast<u64>((V)) << (N))
	static constexpr VertexType addPos(VertexType t, RenderDataType posType) {
		return static_cast<VertexType>(static_cast<u64>(t)
			| static_cast<u64>(posType));
	}

	static constexpr VertexType addColor(VertexType t, RenderDataType colorType, u8 colorCount) {
		return static_cast<VertexType>(static_cast<u64>(t)
			| LSR(colorType, 8)
			| LSR(colorCount, 16));
	}

	static constexpr VertexType addUv(VertexType t, RenderDataType uvType, u8 uvCount) {
		return static_cast<VertexType>(static_cast<u64>(t)
			| LSR(uvType, 18)
			| LSR(uvCount, 26));
	}

	static constexpr VertexType addNormal(VertexType t, RenderDataType normalType, u8 normalCount) {
		return static_cast<VertexType>(static_cast<u64>(t)
			| LSR(normalType, 34)
			| LSR(normalCount, 42));
	}

	static constexpr VertexType addTangent(VertexType t, u8 tangentCount) {
		return static_cast<VertexType>(static_cast<u64>(t)
			| LSR(tangentCount, 44));
	}

	static constexpr VertexType addBinormal(VertexType t, u8 binormalCount) {
		return static_cast<VertexType>(static_cast<u64>(t)
			| LSR(binormalCount, 46));
	}
#undef LSR
	static constexpr VertexType make(
		RenderDataType posType,
		RenderDataType colorType, u8 colorCount,
		RenderDataType uvType, u8 uvCount,
		RenderDataType normalType, u8 normalCount, u8 tangentCount, u8 binormalCount)
	{
		VertexType t = addPos(VertexType::None, posType);
		if (colorCount)	 t = addColor(t, colorType, colorCount);
		if (uvCount)	 t = addUv(t, uvType, uvCount);
		if (normalCount) {
			t = addNormal(t, normalType, normalCount);
			t = addTangent(t, tangentCount);
			t = addBinormal(t, binormalCount);
		}
		return t;
	}
}; // VertexTypeUtil

enum class VertexSemanticType : u8 {
	None,
	POSITION,
	COLOR,
	TEXCOORD,
	NORMAL,
	TANGENT,
	BINORMAL,
};

#define VertexSemanticType_ENUM_LIST(E) \
	E(None)		\
	E(POSITION)	\
	E(COLOR)	\
	E(TEXCOORD)	\
	E(NORMAL)	\
	E(TANGENT)	\
	E(BINORMAL)	\
//----
SGE_ENUM_STR_UTIL(VertexSemanticType)

enum class VertexSemantic : u16; // forward declare
SGE_ENUM_ALL_OPERATOR(VertexSemantic)

struct VertexSemanticUtil {
	using Semantic = VertexSemantic;	// u16 (u8-Vertex_SemanticType, u8-Index)
	using Type = VertexSemanticType;	// u8
	using Index = u8;					// index 0~255, e.g. TEXCOORD255, COLOR0255
	
	// Vertex_SemanticType, Index -> Vertex_Semantic
	static constexpr Semantic make(Type type, Index index) {
		return static_cast<Semantic>((enumInt(type) << 8) | index);
	}

	static constexpr u16 _make(Type type, Index index) {
		return static_cast<u16>(make(type, index));
	}

	// Vertex_Semantic -> Vertex_SemanticType
	static constexpr Type	getType	(Semantic v)	{ return static_cast<Type>(enumInt(v) >> 8); }

	// Vertex_Semantic -> Index
	static constexpr Index	getIndex(Semantic v)	{ return static_cast<Index>(enumInt(v));	 }

}; // Vertex_SemanticUtil

enum class VertexSemantic : u16 { // VertexSemantic = VertexSemanticType|Index
	None = 0,

	POSITION	= VertexSemanticUtil::_make(VertexSemanticType::POSITION, 0),

	COLOR0		= VertexSemanticUtil::_make(VertexSemanticType::COLOR, 0),
	COLOR1		= VertexSemanticUtil::_make(VertexSemanticType::COLOR, 1),
	COLOR2		= VertexSemanticUtil::_make(VertexSemanticType::COLOR, 2),
	COLOR3		= VertexSemanticUtil::_make(VertexSemanticType::COLOR, 3),

	TEXCOORD0	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 0),
	TEXCOORD1	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 1),
	TEXCOORD2	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 2),
	TEXCOORD3	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 3),
	TEXCOORD4	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 4),
	TEXCOORD5	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 5),
	TEXCOORD6	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 6),
	TEXCOORD7	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 7),
	TEXCOORD8	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 8),
	TEXCOORD9	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 9),

	TEXCOORD10	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 10),
	TEXCOORD11	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 11),
	TEXCOORD12	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 12),
	TEXCOORD13	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 13),
	TEXCOORD14	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 14),
	TEXCOORD15	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 15),
	TEXCOORD16	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 16),
	TEXCOORD17	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 17),
	TEXCOORD18	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 18),
	TEXCOORD19	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 19),

	TEXCOORD20	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 20),
	TEXCOORD21	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 21),
	TEXCOORD22	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 22),
	TEXCOORD23	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 23),
	TEXCOORD24	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 24),
	TEXCOORD25	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 25),
	TEXCOORD26	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 26),
	TEXCOORD27	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 27),
	TEXCOORD28	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 28),
	TEXCOORD29	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 29),

	TEXCOORD30	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 30),
	TEXCOORD31	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 31),
	TEXCOORD32	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 32),
	TEXCOORD33	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 33),
	TEXCOORD34	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 34),
	TEXCOORD35	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 35),
	TEXCOORD36	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 36),
	TEXCOORD37	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 37),
	TEXCOORD38	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 38),
	TEXCOORD39	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 39),

	NORMAL		= VertexSemanticUtil::_make(VertexSemanticType::NORMAL,   0),
	TANGENT		= VertexSemanticUtil::_make(VertexSemanticType::TANGENT,  0),
	BINORMAL	= VertexSemanticUtil::_make(VertexSemanticType::BINORMAL, 0),
};

#define VertexSemantic_ENUM_LIST(E) \
	E(None)	\
	E(POSITION) \
	\
	E(COLOR0) \
	E(COLOR1) \
	E(COLOR2) \
	E(COLOR3) \
	\
	E(TEXCOORD0) \
	E(TEXCOORD1) \
	E(TEXCOORD2) \
	E(TEXCOORD3) \
	E(TEXCOORD4) \
	E(TEXCOORD5) \
	E(TEXCOORD6) \
	E(TEXCOORD7) \
	E(TEXCOORD8) \
	E(TEXCOORD9) \
	\
	E(TEXCOORD10) \
	E(TEXCOORD11) \
	E(TEXCOORD12) \
	E(TEXCOORD13) \
	E(TEXCOORD14) \
	E(TEXCOORD15) \
	E(TEXCOORD16) \
	E(TEXCOORD17) \
	E(TEXCOORD18) \
	E(TEXCOORD19) \
	\
	E(TEXCOORD20) \
	E(TEXCOORD21) \
	E(TEXCOORD22) \
	E(TEXCOORD23) \
	E(TEXCOORD24) \
	E(TEXCOORD25) \
	E(TEXCOORD26) \
	E(TEXCOORD27) \
	E(TEXCOORD28) \
	E(TEXCOORD29) \
	\
	E(TEXCOORD30) \
	E(TEXCOORD31) \
	E(TEXCOORD32) \
	E(TEXCOORD33) \
	E(TEXCOORD34) \
	E(TEXCOORD35) \
	E(TEXCOORD36) \
	E(TEXCOORD37) \
	E(TEXCOORD38) \
	E(TEXCOORD39) \
	\
	E(NORMAL)   \
	E(TANGENT)  \
	E(BINORMAL) \
//----
SGE_ENUM_STR_UTIL(VertexSemantic)

struct VertexLayout : public NonCopyable {
	using Semantic = VertexSemantic;
	using DataType = RenderDataType;

	struct Element {
		Semantic	semantic = Semantic::None;
		u16			offset = 0;
		DataType	dataType = DataType::None;
	};

	VertexType type = VertexType::None;
	size_t stride = 0;
	Vector_<Element, 16> elements;

	template<class VERTEX, class ATTR>
	void addElement(Semantic semantic, ATTR VERTEX::* attr) { // e.g. attr = &VertexT_xxx::VertexT_xxxMember
		if (std::is_array<ATTR>()) {
			size_t n = std::extent<ATTR>();
			for (size_t i = 0; i < n; i++) {
				_addElement(semantic, attr, i);
			}
		}
		else {
			_addElement(semantic, attr, 0);
		}
	}

private:
	template<class VERTEX, class ATTR>
	void _addElement(Semantic semantic, ATTR VERTEX::* attr, size_t index) {
		auto& o = elements.push_back();
		o.semantic = semantic;
		using A = std::remove_extent<ATTR>::type;
		o.dataType = RenderDataTypeUtil::get<A>();
		o.offset = static_cast<u16>(memberOffset(attr) + sizeof(A) * index);
	}
}; // VertexLayout

struct VertexBase {
	using Semantic = VertexSemantic;

	using PosType		= void;
	using ColorType		= void;
	using UvType		= void;
	using NormalType	= void;

	// set all type, count = None/0
	static const RenderDataType	kPosType	= RenderDataType::None;
	static const RenderDataType	kColorType	= RenderDataType::None;
	static const RenderDataType	kUvType		= RenderDataType::None;
	static const RenderDataType	kNormalType = RenderDataType::None;

	static const VertexType kType = VertexType::None;

	static const u8 kColorCount		= 0;
	static const u8 kUvCount		= 0;
	static const u8 kNormalCount	= 0;
	static const u8 kTangentCount	= 0;
	static const u8 kBinormalCount	= 0;

}; // VertexBase

template<class POS_TYPE> /* VertexT_Pos<Tuple3f> */
struct VertexT_Pos : public VertexBase
{
	using PosType = POS_TYPE;
	POS_TYPE pos;

	static const RenderDataType	kPosType = RenderDataTypeUtil::get<POS_TYPE>();
	static const VertexType kType = VertexTypeUtil::addPos(VertexBase::kType, kPosType);

	static const VertexLayout* layout() {
		static const VertexLayout* s = VertexLayoutManager::instance()->getLayout(kType);
		return s;
	}

	static void onRegister(VertexLayout* layout) {
		layout->addElement(Semantic::POSITION, &VertexT_Pos::pos);
	}
};

template<class COLOR_TYPE, u8 COLOR_COUNT, class BASE> /* VertexT_Color<Color4f, 1, VertexT_Pos> */
struct VertexT_Color : public BASE
{
	using ColorType = COLOR_TYPE;
	COLOR_TYPE	color[COLOR_COUNT];

	static const RenderDataType	kColorType = RenderDataTypeUtil::get<COLOR_TYPE>();
	static const u8 kColorCount = COLOR_COUNT;
	static const VertexType kType = VertexTypeUtil::addColor(BASE::kType, kColorType, kColorCount);

	static const VertexLayout* layout() {
		static const VertexLayout* s = VertexLayoutManager::instance()->getLayout(kType);
		return s;
	}

	static void onRegister(VertexLayout* layout) {
		BASE::onRegister(layout);
		layout->addElement(Semantic::COLOR0, &VertexT_Color::color);
	}
};

template<class UV_TYPE, u8 UV_COUNT, class BASE> /* VertexT_Uv<Tuple2f, 1, VertexT_Color> */
struct VertexT_Uv : public BASE
{
	using UvType = UV_TYPE;
	UV_TYPE		uv[UV_COUNT];

	static const RenderDataType	kUvType = RenderDataTypeUtil::get<UV_TYPE>();
	static const u8 kUvCount = UV_COUNT;
	static const VertexType kType = VertexTypeUtil::addUv(BASE::kType, kUvType, kUvCount);

	static const VertexLayout* layout() {
		static const VertexLayout* s = VertexLayoutManager::instance()->getLayout(kType);
		return s;
	}

	static void onRegister(VertexLayout* layout) {
		BASE::onRegister(layout);
		layout->addElement(Semantic::TEXCOORD0, &VertexT_Uv::uv);
	}
};

template<class NORMAL_TYPE, u8 NORMAL_COUNT, class BASE> /* VertexT_Normal<Tuple3f, 1, VertexT_Pos> */
struct VertexT_Normal : public BASE
{
	using NormalType = NORMAL_TYPE;
	NORMAL_TYPE		normal[NORMAL_COUNT];

	static const RenderDataType	kNormalType = RenderDataTypeUtil::get<NORMAL_TYPE>();
	static const u8 kNormalCount = NORMAL_COUNT;
	static const VertexType kType = VertexTypeUtil::addNormal(BASE::kType, kNormalType, kNormalCount);

	static const VertexLayout* layout() {
		static const VertexLayout* s = VertexLayoutManager::instance()->getLayout(kType);
		return s;
	}

	static void onRegister(VertexLayout* layout) {
		BASE::onRegister(layout);
		layout->addElement(Semantic::NORMAL, &VertexT_Normal::normal);
	}
};

template<class TANGENT_TYPE, u8 TANGENT_COUNT, class BASE> /* VertexT_Tangent<Tuple3f, 1, VertexT_Normal> */
struct VertexT_Tangent : public BASE
{
	using TangentType = TANGENT_TYPE;
	TANGENT_TYPE		tangent[TANGENT_COUNT];

	static const RenderDataType	kTangentType = RenderDataTypeUtil::get<TANGENT_TYPE>();
	static const u8 kTangentCount = TANGENT_COUNT;
	static const VertexType kType = VertexTypeUtil::addTangent(BASE::kType, kTangentCount); // kTangentType same as kNormalType

	static const VertexLayout* layout() {
		static const VertexLayout* s = VertexLayoutManager::instance()->getLayout(kType);
		return s;
	}

	static void onRegister(VertexLayout* layout) {
		static_assert(std::is_same<TangentType, NormalType>::value, "");

		BASE::onRegister(layout);
		layout->addElement(Semantic::TANGENT, &VertexT_Tangent::tangent);
	}
};

template<class BINORMAL_TYPE, u8 BINORMAL_COUNT, class BASE> /* VertexT_Binormal<Tuple3f, 1, VertexT_Tangent> */
struct VertexT_Binormal : public BASE
{
	using BinormalType = BINORMAL_TYPE;
	BINORMAL_TYPE		binormal[BINORMAL_COUNT];

	static const RenderDataType	kBinormalType = RenderDataTypeUtil::get<BINORMAL_TYPE>();
	static const u8 kBinormalCount = BINORMAL_COUNT;
	static const VertexType kType = VertexTypeUtil::addBinormal(BASE::kType, kBinormalCount); // kBinormalType same as kNormalType

	static const VertexLayout* layout() {
		static const VertexLayout* s = VertexLayoutManager::instance()->getLayout(kType);
		return s;
	}

	static void onRegister(VertexLayout* layout) {
		static_assert(std::is_same<BinormalType, NormalType>::value, "");

		BASE::onRegister(layout);
		layout->addElement(Semantic::BINORMAL, &VertexT_Binormal::binormal);
	}
};

using Vertex_Pos			= VertexT_Pos<Tuple3f>;
using Vertex_PosNormal		= VertexT_Normal<Tuple3f, 1, Vertex_Pos>;

using Vertex_PosColor		= VertexT_Color<Color4b, 1, Vertex_Pos>;
using Vertex_PosColorNormal	= VertexT_Normal<Tuple3f, 1, Vertex_PosColor>;

template<u8 UV_COUNT> using Vertex_PosUv				= VertexT_Uv<Tuple2f, UV_COUNT, Vertex_Pos>;
template<u8 UV_COUNT> using Vertex_PosColorUv			= VertexT_Uv<Tuple2f, UV_COUNT, Vertex_PosColor>;

template<u8 UV_COUNT> using Vertex_PosNormalUv			= VertexT_Normal<Tuple3f,   1, Vertex_PosUv<UV_COUNT>>;
template<u8 UV_COUNT> using Vertex_PosColorNormalUv		= VertexT_Normal<Tuple3f,   1, Vertex_PosColorUv<UV_COUNT>>;

template<u8 UV_COUNT> using Vertex_PosTangentUv			= VertexT_Tangent<Tuple3f,  1, Vertex_PosNormalUv<UV_COUNT>>;
template<u8 UV_COUNT> using Vertex_PosColorTangentUv	= VertexT_Tangent<Tuple3f,  1, Vertex_PosColorNormalUv<UV_COUNT>>;

template<u8 UV_COUNT> using Vertex_PosBinormalUv		= VertexT_Binormal<Tuple3f, 1, Vertex_PosTangentUv<UV_COUNT>>;
template<u8 UV_COUNT> using Vertex_PosColorBinormalUv	= VertexT_Binormal<Tuple3f, 1, Vertex_PosColorTangentUv<UV_COUNT>>;
} // namespace