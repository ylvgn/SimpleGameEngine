#if 0
Shader {
	Pass {
		VsFunc	vs_main
		PsFunc	ps_main
	}
}
#endif

struct VertexIn {
	float3 positionOS : POSITION;
	float4 color : COLOR;
};

struct PixelIn {	
	float4 positionHCS : SV_POSITION;
	float4 color : COLOR;
};

PixelIn vs_main(VertexIn i) {
    PixelIn o;

    o.positionHCS = float4(i.positionOS.xyz, 1.0);
    o.color = i.color;

    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET {
    return i.color;
}