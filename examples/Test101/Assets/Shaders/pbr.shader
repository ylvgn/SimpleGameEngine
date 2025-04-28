#if 0
Shader {
	Properties {
		Float	test  = 0.5
		Vec4f	test2 = {0,0,0,1}
		
		[DisplayName="Color Test"]
		Color4f	color = {1,1,1,1}
		
		Texture2D albedoMap
		Texture2D normalMap
		Texture2D metallicMap
		Texture2D roughnessMap
		Texture2D aoMap
	}
	
	Pass {
//		Queue	"Transparent"
//		Cull	None

		DepthTest	LessEqual

//		DepthTest	Always
		DepthWrite	true

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
	VA_NORMAL(float3);
	PA_UV0(float2);
};

struct PixelIn {
	PA_POS(float4);
	PA_WPOS(float4);
	PA_NORMAL(float3);
	PA_UV0(float2);
};

float4x4	sge_matrix_model;
float4x4	sge_matrix_view;
float4x4	sge_matrix_proj;
float4x4	sge_matrix_mvp;

float4x4	sge_matrix_it_model;

float3		sge_camera_pos;

// lights
static const int kLightCount = 1; // TODO
float3 my_light_positions[kLightCount];
float3 my_light_colors[kLightCount];

SGE_TEX2D(albedoMap)
SGE_TEX2D(normalMap)
SGE_TEX2D(metallicMap)
SGE_TEX2D(roughnessMap)
SGE_TEX2D(aoMap)

PixelIn vs_main(VertexIn i) {
	PixelIn o;
	o.positionWS  = mul(sge_matrix_model, i.positionOS);
	o.positionHCS = mul(sge_matrix_mvp,   i.positionOS);
	o.normal	  = normalize(mul( (float3x3)sge_matrix_it_model, i.normal) );
	o.uv		  = i.uv;
	return o;
}


#if 0
#pragma mark ========= Helper Functions ============
#endif
float3	Color_Linear_to_sRGB(float3 x) { return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719; }
float3	Color_sRGB_to_Linear(float3 x) { return x < 0.04045 ? x / 12.92 : -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864; }
float	sqr(float x) { return x*x; }

float3 calcNormalTS(float2 uv, float3 positionWS, float3 normalWS) {
// Gram-Schmidt process
    float3 tangentNormal = SGE_TEX2D_SAMPLE(normalMap, uv).xyz * 2.0 - 1.0;
    float3 Q1			 = ddx(positionWS); // dFdx(positionWS);
    float3 Q2			 = ddy(positionWS); // dFdy(positionWS);
    float2 st1			 = ddx(uv);			// dFdx(uv);
    float2 st2			 = ddy(uv);			// dFdy(uv);
    float3 N			 = normalize(normalWS);
    float3 T			 = normalize(Q1*st2.y - Q2*st1.y); // component s, t == x, y
    float3 B			 = -normalize(cross(N, T));
    float3x3 TBN		 = float3x3(T, B, N);
	return normalize(mul(TBN, tangentNormal));
}

#if 0
#pragma mark ========= (N)ormal (D)istribution (F)unctions - D(h) ============
#endif
float TrowbridgeReitzNormalDistribution(float NdotH, float roughness){
// Trowbridge-Reitz GGX
    float roughnessSqr = sqr(roughness);
    float distribution = sqr(NdotH) * (roughnessSqr - 1.0) + 1.0;
    return roughnessSqr / (SGE_PI * sqr(distribution));
}

#if 0
#pragma mark ========= (F)resnel Function - F(l,h)============
#endif
float SchlickFresnel(float VdotH) {
	float x = clamp(1.0 - VdotH, 0.0, 1.0);
	float x2 = x*x;
	return x2*x2*x;
}

float3 SchlickFresnelFunction(float3 specular, float LdotH) {
	return specular + (1 - specular) * SchlickFresnel(LdotH);
}

#if 0
#pragma mark ========= (G)eometry Function - G(l,v,h) ============
#endif
float SchlickBeckmanGeometricShadowingFunction (float NdotL, float NdotV, float roughness){
    float roughnessSqr = roughness*roughness;
    float k = roughnessSqr * 0.797884560802865;
    float SmithL = (NdotL)/ (NdotL * (1- k) + k);
    float SmithV = (NdotV)/ (NdotV * (1- k) + k);
	float Gs = SmithL * SmithV;
	return Gs;
}

struct Surface {
	float3 positionWS;
	float3 albedo;
	float3 normal;

	float  metallic; // 1 mental, 0 non-mental
	float  roughness;
    float  ao;

	float3 ambient;
	float3 specular;
	float3 diffuse;
};

Surface Surface_ctor(PixelIn i) {
	Surface s;

	s.positionWS = i.positionWS;
	s.normal     = calcNormalTS(i.uv, s.positionWS, i.normal);

	s.albedo     = Color_sRGB_to_Linear(SGE_TEX2D_SAMPLE(albedoMap, i.uv).rgb);
    s.metallic   = SGE_TEX2D_SAMPLE(metallicMap, i.uv).r;
	s.roughness  = SGE_TEX2D_SAMPLE(roughnessMap, i.uv).r;
	s.ao         = SGE_TEX2D_SAMPLE(aoMap, i.uv).r;
	
	s.ambient    = s.albedo * s.ao * 0.03;
	s.diffuse    = s.albedo * (1.0 - s.metallic); // if metal, diffuse color is black

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 of 0.04
    // and if it's a metal, use the albedo color as F0 (metallic workflow)
	float3 F0  = 0.04; // default for insulators
	s.specular = lerp(F0, s.albedo, s.metallic);

	return s;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
	Surface s = Surface_ctor(i);

	float3 viewDir = normalize(sge_camera_pos - s.positionWS);

	float3 Lo = 0;
	for (int i = 0; i < kLightCount; ++i)
	{
		float3 lightDir		 = my_light_positions[i] - s.positionWS; // point light TODO
		float  lightDistance = length(lightDir);
		lightDir			 = normalize(lightDir);
		float  attenuation	 = 1.0 / ( 1.0 * sqr(lightDistance)); 
		float3 radiance		 = attenuation * my_light_colors[i];

		float3 reflectDir	 = reflect(-lightDir, s.normal);
		float3 halfDir		 = normalize(viewDir - lightDir);

		float NdotL = max(0.0, dot( s.normal,	lightDir));
		float NdotH = max(0.0, dot( s.normal,	halfDir	));
		float NdotV = max(0.0, dot( s.normal,	viewDir	));
		float VdotH = max(0.0, dot( viewDir,	halfDir	));
		float LdotH = max(0.0, dot( lightDir,	halfDir	));
		float LdotV = max(0.0, dot( lightDir,	viewDir	)); 
		float RdotV = max(0.0, dot( reflectDir, viewDir ));

// Cook-Torrance BRDF: F(l, v) = F(l,h)*G(l,v,h)*D(h) / 4*(dot(N, L)*(dot(N, V)))
        float  D = TrowbridgeReitzNormalDistribution(NdotH, s.roughness);
		float3 F = SchlickFresnelFunction(s.specular, LdotH);
        float  G = SchlickBeckmanGeometricShadowingFunction(NdotL, NdotV, s.roughness);
		float3 specular = (D * F * G) / ((4.0 * (NdotL * NdotV)) + 0.0001);

		float3 kF = F;
		float3 kD = (1.0 - kF) * (1.0 - s.metallic); // Diffuse is only applied where light isn't reflected
		float3 diffuse = (kD * s.albedo) / SGE_PI; // Lambertian diffuss

		Lo += (diffuse + specular) * radiance * NdotL; 
	}
	
	float3 color = s.ambient + Lo;
//	return float4(1,0,0,1);
//	return float4(s.ambient, 1.0);
//	return float4(my_light_colors[0], 1.0);
	return float4(Color_Linear_to_sRGB(color), 1.0);
}