#pragma once
#include "imgui.h"
#include <sge_render/command/RenderRequest.h>

namespace sge {

struct NativeImGui_CreateDesc {
	NativeUIWindow* window = nullptr;
};

class NativeImGui : public NonCopyable {
	using CreateDesc	= NativeImGui_CreateDesc;
	using Vertex		= Vertex_PosColorUv<1>;
public:
	NativeImGui();
	~NativeImGui() { destroy(); };

	void create(CreateDesc& desc);
	void render(RenderRequest& req);
	void destroy();

private:
	SPtr<Material>			_material;
	SPtr<Texture2D>			_fontsTexture;

	const VertexLayout*		_vertexLayout = nullptr;
	SPtr<RenderGpuBuffer>	_vertexBuffer;

	RenderDataType			indexType() const { return sizeof(ImDrawIdx) == 2 ? RenderDataTypeUtil::get<u16>() : RenderDataTypeUtil::get<u32>(); }
	SPtr<RenderGpuBuffer>	_indexBuffer;

	Vector<u8>	_vertexData;
	Vector<u8>	_indexData;

	void _createFontsTexture();
	void _createDeviceObjects();
	void _renderDrawData(RenderRequest& req);
};

} // namespace