#pragma once

namespace sge {

#define RenderState_Cull_ENUM_LIST(E) \
	E(None,) /* FrontAndBack*/ \
	E(Back,) \
	E(Front,) \
//----
SGE_ENUM_CLASS(RenderState_Cull, u8)

#define RenderState_DepthTestOp_ENUM_LIST(E) \
	E(Less,) \
	E(Equal,) \
	E(LessEqual,) \
	E(Greater,) \
	E(GreaterEqual,) \
	E(NotEqual,) \
	E(Always,) \
	E(Never,) \
	E(_END,) \
//-----
SGE_ENUM_CLASS(RenderState_DepthTestOp, u8) /* depth & stencil */

#define RenderState_BlendOp_ENUM_LIST(E) \
	E(Disable,) \
	E(Add,) \
	E(Sub,) \
	E(RevSub,) \
	E(Min,) \
	E(Max,) \
	E(_END,) \
//-----
SGE_ENUM_CLASS(RenderState_BlendOp, u8)

#define RenderState_BlendFactor_ENUM_LIST(E) \
	E(Zero,) \
	E(One,) \
	E(SrcColor,) \
	E(DstColor,) \
	E(SrcAlpha,) \
	E(DstAlpha,) \
	E(ConstColor,) \
/*	E(ConstAlpha,) */ \
	E(OneMinusSrcColor,) \
	E(OneMinusDstColor,) \
	E(OneMinusSrcAlpha,) \
	E(OneMinusDstAlpha,) \
	E(OneMinusConstColor,) \
/*	E(OneMinusConstAlpha,) */\
	E(SrcAlphaSaturate,) \
	E(_END,) \
//-----
SGE_ENUM_CLASS(RenderState_BlendFactor, u8)

#define RenderState_StencilTestOp_ENUM_LIST(E) \
	E(Keep,) \
	E(Zero,) \
	E(Replace,) \
	E(Increment,) \
	E(IncrementWrap,) \
	E(Decrement,) \
	E(DecrementWrap,) \
	E(Invert,) \
	E(_END,) \
//-----
SGE_ENUM_CLASS(RenderState_StencilTestOp, u8)

class RenderState {
public:
	using Cull			= RenderState_Cull;
	using DepthTestOp	= RenderState_DepthTestOp;
	using StencilTestOp = RenderState_StencilTestOp;
	using BlendOp		= RenderState_BlendOp;
	using BlendFactor	= RenderState_BlendFactor;

	bool	wireframe = false;
	Cull	cull	  = Cull::Back;

	struct DepthTest {
		DepthTestOp	op = DepthTestOp::LessEqual;
		bool writeMask = true;

		SGE_INLINE bool operator==(const DepthTest& r) const {
			return op == r.op && writeMask == r.writeMask;
		}

		bool isEnable() const { return op != DepthTestOp::Always; }

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, op);
			SGE_NAMED_IO(se, writeMask);
		}
	};
	DepthTest	depthTest;

	struct StencilTest {
		Cull			cull = Cull::None;

		DepthTestOp		op   = DepthTestOp::Always;
		u8				ref	 = 0;
		u8				mask = 0xff;

		StencilTestOp	sfail = StencilTestOp::Keep;
		StencilTestOp	dfail = StencilTestOp::Keep;
		StencilTestOp	bfail = StencilTestOp::Keep;

		bool isEnable() const { return op != DepthTestOp::Always; }

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, cull);
			SGE_NAMED_IO(se, ref);
			SGE_NAMED_IO(se, mask);
			SGE_NAMED_IO(se, sfail);
			SGE_NAMED_IO(se, dfail);
			SGE_NAMED_IO(se, bfail);
		}
	};
	StencilTest stencilTest;

	struct BlendFunc {
		BlendOp		op		  = BlendOp::Disable;
		BlendFactor	srcFactor = BlendFactor::SrcAlpha;
		BlendFactor	dstFactor = BlendFactor::OneMinusSrcAlpha;

		void set(BlendOp op_, BlendFactor srcFactor_, BlendFactor dstFactor_) {
			op = op_;
			srcFactor = srcFactor_;
			dstFactor = dstFactor_;
		}

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, op);
			SGE_NAMED_IO(se, srcFactor);
			SGE_NAMED_IO(se, dstFactor);
		}
	};

	struct Blend {
		BlendFunc	rgb;
		BlendFunc	alpha;
		Color4f		constColor{1,1,1,1};

		bool isEnable() const { return rgb.op != BlendOp::Disable || alpha.op != BlendOp::Disable; }

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, rgb);
			SGE_NAMED_IO(se, alpha);
			SGE_NAMED_IO(se, constColor);
		}
	};
	Blend blend;

	template<class SE>
	void onJson(SE& se) {
		SGE_NAMED_IO(se, wireframe);
		SGE_NAMED_IO(se, cull);
		SGE_NAMED_IO(se, depthTest);
		SGE_NAMED_IO(se, stencilTest);
		SGE_NAMED_IO(se, blend);
	}
};

} // namespace sge