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
float3		sge_light_pos;
float3		sge_light_dir;
float		sge_light_power;
float3		sge_light_color;

// lights
static const int kLightCount = 4;
#if 0
static const float d = 5;
static const float3 my_light_positions[kLightCount] = {
	float3(d, 0, 0),
	float3(0, d, 0),
	float3(0, 0, d),
	float3(d, d, d),
};
#else
	float3 my_light_positions[kLightCount];
#endif

#if 0
static const float3 my_light_colors[kLightCount] = {
	float3(1,   0,   0),
	float3(0,   1,   0),
	float3(0,   0,   1),
	float3(0.1, 0.1, 0.1),
};
#else
	float3 my_light_colors[kLightCount];
#endif

// textures
SGE_TEX2D(albedoMap)
SGE_TEX2D(normalMap)
SGE_TEX2D(metallicMap)
SGE_TEX2D(roughnessMap)
SGE_TEX2D(aoMap)

// unit test
#define SGE_TEST_A 0 // Light Attenuation
#define SGE_TEST_D 0 // Normal Distribution
#define SGE_TEST_G 0 // Geometry Distribution
#define SGE_TEST_F 0 // Fresnel Distribution

PixelIn vs_main(VertexIn i) {
	PixelIn o;
	o.positionWS  = mul(sge_matrix_model, i.positionOS);
	o.positionHCS = mul(sge_matrix_mvp,   i.positionOS);
	o.normal	  = normalize(mul( (float3x3)sge_matrix_it_model, i.normal) );
	o.uv		  = i.uv;
	return o;
}

int my_switch_impl;

#if 0
#pragma mark ========= Helper Functions ============
#endif
float3	Color_Linear_to_sRGB(float3 x) { return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719; }
float3	Color_sRGB_to_Linear(float3 x) { return x < 0.04045 ? x / 12.92 : -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864; }
float	Sqr(float x) { return x*x; }

float3 CalcNormalTS(float2 uv, float3 positionWS, float3 normalWS) {
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
float TrowbridgeReitzNormalDistribution(float NdotH, float roughness) {
// Trowbridge-Reitz GGX
    float roughnessSqr = Sqr(roughness);
    float denom = max( Sqr(NdotH) * (roughnessSqr - 1.0) + 1.0, SGE_EPSILON);
    return roughnessSqr / (SGE_PI * Sqr(denom));
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
#pragma mark ========= (G)eometry Shadowing Function - G(l,v,h) ============
#endif
float SchlickGGX(float NdotX, float roughness)
{
    float roughnessSqr = Sqr(roughness);
    float k = roughnessSqr / 2.0;
    
    float denom = NdotX * (1.0 - k) + k;
    return NdotX / max(denom, SGE_EPSILON);
}

float SchlickGGX_UE4(float NdotX, float roughness)
{
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0; // UE4 remapping
    float denom = NdotX * (1.0 - k) + k;
    return NdotX / max(denom, SGE_EPSILON);
}

float SmithGGXGeometricShadowingFunction(float NdotV, float NdotL, float roughness)
{
#if 0
    float ggxV = SchlickGGX(NdotV, roughness);
    float ggxL = SchlickGGX(NdotL, roughness);
#else
	float ggxV = SchlickGGX_UE4(NdotV, roughness);
    float ggxL = SchlickGGX_UE4(NdotL, roughness);
#endif
    return ggxV * ggxL;
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
	s.normal     = CalcNormalTS(i.uv, s.positionWS, i.normal);

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

#if SGE_TEST_F
	s.specular = F0;
#endif
	return s;
}

// ---------------------------------------------------------------------------- Learn OpenGL start
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = SGE_PI * denom * denom;

    return nom / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}
float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ---------------------------------------------------------------------------- Learn OpenGL end

float4 ps_main(PixelIn input) : SV_TARGET
{
	static const float kLightPower = 10.0;

	Surface s = Surface_ctor(input);

	float3 viewDir = normalize(sge_camera_pos - s.positionWS);

	float3 Lo = 0;

#if 1
	for (int i = 0; i < kLightCount; ++i)
#else
	int i = 0; // just test single point light
#endif
	{
		if (my_switch_impl == 1) {
			float3 lightDir		 = s.positionWS - my_light_positions[i]; // TODO: point light only
			//float  lightDistance = length(lightDir);
			float lightDistance  = length(my_light_positions[i] - s.positionWS);

			lightDir			 = normalize(lightDir);
			float  attenuation	 = 1.0 / ( max(Sqr(lightDistance), SGE_EPSILON) ); 
			float3 radiance		 = my_light_colors[i] * kLightPower * attenuation;
#if SGE_TEST_A
		return float4(radiance, 1.0);
#endif
			float3 reflectDir	 = reflect(-lightDir, s.normal);
			float3 halfDir		 = normalize(viewDir -lightDir);

			float NdotL = max(0.0, dot( s.normal,	-lightDir));
			float NdotH = max(0.0, dot( s.normal,	halfDir	));
			float NdotV = max(0.0, dot( s.normal,	viewDir	));
			float VdotH = max(0.0, dot( viewDir,	halfDir	));
			float LdotH = max(0.0, dot( -lightDir,	halfDir	));
			float LdotV = max(0.0, dot( -lightDir,	viewDir	)); 
			float RdotV = max(0.0, dot( reflectDir, viewDir ));

			// Cook-Torrance BRDF: F(l, v) = F(l,h)*G(l,v,h)*D(h) / 4*(dot(N, L)*(dot(N, V)))
			float  D = TrowbridgeReitzNormalDistribution(NdotH, s.roughness);
			float  G = SmithGGXGeometricShadowingFunction(NdotV, NdotL, s.roughness);
			float3 F = SchlickFresnelFunction(s.specular, LdotH);
			float3 specular = (D * F * G) / ( max((4.0 * (NdotL * NdotV)), SGE_EPSILON));

#if SGE_TEST_D
		return float4(0,D,0,1);
#endif
#if SGE_TEST_G
		return float4(0,0,G,1);
#endif
#if SGE_TEST_F
		return float4(F,1);
#endif
			float3 kS = F;
			float3 kD = (1.0 - kS) * (1.0 - s.metallic); // diffuse is only applied where light isn't reflected

			float3 diffuse = (kD * s.diffuse) / SGE_PI;
			Lo += (diffuse + specular) * radiance * NdotL;
		}
		else { // Learn Opengl Impl: https://learnopengl.com/code_viewer_gh.php?code=src/6.pbr/1.2.lighting_textured/1.2.pbr.fs
			float3 V		 = viewDir;
			float3 N		 = s.normal;
			float3 WorldPos  = s.positionWS;
			float3 albedo    = s.albedo;
			float3 F0        = s.specular;
			float  roughness = s.roughness;
			float  metallic  = s.metallic;

			// calculate per-light radiance
			float3 L = normalize(my_light_positions[i] - WorldPos);
			float3 H = normalize(V + L);

			float	distance	= length(my_light_positions[i] - WorldPos);
			float	attenuation = 1.0 / (distance * distance);
			float3	radiance	= my_light_colors[i] * kLightPower * attenuation;
#if SGE_TEST_A
		return float4(radiance, 1.0);
#endif
			// Cook-Torrance BRDF
			float  NDF = DistributionGGX(N, H, roughness);   
			float  G   = GeometrySmith(N, V, L, roughness);      
			float3 F   = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
#if SGE_TEST_D
		return float4(0,NDF,0,1);
#endif
#if SGE_TEST_G
		return float4(0,0,G,1);
#endif
#if SGE_TEST_F
		return float4(F,1);
#endif

			float3 numerator    = NDF * G * F;
			float  denominator  = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
			float3 specular = numerator / denominator;

			// kS is equal to Fresnel
			float3 kS = F;
			// for energy conservation, the diffuse and specular light can't
			// be above 1.0 (unless the surface emits light); to preserve this
			// relationship the diffuse component (kD) should equal 1.0 - kS.
			float3 kD = 1.0 - kS;

			// multiply kD by the inverse metalness such that only non-metals 
			// have diffuse lighting, or a linear blend if partly metal (pure metals have no diffuse light).
			kD *= 1.0 - metallic;

			float NdotL = max(dot(N, L), 0.0);
			Lo += (kD * albedo / SGE_PI + specular) * radiance * NdotL;
		}
	}

	float3 color = s.ambient + Lo;
//	return float4(1,0,0,1);
//	return float4(s.ambient, 1.0);
//	return float4(my_light_colors[2], 1.0);

	color = color / (color + 1.0); // HDR tonemapping
	return float4(Color_Linear_to_sRGB(color), 1.0); // gamma correction
}