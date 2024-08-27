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

#include "sge_define.h"

struct VertexIn {
	VA_POS(float4);
	VA_COLOR(float4);
};

struct PixelIn {	
	PA_POS(float4);
	PA_COLOR(float4);
};

PixelIn vs_main(VertexIn i)
{
    PixelIn o;

    o.positionHCS = i.positionOS;
    o.color = i.color;

    return o;
}


float4 ps_main(PixelIn i) : SV_TARGET
{
    return i.color;
}