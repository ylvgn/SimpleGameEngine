#include "ImGui_Base.h"

#include "../backend/dx11/ImGui_DX11.h"

namespace sge {

ImGui_Base* ImGui_Base::s_instance = nullptr;

ImGui_Base* ImGui_Base::create(CreateDesc& desc) {
	ImGui_Base* ret = nullptr;
	switch (Renderer::instance()->apiType())
	{
		case Renderer::ApiType::DX11: {
			ret = new ImGui_DX11(desc);
		}break;
		case Renderer::ApiType::OpenGL: {
			SGE_ASSERT("error: not support OpenGL api");
		}break;

		default: {
			SGE_ASSERT("error: not support render api");
		}break;
	}
	return ret;
}

ImGui_Base::ImGui_Base() {
	SGE_ASSERT(s_instance == nullptr);
	s_instance = this;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();
}

ImGui_Base::~ImGui_Base() {
	ImGui::DestroyContext();

	SGE_ASSERT(s_instance == this);
	s_instance = nullptr;
}

} // namespace

