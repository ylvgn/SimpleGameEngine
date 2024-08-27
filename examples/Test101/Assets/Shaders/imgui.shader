#if 0
Shader {
	Properties {
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

		DepthTest	Always
//		DepthWrite	false

		BlendRGB 	Add One OneMinusSrcAlpha
		BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

#include "sge_define.h"

struct VertexIn {
	VA_POS(float2);
	VA_COLOR(float4);
	VA_UV(float2);
};

struct PixelIn {
	PA_POS(float4);
	PA_COLOR(float4);
	PA_UV(float2);
};

float4x4 ProjectionMatrix; 

Texture2D    texture0;
SamplerState sampler0;

PixelIn vs_main(VertexIn i) {
	PixelIn o;
	o.positionHCS = mul(ProjectionMatrix, float4(i.positionOS.xy, 0.f, 1.f));
	o.color = i.color;
	o.uv  = i.uv;
	return o;
}

float4 ps_main(PixelIn i) : SV_TARGET {
	float4 o = i.color * texture0.Sample(sampler0, i.uv).r;
	return o;
}
