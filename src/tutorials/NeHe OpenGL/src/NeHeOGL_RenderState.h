#pragma once

namespace sge {

#define NeHeOGL_RenderState_Cull_ENUM_LIST(E) \
	E(None,)	\
	E(Back,)	\
	E(Front,)	\
//----
SGE_ENUM_CLASS(NeHeOGL_RenderState_Cull, u8)

#define NeHeOGL_RenderState_DepthTestFunc_ENUM_LIST(E) \
	E(Less,)		\
	E(LessEqual,)	\
	E(Equal,)		\
	E(Greater,)		\
	E(GreaterEqual,)\
	E(NotEqual,)	\
	E(Always,)		\
	E(Never,)		\
//----
SGE_ENUM_CLASS(NeHeOGL_RenderState_DepthTestFunc, u8)

class NeHeOGL_RenderState {
public:
	using Cull			= NeHeOGL_RenderState_Cull;
	using DepthTestOp	= NeHeOGL_RenderState_DepthTestFunc;

	bool wireframe	= false;
	Cull cull		= Cull::Back;

	float lineWidth	= 1.f;

	struct DepthTest {
		
		DepthTest()
			: writeMask(true)
			, op(DepthTestOp::Less)
		{}

		bool isEnable() { return op != DepthTestOp::Always; }

		DepthTestOp op;
		bool writeMask : 1;
	};
	DepthTest	depthTest;
};

}