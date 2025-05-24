#pragma once

#include <sge_core.h>
#include "RenderDataType.h"
#include "shader/RenderState.h"

namespace sge {

class Material;
class MaterialPass;

class Shader;
class ShaderPass;

class Texture;
class Texture1D;
class Texture2D;
class Texture3D;
class TextureCube;
class SamplerState;

class Renderer;
class RenderBuiltInAssets;
class RenderContext;
class RenderContext_CreateDesc;

class RenderDataTypeUtil;

class RenderGpuQuery;
class RenderGpuQuery_CreateDesc;
class RenderGpuBuffer;
class RenderGpuBuffer_CreateDesc;
class RenderGpuStorageBuffer;
class RenderGpuStorageBuffer_CreateDesc;
class RenderMesh;
class RenderMultiGpuBuffer;
class RenderRequest;

class RenderStageBuffer;
class RenderStageBuffer_CreateDesc;
class RenderStorageBuffer;
class RenderStorageBuffer_CreateDesc;

class RenderState;
class RenderSubMesh;

class RenderVertexArray;
class RenderVertexDesc;
class RenderIndexArray;

class RenderCommonBase {
public:
	using BlendOp			= RenderState_BlendOp;
	using BlendFactor		= RenderState_BlendFactor;
	using BuiltInAssets		= RenderBuiltInAssets;

	using ColorType			= ColorType;

	using DataType			= RenderDataType;
	using DataTypeUtil		= RenderDataTypeUtil;
	using DepthTestOp		= RenderState_DepthTestOp;

	using GpuBuffer			= RenderGpuBuffer;

	using PrimitiveType 	= RenderPrimitiveType;

	using Rect2				= Rect2f;
	using RenderContext		= RenderContext;
	using RenderState		= RenderState;

	using Mat4				= Mat4f;
	using Material			= Material;
	using MaterialPass		= MaterialPass;
	using Mesh				= RenderMesh;
	using MultiGpuBuffer	= RenderMultiGpuBuffer;

	using SamplerState		= SamplerState;

	using Shader			= Shader;
	using ShaderPass		= ShaderPass;
	using SubMesh			= RenderSubMesh;

	using Texture			= Texture;
	using Texture1D			= Texture1D;
	using Texture2D			= Texture2D;
	using Texture3D			= Texture3D;
	using TextureCube		= TextureCube;

	using Vec2				= Vec2f;
	using Vec3				= Vec3f;
	using Vec4				= Vec4f;
};
SGE_STATIC_ASSERT_NO_MEMBER_CLASS(RenderCommonBase);

struct RenderAdapterInfo {
	String	adapterName;
	i64		memorySize = 0;

	bool	multithread		 = false;
	bool	hasComputeShader = false;
	bool	shaderHasFloat64 = false;
	int		minThreadGroupSize = 1;
};

} // namespace sge
