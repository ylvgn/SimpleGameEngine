//!!<-------- do not use underline '_' to define shader attribute variable name

#define VA_POS(T) 		T positionOS : POSITION

#define VA_COLOR(T) 	T color : COLOR
#define VA_COLOR0(T) 	VACOL(T)
#define VA_COLOR1(T) 	T color1 : COLOR1
#define VA_COLOR2(T) 	T color2 : COLOR2
#define VA_COLOR3(T) 	T color3 : COLOR3

#define VA_UV(T) 		T uv : TEXCOORD0
#define VA_UV0(T) 		VA_UV(T)
#define VA_UV1(T) 		T uv1 : TEXCOORD1
#define VA_UV2(T) 		T uv2 : TEXCOORD2
#define VA_UV3(T) 		T uv3 : TEXCOORD3
#define VA_UV4(T) 		T uv4 : TEXCOORD4
#define VA_UV5(T) 		T uv5 : TEXCOORD5
#define VA_UV6(T) 		T uv6 : TEXCOORD6
#define VA_UV7(T) 		T uv7 : TEXCOORD7
#define VA_UV8(T) 		T uv8 : TEXCOORD8

#define VA_NORMAL(T) 	T normal : NORMAL
#define VA_NORMAL0(T) 	VA_NORMAL(T)
#define VA_TANGENT(T) 	T tangent : TANGENT
#define VA_TANGENT0(T) 	VA_TANGENT(T)
#define VA_BINORMAL(T) 	T binormal : BINORMAL
#define VA_BINORMAL0(T) VA_BINORMAL(T)

#define PA_POS(T) 		T positionHCS : SV_POSITION // HCS: homogeneous clip space

//!!<------------ need increase TEXCOORD10~TEXCOORD13 when TEXCOORD0~9 may not enough to use
#define PA_WPOS(T) 		T positionWS : TEXCOORD10  	// world space
#define PA_VPOS(T) 		T positionVS : TEXCOORD11  	// view space
#define PA_CPOS(T) 		T positionCS : TEXCOORD12  	// clip space
#define PA_SPOS(T) 		T positionSS : TEXCOORD13  	// screen space

#define PA_COLOR(T) 	VA_COLOR(T)
#define PA_COLOR0(T) 	VA_COLOR0(T)
#define PA_COLOR1(T) 	VA_COLOR1(T)
#define PA_COLOR2(T) 	VA_COLOR2(T)
#define PA_COLOR3(T) 	VA_COLOR3(T)

#define PA_UV(T) 		VA_UV(T)
#define PA_UV0(T) 		VA_UV0(T)
#define PA_UV1(T) 		VA_UV1(T)
#define PA_UV2(T) 		VA_UV2(T)
#define PA_UV3(T) 		VA_UV3(T)
#define PA_UV4(T) 		VA_UV4(T)
#define PA_UV5(T) 		VA_UV5(T)
#define PA_UV6(T) 		VA_UV6(T)
#define PA_UV7(T) 		VA_UV7(T)
#define PA_UV8(T) 		VA_UV8(T)

#define PA_NORMAL(T) 	VA_NORMAL(T)
#define PA_NORMAL0(T) 	VA_NORMAL0(T)
#define PA_TANGENT(T) 	VA_TANGENT(T)
#define PA_TANGENT0(T) 	VA_TANGENT0(T)
#define PA_BINORMAL(T) 	VA_BINORMAL(T)
#define PA_BINORMAL0(T) VA_BINORMAL0(T)

// ----- constant
static const float SGE_PI = 3.1415926535;


// --------- macro
#define SGE_TEX2D(T) \
Texture2D T; \
SamplerState T##_Sampler; \
// -----

#define SGE_TEX2D_SAMPLE(T, UV) T.Sample(T##_Sampler, UV)