#pragma once

#include <sge_render/imgui/ImGui_Base.h>

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

namespace sge {

class ImGui_DX11 : public ImGui_Base {
private:
	using Base = ImGui_Base;

public:
	ImGui_DX11(CreateDesc& desc);
	~ImGui_DX11();
	static ImGui_DX11* instance() { return static_cast<ImGui_DX11*>(s_instance); }

protected:
	virtual void onBeginRender() override;
	virtual void onRender() override;
	virtual void onEndRender() override;
};

} // namespace