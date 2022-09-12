#pragma once
#include "imgui.h"
#include <sge_render/command/RenderRequest.h>

namespace sge {

struct ImGui_SGE_CreateDesc {
	NativeUIWindow* window = nullptr;
};

class ImGui_SGE : public NonCopyable {
	using CreateDesc	= ImGui_SGE_CreateDesc;
	using Vertex		= Vertex_ImGui;
public:
	~ImGui_SGE();

	void create(CreateDesc& desc);
	void destroy();

	void beginRender(RenderContext* renderContext);
	void render(RenderRequest& req);
	void endRender();

	void onUIMouseEvent(UIMouseEvent& ev);

	const bool wantCaptureMouse() const;

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

	int _mouseButton(UIMouseEventButton v);

	ImGuiContext* _ctx;
};

} // namespace