#if 0
Shader {
	Properties {
		Float	test  = 0.5
		Vec4f	test2 = {0,0,0,1}
		
		[DisplayName="Color Test"]
		Color4f	color = {1,1,1,1}
		
		Texture2D mainTex
	}
	
	Pass {
		// Queue	"Transparent"
//		Cull		None

		DepthTest	LessEqual

//		DepthTest	Always
//		DepthWrite	false

		BlendRGB 	Add One OneMinusSrcAlpha
		BlendAlpha	Add One OneMinusSrcAlpha
		
//		Wireframe	true // debug camera culling
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

#include "sge_define.h"

struct VertexIn {
	VA_POS(float4);
	VA_COLOR(float4);
	VA_UV(float2);
	VA_NORMAL(float3);
};

struct PixelIn {
	PA_POS(float4);
	PA_WPOS(float4);
	PA_UV(float2);
	PA_COLOR(float4);
	PA_NORMAL(float3);
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

float  test_float;
float4 test_color;

Texture2D mainTex;
SamplerState mainTex_Sampler;

PixelIn vs_main(VertexIn i) {
	PixelIn o;
	o.positionWS  = mul(sge_matrix_model, i.positionOS);
	o.positionHCS = mul(sge_matrix_mvp,   i.positionOS);
	o.positionHCS.y += test_float;
	
	o.uv     = i.uv;
	o.color	 = i.color;
	o.normal = i.normal;
	return o;
}

struct Surface {
	float3 positionWS;
	float3 normal;	
	float3 color;
	float3 ambient;
	float3 diffuse;
	float3 specular;
	float  shininess;
};

float3 Color_Linear_to_sRGB(float3 x) { return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719; }
float3 Color_sRGB_to_Linear(float3 x) { return x < 0.04045 ? x / 12.92 : -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864; }

float3 lighting_blinn_phong(Surface s) {
	float3 normal   = normalize(s.normal);
	float3 lightDir = s.positionWS - sge_light_pos;
	float  lightDistance = length(sge_light_dir);
	lightDir = normalize(lightDir);

	float lambertian = max(dot(normal, -sge_light_dir), 0);
	float specularPower = 0;

	if (lambertian > 0) {
		float3 viewDir = normalize(sge_camera_pos - s.positionWS);

		if (1) { // blinn-phong
			float3 halfDir = normalize(viewDir - lightDir);
			float specAngle = saturate(dot(halfDir, normal));
			specularPower = pow(specAngle, s.shininess);

		} else { // phong
			float3 reflectDir = reflect(-lightDir, normal);
			float specAngle = max(dot(reflectDir, viewDir), 0);
			specularPower = pow(specAngle, s.shininess / 4.0);
		}
	}

	float3 outLightColor = sge_light_color * sge_light_power / (lightDistance * lightDistance);

	float3 outAmbient  = s.ambient;
	float3 outDiffuse  = s.diffuse  * lambertian    * outLightColor;
	float3 outSpecular = s.specular * specularPower * outLightColor;

	float3 outColor = (outAmbient + outDiffuse + outSpecular) * s.color;
	return outColor;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
//	return float4(i.positionHCS.w * 0.05, 0, 0, 1);
//	return float4(i.normal, 1);
//	return i.color * test_color;
//	return float4(i.uv.x, i.uv.y, 0, 1);

	Surface s;
	s.positionWS = i.positionWS;
	s.normal     = i.normal;
	s.color	     = i.color * test_color;
	s.specular   = float3(0.8, 0.8, 0.8);
	s.ambient    = float3(0.2, 0.2, 0.2);
	s.diffuse	 = float3(1, 1, 1);
	s.shininess	 = 1;
	
	float4 texCol = mainTex.Sample(mainTex_Sampler, i.uv);
//	return texCol;
	float3 color = lighting_blinn_phong(s) * texCol;
	return float4(Color_Linear_to_sRGB(color), 1);
}
