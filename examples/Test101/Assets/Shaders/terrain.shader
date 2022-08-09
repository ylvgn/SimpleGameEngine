#if 0
Shader {
	Properties {
		Texture2D mainTex
	}
	
	Pass {
		Cull None
		DepthTest	LessEqual

		BlendRGB 	Add One OneMinusSrcAlpha
		BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

struct VertexIn {
	float4 positionOS : POSITION;
};

struct PixelIn {
	float4 positionHCS : SV_POSITION;
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

int patchIndex;
int patchMaxLod;
int patchLod;
float4 calcPositionOS(float4 positionOS) {
	int rowCount = (1 << patchMaxLod) + 1;
	float size = rowCount - 1;
	float2 dir = float2(patchIndex / rowCount, patchIndex % rowCount);

	positionOS.x += ceil(dir.x) * size;
	positionOS.z += ceil(dir.y) * size;

	return positionOS;
}

PixelIn vs_main(VertexIn i) {
	PixelIn o;
	float4 positionOS = calcPositionOS(i.positionOS);
	o.positionHCS = mul(sge_matrix_mvp, positionOS);
	return o;
}

float4 ps_main(PixelIn i) : SV_TARGET {
	if (patchIndex % 2 == 0)
		return float4(0.8, 0.8, 0, 1);
	return float4(0.8, 0, 0, 1);
}
