#if 0
Shader {
	Properties {
	}

	Pass {
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

float4x4	sge_matrix_model;
float4x4	sge_matrix_view;
float4x4	sge_matrix_proj;
float4x4	sge_matrix_mvp;

//sampler texture0_Sampler;
SamplerState 	texture0_Sampler;
Texture2D 		texture0;

struct VertexIn
{
	float2 positionOS : POSITION;
	float4 color : COLOR0;
	float2 uv  : TEXCOORD0;
};

struct PixelIn
{
	float4 positionHCS : SV_POSITION;
	float4 color : COLOR0;
	float2 uv  : TEXCOORD0;
};

PixelIn vs_main(VertexIn i)
{
	PixelIn o;
	o.positionHCS = mul(sge_matrix_proj, float4(i.positionOS, 0.f, 1.f));
	o.color = i.color;
	o.uv = i.uv;
	return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    float4 o = i.color * texture0.Sample(texture0_Sampler, i.uv);
    return o;
}