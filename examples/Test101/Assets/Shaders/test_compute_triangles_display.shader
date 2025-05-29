#if 0
Shader {
	Properties {
	}

	Pass {
		//Queue		"Transparent"
		Cull		None
		Wireframe	true

		BlendRGB 	Add One OneMinusSrcAlpha
		BlendAlpha	Add One OneMinusSrcAlpha

		DepthTest	Less

		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

#include "sge_define.h"

float4 color;

struct VS_Input {
	VA_POS(float3);
	VA_COLOR(float4);
};

struct PS_Input {
	PA_POS(float4);
	PA_COLOR(float4);
};

PS_Input vs_main(VS_Input i) {
	PS_Input o;
	o.positionHCS = float4(i.positionOS.xyz, 1);
	o.color = i.color;	
	return o;
}

float4 ps_main(PS_Input i) : SV_Target {
//	return color;
	return color * i.color;
}
