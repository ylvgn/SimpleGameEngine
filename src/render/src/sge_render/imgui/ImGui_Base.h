#pragma once
#include <sge_render/Renderer.h>

#include "imgui.h"

namespace sge {

class ImGui_Base : public NonCopyable {
public:

	struct CreateDesc {
		HWND hwnd = nullptr;
	};

	static ImGui_Base* ImGui_Base::create(CreateDesc& desc);
	static ImGui_Base* instance() { return s_instance; };

	ImGui_Base();
	virtual ~ImGui_Base();

	void beginRender()	{ onBeginRender(); };

	void render() {
		ImGui::Render();
		onRender();
	}

	void endRender()	{ onEndRender(); };

protected:
	static ImGui_Base* s_instance;

	virtual void onBeginRender() {};
	virtual void onRender() {};
	virtual void onEndRender() {};
};

} // namespace