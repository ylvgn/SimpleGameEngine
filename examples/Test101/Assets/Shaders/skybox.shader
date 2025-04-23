#if 0
Shader {
	Properties {
	}
	
	Pass {
// 		Queue	"Transparent"
		Cull	Front

//		DepthTest	Always
		DepthWrite	false

//		BlendRGB 	Add One OneMinusSrcAlpha
//		BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

#include "sge_define.h"

struct VertexIn {
	VA_POS(float3);
	VA_NORMAL(float3);
};

struct PixelIn {
	PA_POS(float4);
	PA_WPOS(float3);
	PA_NORMAL(float3);
};

float4x4	sge_matrix_model;
float4x4	sge_matrix_view;
float4x4	sge_matrix_proj;
float4x4	sge_matrix_mvp;

float3		sge_camera_pos;

TextureCube  skyboxMap;
SamplerState skyboxMap_Sampler;

PixelIn vs_main(VertexIn i) {
	PixelIn o;
	float4 positionOS = float4(i.positionOS, 1.0);

	o.positionWS  = mul(sge_matrix_model, positionOS)
	o.positionHCS = mul(sge_matrix_mvp,  positionOS);
	o.normal      = i.normal;
	return o;
}

float4 ps_main(PixelIn i) : SV_TARGET {
    float3 viewDir = normalize(i.positionWS - sge_camera_pos);
    float3 reflectDir = reflect(viewDir, normalize(i.normal));

	float4 o = skyboxMap.Sample(skyboxMap_Sampler, reflectDir);
	return o;
}