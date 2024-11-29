#pragma once
#include "imgui.h"
#include <sge_render/command/RenderRequest.h>

namespace sge {

struct ImGui_SGE__Util {
	ImGui_SGE__Util() = delete;

	static void convert(Vec2f& o, const ImVec2& i) { o.x = i.x; o.y = i.y; }
	static void convert(ImVec2& o, const Vec2f& i) { o.x = i.x; o.y = i.y; }

	static void convert(Vec4f& o, const ImVec4& i) { o.x = i.x; o.y = i.y; o.z = i.z; o.w = i.w; }
	static void convert(ImVec4& o, const Vec4f& i) { o.x = i.x; o.y = i.y; o.z = i.z; o.w = i.w; }

	static void convert(Color4f& o, const ImVec4& i) { o.r = i.x; o.g = i.y; o.b = i.z; o.a = i.w; }
	static void convert(ImVec4& o, const Color4f& i) { o.x = i.r; o.y = i.g; o.z = i.b; o.w = i.a; }

	static void convert(Color4f& o, const ImColor& i) { convert(o, i.Value); }
	static void convert(ImColor& o, const Color4f& i) { convert(o.Value, i); }
};

enum class ImGui_SGE__Style {
	Classic,
	Light,
	Dark,
	DuneDark, // https://www.unknowncheats.me/forum/direct3d/189635-imgui-style-settings.html
	AlternativeDark,
};

class ImGui_SGE : public NonCopyable {
	using Vertex = Vertex_ImGui;
	using Util = ImGui_SGE__Util;
public:
	struct CreateDesc {
		using Style = ImGui_SGE__Style;

		CreateDesc()
			: iniSavingRate(60.f)
			, style(Style::DuneDark)
		{}

		float iniSavingRate;
		Style style;
	};

	~ImGui_SGE();

	void create(CreateDesc& desc);
	void destroy();

	void setStyle(ImGui_SGE__Style s);

	void setDockingEnable(bool isEnable);

	bool isMultiViewportEnabled();

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

	Vector<u8>				_vertexData;
	Vector<u8>				_indexData;

	void		_createFontsTexture();

	int			_mouseButton(UIMouseEventButton v);
	ImGuiKey	_keyCode(UIKeyboardEventKeyCode v);

	void _setConfigFlag		(ImGuiConfigFlags_ flag);
	void _unsetConfigFlag	(ImGuiConfigFlags_ flag);
	bool _hasAnyConfigFlag	(ImGuiConfigFlags_ flag);

	void _onAddKeyEventByUIKeyboardEvent(ImGuiIO& io, UIKeyboardEvent& ev, UIKeyboardEventKeyCode v);

	ImGuiContext* _ctx = nullptr;
}; // ImGui_SGE

inline Vec2f	Vec2f_make(const ImVec2& v)							{ return Vec2f(v.x, v.y); }
inline Vec4f	Vec4f_make(const ImVec4& v)							{ return Vec4f(v.x, v.y, v.z, v.w); }
inline Color4f	Color4f_make(const ImVec4& v)						{ return Color4f(v.x, v.y, v.z, v.w); }
inline Color4f	Color4f_make(const ImColor& v)						{ return Color4f_make(v.Value); }
inline Rect2f	Rect2f_make(const ImVec2& pos, const ImVec2& size)	{ return Rect2f(pos.x, pos.y, size.x, size.y); }

} // namespace sge