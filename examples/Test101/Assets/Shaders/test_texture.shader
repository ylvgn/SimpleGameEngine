#if 0
Shader {
	Properties {	
		[DisplayName="Color Test"]
		Color4f	color = {1,1,1,1}
	}
	
	Pass {
		// Queue	"Transparent"
		// Cull		None

		// BlendRGB 	Add One OneMinusSrcAlpha
		// BlendAlpha	Add One OneMinusSrcAlpha

		// DepthTest	Always
		// DepthWrite	false
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

#include "sge_define.h"

struct VertexIn {
	VA_POS(float4);
	VA_UV(float2);
	VA_COLOR(float4);
};

struct PixelIn {
	PA_POS(float4);
	PA_UV(float2);
	PA_COLOR(float4);
};

//float4x4	SGE_MVP;

float  test_float;
float4 test_color;

Texture2D 		mainTex;
SamplerState 	mainTex_Sampler;

PixelIn vs_main(VertexIn i) {
    PixelIn o;
    o.positionHCS = i.positionOS;
	o.positionHCS.y += test_float;

    o.color = i.color;
	o.uv = i.uv;
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
	float4 texCol = mainTex.Sample(mainTex_Sampler, i.uv);
	return texCol * test_color;
}
