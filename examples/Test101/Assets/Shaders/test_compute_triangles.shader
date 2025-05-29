#if 0
Shader {
	Properties {
//		Color4f	color = {1,1,1,1}
	}

	Pass {
		CsFunc	cs_main
	}
}
#endif

#include "sge_define.h"

RWByteAddressBuffer outVertexBuffer : register(u0); // requires explicit 4-byte alignment

/*
struct CPUSideTriangles
{
	float	 pos[3];   // 12 bytes
	uint8_t  color[4]; // 4  bytes
};
*/
static const int kSizeofStruct = 16;

#define TEST_EXAMPLE_1 1
#define TEST_EXAMPLE_2 0

#if TEST_EXAMPLE_1
	static const int kVertexCount = 3;
#elif TEST_EXAMPLE_2
	static const int kVertexCount = 6;
#endif

uint toColorRGBAb(float4 color) {
	return (uint(color.a * 255) << 24) |
		   (uint(color.b * 255) << 16) |
           (uint(color.g * 255) << 8 ) |
		   (uint(color.r * 255)      );
}

struct CS_Input {
	uint3 t  : SV_DispatchThreadID;
	uint3 g  : SV_GroupID;
	uint3 gt : SV_GroupThreadID;
};

#if TEST_EXAMPLE_1
void writeOutput_example1(CS_Input i) {
	uint3 tid	 = i.t;
    float3 pos   = 0;
	float4 color = 0;

	uint byteOffset = tid.x * kSizeofStruct; // 16 bytes per vertex (12 + 4)

	if (tid.x == 0) {
        pos   = float3(-0.5f, 0, 0);
        color = float4(1, 0, 0, 1);
    }
    else if (tid.x == 1) {
        pos   = float3(0, 0.5f, 0.0);
        color = float4(0, 1, 0, 1);
    }
    else if (tid.x == 2) {
        pos   = float3(0.5, 0, 0);
        color = float4(0, 0, 1, 1);
    }

	outVertexBuffer.Store3(byteOffset, asuint(pos)); // Works if byteOffset % 4 == 0
    outVertexBuffer.Store (byteOffset + 12, toColorRGBAb(color));
}
#endif // TEST_EXAMPLE_1

#if TEST_EXAMPLE_2
// --------------------- writeOutput_example2 start-------------------
float cellSize;
int2  gridSize;

static float  d		  = 1; // 0.5f
static float3 kOffset = 0; // float3(-0.25f, -0.25f, 0);
static const float3 kTriVertex[6] = {
	float3(0, 0, 0) + kOffset
  , float3(0, d, 0) + kOffset
  , float3(d, 0, 0) + kOffset
  , float3(d, 0, 0) + kOffset
  , float3(0, d, 0) + kOffset
  , float3(d, d, 0) + kOffset
};
void writeOutput_example2(CS_Input i) {
	uint3 tid	= i.t;
	float3 v	= kTriVertex[tid.x];

	float3 pos  = (v + i.g) * cellSize;

	uint x = tid.x;
	uint y = tid.y * 6;

	int index = (x + y * gridSize.x);
	const int byteOffset = index * kSizeofStruct;

	float4 color = float4(1, 0, 1, 1);

	outVertexBuffer.Store3(byteOffset, asuint(pos));
	outVertexBuffer.Store (byteOffset + 12, toColorRGBAb(color));
}
#endif // TEST_EXAMPLE_2

[numthreads(kVertexCount, 1, 1)] 
void cs_main(CS_Input i) {
#if TEST_EXAMPLE_1
	writeOutput_example1(i);
#elif TEST_EXAMPLE_2
	writeOutput_example2(i);
#endif
}
