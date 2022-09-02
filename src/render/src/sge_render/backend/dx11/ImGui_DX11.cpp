#include "ImGui_DX11.h"

#include "Renderer_DX11.h"

namespace sge {

ImGui_DX11::ImGui_DX11(CreateDesc& desc) {
	auto* renderer = Renderer_DX11::instance();
	ImGui_ImplWin32_Init(desc.hwnd);
	ImGui_ImplDX11_Init(renderer->d3dDevice(), renderer->d3dDeviceContext());
}

ImGui_DX11::~ImGui_DX11() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

void ImGui_DX11::onBeginRender() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGui_DX11::onEndRender() {
}

void ImGui_DX11::onRender() {
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

} // namespace
