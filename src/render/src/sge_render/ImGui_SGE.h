#pragma once
#include "imgui.h"
#include <sge_render/command/RenderRequest.h>

namespace sge {

class ImGui_SGE : public NonCopyable {
	using Vertex = Vertex_ImGui;
public:
	~ImGui_SGE();

	void create();
	void destroy();

	void onBeginRender(RenderContext* renderContext);
	void onEndRender(RenderContext* renderContext);
	void onDrawUI(RenderRequest& req);

	bool onUIMouseEvent(UIMouseEvent& ev);
	void onUIKeyboardEvent(UIKeyboardEvent& ev);

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
	ImGuiKey _keyCode(UIKeyboardEventKeyCode v);

	void _onAddKeyEventByUIKeyboardEvent(ImGuiIO& io, UIKeyboardEvent& ev, UIKeyboardEventKeyCode v);

	ImGuiContext* _ctx = nullptr;
};

inline Vec2f Vec2f_make(const ImVec2& v) { return Vec2f(v.x, v.y); }
inline Vec4f Vec4f_make(const ImVec4& v) { return Vec4f(v.x, v.y, v.z, v.w); }

} // namespace