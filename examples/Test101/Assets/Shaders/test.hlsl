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

#include "define.h"
	
struct VOut {	
	VA_POS(float4);
	VA_COLOR(float4);
};

VOut vs_main(VA_POS(float4), VA_COLOR(float))
{
    VOut output;

    output.positionOS = positionOS;
    output.color = color;

    return output;
}


float4 ps_main(PA_POS(float4), PA_COLOR(float)) : SV_TARGET
{
    return color;
}