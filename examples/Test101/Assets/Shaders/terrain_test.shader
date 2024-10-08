#if 0
Shader {
	Properties {
		Texture2D mainTex
	}
	
	Pass {
		//Cull None
		DepthTest	LessEqual

		BlendRGB 	Add One OneMinusSrcAlpha
		BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

#include "sge_define.h"

struct VertexIn {
	VA_POS(float4);
};

struct PixelIn {
	PA_POS(float4);
};

float4x4	sge_matrix_model;
float4x4	sge_matrix_view;
float4x4	sge_matrix_proj;
float4x4	sge_matrix_mvp;

float3		sge_camera_pos;
float3		sge_light_pos;
float3		sge_light_dir;
float		sge_light_power;
float3		sge_light_color;

int2 patchIndex;
int patchCellsPerRow;
float2	patchSize;
float4 calcPositionOS(float4 positionOS) {
	//float padding = 0;
	float padding = 1; // debug
	positionOS.xz += patchIndex * (patchCellsPerRow + padding);
	positionOS.y += 10;
	return positionOS;
}

PixelIn vs_main(VertexIn i) {
	PixelIn o;
	float4 positionOS = calcPositionOS(i.positionOS);
	o.positionHCS = mul(sge_matrix_mvp, positionOS);
	return o;
}

float4 ps_main(PixelIn i) : SV_TARGET {
	int tmp = patchIndex.x + patchIndex.y;
	if (tmp % 2 == 0)
		return float4(0.8, 0.8, 0, 1);
	return float4(0.8, 0, 0, 1);
}
