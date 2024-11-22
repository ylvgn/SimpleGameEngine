#include "ImGui_SGE.h"
#include <sge_render/Renderer.h>
#include <sge_render/vertex/Vertex.h>

namespace sge {

ImGui_SGE::~ImGui_SGE() {
	destroy();
}

void ImGui_SGE::create(CreateDesc& desc) {
	if (!IMGUI_CHECKVERSION())
		throw SGE_ERROR("ImGui version error");
	_ctx = ImGui::CreateContext();
	if (!_ctx)
		throw SGE_ERROR("ImGui error create context");

	_vertexLayout = Vertex::s_layout();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererUserData = this;
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.BackendRendererName = "ImGui_SGE";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	io.ConfigFlags  |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniSavingRate = desc.iniSavingRate;

	if (desc.docking)		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if (desc.multiViewport) io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	setStyle(desc.style);

	auto* renderer	= Renderer::instance();
	auto shader		= Renderer::instance()->createShader("Assets/Shaders/imgui.shader");
	_material		= renderer->createMaterial();

	_material->setShader(shader);
}

void ImGui_SGE::destroy() {
	if (!_ctx) return;

	ImGuiIO& io					= ImGui::GetIO();
	io.BackendRendererUserData	= nullptr;
	io.BackendRendererName		= nullptr;

	ImGui::DestroyContext(_ctx);
	_ctx = nullptr;
}

void ImGui_SGE::setStyle(ImGui_SGE__Style s) {
	ImGuiStyle* style = &ImGui::GetStyle();

	using SRC = ImGui_SGE__Style;

	switch (s)
	{
		case SRC::Classic:	ImGui::StyleColorsClassic();	break;
		case SRC::Light:	ImGui::StyleColorsLight();		break;
		case SRC::Dark:		ImGui::StyleColorsDark();		break;
	}

	if (s == SRC::DuneDark) {
		style->WindowPadding		= ImVec2(15, 15);
		style->WindowRounding		= 5.0f;
		style->FramePadding			= ImVec2(5, 5);
		style->FrameRounding		= 4.0f;
		style->ItemSpacing			= ImVec2(12, 8);
		style->ItemInnerSpacing		= ImVec2(8, 6);
		style->IndentSpacing		= 25.0f;
		style->ScrollbarSize		= 15.0f;
		style->ScrollbarRounding	= 9.0f;
		style->GrabMinSize			= 5.0f;
		style->GrabRounding			= 3.0f;

		style->Colors[ImGuiCol_Text]					= ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled]			= ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg]				= ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ChildBg]					= ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_PopupBg]					= ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_Border]					= ImVec4(0.2f, 0.2f, 0.2f, 0.88f);
		style->Colors[ImGuiCol_BorderShadow]			= ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg]					= ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered]			= ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive]			= ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg]					= ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed]		= ImVec4(0.3f, 0.3f, 0.3f, 0.75f);
		style->Colors[ImGuiCol_TitleBgActive]			= ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg]				= ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg]				= ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab]			= ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_ScrollbarGrabHovered]	= ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrabActive]		= ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_CheckMark]				= ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrab]				= ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrabActive]		= ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Button]					= ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ButtonHovered]			= ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ButtonActive]			= ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_Header]					= ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_HeaderHovered]			= ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive]			= ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Separator]				= ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_SeparatorHovered]		= ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_SeparatorActive]			= ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip]				= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered]		= ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] 		= ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PlotLines]				= ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered]		= ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram]			= ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered]	= ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg]			= ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
//		style->Colors[ImGuiCol_ModalWindowDarkening]	= ImVec4(1.00f, 0.98f, 0.95f, 0.73f); // Obsolete names (will be removed)
		style->Colors[ImGuiCol_Tab]						= style->Colors[ImGuiCol_TabUnfocused];
	}
	else if (s == SRC::AlternativeDark)
    {
        style->WindowPadding		= ImVec2(15, 15);
        style->WindowRounding		= 0.0f;
        style->FramePadding			= ImVec2(5, 5);
        style->FrameRounding		= 0.0f;
        style->ItemSpacing			= ImVec2(12, 8);
        style->ItemInnerSpacing		= ImVec2(8, 6);
        style->IndentSpacing		= 25.0f;
        style->ScrollbarSize		= 15.0f;
        style->ScrollbarRounding	= 0.0f;
        style->GrabMinSize			= 5.0f;
        style->GrabRounding			= 0.0f;
        style->TabRounding			= 0.0f;
        style->ChildRounding		= 0.0f;
        style->PopupRounding		= 0.0f;

        style->WindowBorderSize = 1.0f;
        style->FrameBorderSize = 0.0f;
        style->PopupBorderSize = 1.0f;

        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.27f, 0.27f, 0.29f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.32f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.42f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.53f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.47f, 0.39f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.44f, 0.44f, 0.47f, 0.59f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.20f, 0.22f, 0.39f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.44f, 0.44f, 0.47f, 0.39f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.91f, 0.62f, 0.00f, 0.78f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.91f, 0.62f, 0.00f, 1.00f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

// ----
	if (isMultiViewportEnabled()) {
		style->WindowRounding = 0.0f;
		style->Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
}

void ImGui_SGE::_createFontsTexture() {
	using Color = ColorRb;

	auto* renderer = Renderer::instance();

	unsigned char* pixels;
	int w, h;

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->GetTexDataAsAlpha8(&pixels, &w, &h);

	Texture2D_CreateDesc texDesc;
	Texture2D::UploadRequest texUploadRequest;
	texDesc.uploadRequest = &texUploadRequest;
	texDesc.size.set(w, h);
	texDesc.colorType	= Color::kColorType;
	texDesc.mipmapCount = 1;
	auto& image	= texUploadRequest.imageToUpload;
	image.create(texDesc.colorType, w, h);
	image.copyToPixelData(ByteSpan(pixels, w * h));

	_fontsTexture = renderer->createTexture2D(texDesc);
}

void ImGui_SGE::setDockingEnable(bool isEnable) {
	return isEnable ? _setConfigFlag(ImGuiConfigFlags_DockingEnable) : _unsetConfigFlag(ImGuiConfigFlags_DockingEnable);
}

bool ImGui_SGE::isMultiViewportEnabled() {
	return _hasAnyConfigFlag(ImGuiConfigFlags_ViewportsEnable);
}

void ImGui_SGE::onBeginRender(RenderContext* renderContext) {
	if (!_ctx) return;

	ImGuiIO& io = ImGui::GetIO();
	auto s = renderContext->frameBufferSize();
	Util::convert(io.DisplaySize, s);

	io.DeltaTime = NativeUIApp::current()->targetFrequency(); // 1.0f / 60.0f;

	if (!_fontsTexture) {
		_createFontsTexture();
	}

	ImGui::NewFrame();
}

void ImGui_SGE::onEndRender(RenderContext* renderContext) {
	if (!_ctx) return;
}

void ImGui_SGE::onDrawUI(RenderRequest& req) {
	ImGui::Render();
	
	if (!_material) return;

	auto* data = ImGui::GetDrawData();
	if (!data) return;

	// Avoid rendering when minimized
	if (data->DisplaySize.x <= 0 || data->DisplaySize.y <= 0)
		return;

	if (data->TotalVtxCount <= 0 || data->TotalIdxCount <= 0)
		return;

	auto* renderer = Renderer::instance();

	{ // material
		if (!_material) { SGE_ASSERT(false); return; }
		req.setMaterialCommonParams(_material);

		float L = data->DisplayPos.x;
		float R = data->DisplayPos.x + data->DisplaySize.x;
		float T = data->DisplayPos.y;
		float B = data->DisplayPos.y + data->DisplaySize.y;

		Mat4f sge_matrix_proj (
			{ 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
			{ 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
			{ 0.0f,         0.0f,           0.5f,       0.0f },
			{ (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f }
		);
		_material->setParam("sge_matrix_proj", sge_matrix_proj);
		_material->setParam("texture0", _fontsTexture);
	}

	auto vertexSize = sizeof(ImDrawVert);
	auto indexSize = sizeof(ImDrawIdx);

	SGE_ASSERT(vertexSize == _vertexLayout->stride);
	SGE_ASSERT(indexSize == sizeof(u16));

	auto totalVertexDataSize = data->TotalVtxCount * vertexSize;
	auto totalIndexDataSize = data->TotalIdxCount * indexSize;

	// vertex buffer
	if (!_vertexBuffer || _vertexBuffer->bufferSize() < totalVertexDataSize)
	{
		if (_vertexBuffer) {
			_vertexBuffer.reset(nullptr);
		}
		RenderGpuBuffer::CreateDesc desc;
		desc.type		= RenderGpuBufferType::Vertex;
		desc.bufferSize = totalVertexDataSize;
		_vertexBuffer	= renderer->createGpuBuffer(desc);
	}

	// index buffer
	if (!_indexBuffer || _indexBuffer->bufferSize() < totalIndexDataSize)
	{
		if (_indexBuffer) {
			_indexBuffer.reset(nullptr);
		}
		RenderGpuBuffer::CreateDesc desc;
		desc.type		= RenderGpuBufferType::Index;
		desc.bufferSize = totalIndexDataSize;
		_indexBuffer	= renderer->createGpuBuffer(desc);
	}

	{
		_vertexData.clear();
		_indexData.clear();

		// Render command lists
		// (Because we merged all buffers into a single one, we maintain our own offset into them)
		int global_idx_offset = 0;
		int global_vtx_offset = 0;

		ImVec2 clip_off = data->DisplayPos;
		for (int n = 0; n < data->CmdListsCount; n++)
		{
			const ImDrawList* srcCmd = data->CmdLists[n];
			for (int j = 0; j < srcCmd->CmdBuffer.Size; j++) {
				const ImDrawCmd* srcBuf = &srcCmd->CmdBuffer[j];

				// Project scissor/clipping rectangles into framebuffer space
				ImVec2 clip_min(srcBuf->ClipRect.x - clip_off.x, srcBuf->ClipRect.y - clip_off.y);
				ImVec2 clip_max(srcBuf->ClipRect.z - clip_off.x, srcBuf->ClipRect.w - clip_off.y);

				if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
					continue;

				// Apply scissor/clipping rectangle
				auto a = Vec2f_make(clip_min);
				auto b = Vec2f_make(clip_max);

				req.setScissorRect({ a, b - a });

				// Bind texture, Draw
				//ID3D11ShaderResourceView* texture_srv = (ID3D11ShaderResourceView*)srcBuf->GetTexID();
				//ctx->PSSetShaderResources(0, 1, &texture_srv);

				//ctx->DrawIndexed(srcBuf->ElemCount, srcBuf->IdxOffset + global_idx_offset, srcBuf->VtxOffset + global_vtx_offset);
				auto* cmd = req.addDrawCall();
		#if _DEBUG
				cmd->debugLoc = SGE_LOC;
		#endif
				cmd->material			= _material;
				cmd->materialPassIndex	= 0;
				cmd->primitive			= RenderPrimitiveType::Triangles;
				cmd->vertexLayout		= _vertexLayout;
				cmd->vertexBuffer		= _vertexBuffer;
				cmd->vertexOffset		= (global_vtx_offset + srcBuf->VtxOffset) * vertexSize; 
				cmd->vertexCount		= 0;
				cmd->indexType			= RenderDataTypeUtil::get<ImDrawIdx>();
				cmd->indexBuffer		= _indexBuffer;
				cmd->indexOffset		= (global_idx_offset + srcBuf->IdxOffset) * indexSize;
				cmd->indexCount			= srcBuf->ElemCount;
			}
			global_idx_offset += srcCmd->IdxBuffer.Size;
			global_vtx_offset += srcCmd->VtxBuffer.Size;

			_vertexData.appendRange(Span<const u8>(reinterpret_cast<const u8*>(srcCmd->VtxBuffer.Data), srcCmd->VtxBuffer.Size * vertexSize));
			 _indexData.appendRange(Span<const u8>(reinterpret_cast<const u8*>(srcCmd->IdxBuffer.Data), srcCmd->IdxBuffer.Size * indexSize));
		}

		_vertexBuffer->uploadToGpu(_vertexData);
		 _indexBuffer->uploadToGpu(_indexData);
	}
}

bool ImGui_SGE::onUIMouseEvent(UIMouseEvent& ev) {
	ImGuiIO& io = ImGui::GetIO();
	
	using Type = UIMouseEventType;
	switch (ev.type) {
		case Type::Move: {
			io.AddMousePosEvent(ev.pos.x, ev.pos.y);
		} break;

		case Type::Down: {
			io.AddMouseButtonEvent(_mouseButton(ev.button), true);
		} break;

		case Type::Up: {
			io.AddMouseButtonEvent(_mouseButton(ev.button), false);
		} break;

		case Type::Scroll: {
			io.AddMouseWheelEvent(ev.scroll.x, ev.scroll.y);
		} break;
	}
	return io.WantCaptureMouse;
}

int ImGui_SGE::_mouseButton(UIMouseEventButton v) {
	using Button = UIMouseEventButton;
	switch (v) {
		case Button::Left:		return ImGuiPopupFlags_MouseButtonLeft;
		case Button::Right:		return ImGuiPopupFlags_MouseButtonRight;
		case Button::Middle:	return ImGuiPopupFlags_MouseButtonMiddle;
		case Button::Button4:	return 3;
		case Button::Button5:	return 4;
	}
	return ImGuiPopupFlags_None;
}

void ImGui_SGE::onUIKeyboardEvent(UIKeyboardEvent& ev) {
	using Modifier	= UIEventModifier;
	using Type		= UIKeyCodeEventType;
	using KeyCode	= UIKeyboardEvent::KeyCode;

	ImGuiIO& io		= ImGui::GetIO();

	if (ev.charCode > 0) {
		io.AddInputCharacter(ev.charCode);
	}

	if (ev.isDown()) {
		io.AddKeyEvent(_keyCode(ev.keyCode), true);
	} else if (ev.isUp()) {
		io.AddKeyEvent(_keyCode(ev.keyCode), false);
	}

	// reset modified key
	_onAddKeyEventByUIKeyboardEvent(io, ev, KeyCode::LCtrl);
	_onAddKeyEventByUIKeyboardEvent(io, ev, KeyCode::RCtrl);
	_onAddKeyEventByUIKeyboardEvent(io, ev, KeyCode::LShift);
	_onAddKeyEventByUIKeyboardEvent(io, ev, KeyCode::RShift);
	_onAddKeyEventByUIKeyboardEvent(io, ev, KeyCode::LAlt);
	_onAddKeyEventByUIKeyboardEvent(io, ev, KeyCode::RAlt);
}

ImGuiKey ImGui_SGE::_keyCode(UIKeyboardEventKeyCode v) {

	using KeyCode = UIKeyboardEventKeyCode;

	switch (v) {
		case KeyCode::Tab:				return ImGuiKey_Tab;
		case KeyCode::LeftArrow:		return ImGuiKey_LeftArrow;
		case KeyCode::RightArrow:		return ImGuiKey_RightArrow;
		case KeyCode::UpArrow:			return ImGuiKey_UpArrow;
		case KeyCode::DownArrow:		return ImGuiKey_DownArrow;
		case KeyCode::PageUp:			return ImGuiKey_PageUp;
		case KeyCode::PageDown:			return ImGuiKey_PageDown;
		case KeyCode::Home:				return ImGuiKey_Home;
		case KeyCode::End:				return ImGuiKey_End;
		case KeyCode::Insert:			return ImGuiKey_Insert;
		case KeyCode::Delete:			return ImGuiKey_Delete;
		case KeyCode::Backspace:		return ImGuiKey_Backspace;
		case KeyCode::Space:			return ImGuiKey_Space;
		case KeyCode::Enter:			return ImGuiKey_Enter;
		case KeyCode::Escape:			return ImGuiKey_Escape;
		case KeyCode::Quote:			return ImGuiKey_Apostrophe;
		case KeyCode::Comma:			return ImGuiKey_Comma;
		case KeyCode::Hyphen:			return ImGuiKey_Minus;
		case KeyCode::Period:			return ImGuiKey_Period;
		case KeyCode::Slash:			return ImGuiKey_Slash;
		case KeyCode::Semicolon:		return ImGuiKey_Semicolon;
		case KeyCode::Equals:			return ImGuiKey_Equal;
		case KeyCode::LeftBracket:		return ImGuiKey_LeftBracket;
		case KeyCode::Backslash:		return ImGuiKey_Backslash;
		case KeyCode::RightBracket:		return ImGuiKey_RightBracket;
		case KeyCode::BackQuote:		return ImGuiKey_GraveAccent;
		case KeyCode::CapsLock:			return ImGuiKey_CapsLock;
		case KeyCode::ScrollLock:		return ImGuiKey_ScrollLock;
		case KeyCode::NumLock:			return ImGuiKey_NumLock;
		case KeyCode::PrintScreen:		return ImGuiKey_PrintScreen;
		case KeyCode::Pause:			return ImGuiKey_Pause;
		case KeyCode::Keypad0:			return ImGuiKey_Keypad0;
		case KeyCode::Keypad1:			return ImGuiKey_Keypad1;
		case KeyCode::Keypad2:			return ImGuiKey_Keypad2;
		case KeyCode::Keypad3:			return ImGuiKey_Keypad3;
		case KeyCode::Keypad4:			return ImGuiKey_Keypad4;
		case KeyCode::Keypad5:			return ImGuiKey_Keypad5;
		case KeyCode::Keypad6:			return ImGuiKey_Keypad6;
		case KeyCode::Keypad7:			return ImGuiKey_Keypad7;
		case KeyCode::Keypad8:			return ImGuiKey_Keypad8;
		case KeyCode::Keypad9:			return ImGuiKey_Keypad9;
		case KeyCode::KeypadPeriod:		return ImGuiKey_KeypadDecimal;
		case KeyCode::KeypadDivide:	 	return ImGuiKey_KeypadDivide;
		case KeyCode::KeypadMultiply:	return ImGuiKey_KeypadMultiply;
		case KeyCode::KeypadMinus:		return ImGuiKey_KeypadSubtract;
		case KeyCode::KeypadPlus:	 	return ImGuiKey_KeypadAdd;
		case KeyCode::Ctrl:				return ImGuiKey_ModCtrl;
		case KeyCode::Shift:			return ImGuiKey_ModShift;
		case KeyCode::Alt:				return ImGuiKey_ModAlt;
		case KeyCode::LShift:			return ImGuiKey_LeftShift;
		case KeyCode::LCtrl:			return ImGuiKey_LeftCtrl;
		case KeyCode::LAlt:				return ImGuiKey_LeftAlt;
		case KeyCode::LCmd: 			return ImGuiKey_LeftSuper;
		case KeyCode::RShift:  			return ImGuiKey_RightShift;
		case KeyCode::RCtrl:   			return ImGuiKey_RightCtrl;
		case KeyCode::RAlt:	   			return ImGuiKey_RightAlt;
		case KeyCode::RCmd:				return ImGuiKey_RightSuper;
		case KeyCode::Cmd:				return ImGuiKey_Menu;
		case KeyCode::Alpha0:			return ImGuiKey_0;
		case KeyCode::Alpha1:			return ImGuiKey_1;
		case KeyCode::Alpha2:			return ImGuiKey_2;
		case KeyCode::Alpha3:			return ImGuiKey_3;
		case KeyCode::Alpha4:			return ImGuiKey_4;
		case KeyCode::Alpha5:			return ImGuiKey_5;
		case KeyCode::Alpha6:			return ImGuiKey_6;
		case KeyCode::Alpha7:			return ImGuiKey_7;
		case KeyCode::Alpha8:			return ImGuiKey_8;
		case KeyCode::Alpha9:			return ImGuiKey_9;
		case KeyCode::A:				return ImGuiKey_A;
		case KeyCode::B:				return ImGuiKey_B;
		case KeyCode::C:				return ImGuiKey_C;
		case KeyCode::D:				return ImGuiKey_D;
		case KeyCode::E:				return ImGuiKey_E;
		case KeyCode::F:				return ImGuiKey_F;
		case KeyCode::G:				return ImGuiKey_G;
		case KeyCode::H:				return ImGuiKey_H;
		case KeyCode::I:				return ImGuiKey_I;
		case KeyCode::J:				return ImGuiKey_J;
		case KeyCode::K:				return ImGuiKey_K;
		case KeyCode::L:				return ImGuiKey_L;
		case KeyCode::M:				return ImGuiKey_M;
		case KeyCode::N:				return ImGuiKey_N;
		case KeyCode::O:				return ImGuiKey_O;
		case KeyCode::P:				return ImGuiKey_P;
		case KeyCode::Q:				return ImGuiKey_Q;
		case KeyCode::R:				return ImGuiKey_R;
		case KeyCode::S:				return ImGuiKey_S;
		case KeyCode::T:				return ImGuiKey_T;
		case KeyCode::U:				return ImGuiKey_U;
		case KeyCode::V:				return ImGuiKey_V;
		case KeyCode::W:				return ImGuiKey_W;
		case KeyCode::X:				return ImGuiKey_X;
		case KeyCode::Y:				return ImGuiKey_Y;
		case KeyCode::Z:				return ImGuiKey_Z;
		case KeyCode::F1:				return ImGuiKey_F1;
		case KeyCode::F2:				return ImGuiKey_F2;
		case KeyCode::F3:				return ImGuiKey_F3;
		case KeyCode::F4:				return ImGuiKey_F4;
		case KeyCode::F5:				return ImGuiKey_F5;
		case KeyCode::F6:				return ImGuiKey_F6;
		case KeyCode::F7:				return ImGuiKey_F7;
		case KeyCode::F8:				return ImGuiKey_F8;
		case KeyCode::F9:				return ImGuiKey_F9;
		case KeyCode::F10:				return ImGuiKey_F10;
		case KeyCode::F11:				return ImGuiKey_F11;
		case KeyCode::F12:				return ImGuiKey_F12;
	}
	return ImGuiKey_None;
}

void ImGui_SGE::_setConfigFlag(ImGuiConfigFlags_ flag) {
	using T = decltype(ImGui::GetIO().ConfigFlags);
	return BitUtil::set(ImGui::GetIO().ConfigFlags, T(flag));
}

void ImGui_SGE::_unsetConfigFlag(ImGuiConfigFlags_ flag) {
	using T = decltype(ImGui::GetIO().ConfigFlags);
	return BitUtil::unset(ImGui::GetIO().ConfigFlags, T(flag));
}

bool ImGui_SGE::_hasAnyConfigFlag(ImGuiConfigFlags_ flag) {
	using T = decltype(ImGui::GetIO().ConfigFlags);
	return BitUtil::hasAny(ImGui::GetIO().ConfigFlags, T(flag));
}

void ImGui_SGE::_onAddKeyEventByUIKeyboardEvent(ImGuiIO& io, UIKeyboardEvent& ev, UIKeyboardEventKeyCode v) {
	if (ev.isDown(v)) io.AddKeyEvent(_keyCode(v), true);
	else if (ev.isUp(v)) io.AddKeyEvent(_keyCode(v), false);
}

#if 0 // how to set cursor in good practise ???
void ImGui_SGE::onUIMouseCursor(UIMouseEvent& ev) {
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
		return;

	using Cursor = UIMouseCursor;

	ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
	if (cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
		ev.cursor = Cursor::None;
	} else {
		switch (cursor)
		{
			case ImGuiMouseCursor_Arrow:        ev.cursor = Cursor::Arrow;		break;
			case ImGuiMouseCursor_TextInput:    ev.cursor = Cursor::IBeam;		break;
			case ImGuiMouseCursor_ResizeAll:    ev.cursor = Cursor::SizeAll;	break;
			case ImGuiMouseCursor_ResizeEW:     ev.cursor = Cursor::SizeWE;		break;
			case ImGuiMouseCursor_ResizeNS:     ev.cursor = Cursor::SizeNS;		break;
			case ImGuiMouseCursor_ResizeNESW:   ev.cursor = Cursor::SizeNESW;	break;
			case ImGuiMouseCursor_ResizeNWSE:   ev.cursor = Cursor::SizeNWSE;	break;
			case ImGuiMouseCursor_Hand:         ev.cursor = Cursor::Hand;		break;
			case ImGuiMouseCursor_NotAllowed:   ev.cursor = Cursor::No;			break;
			default:							ev.cursor = Cursor::Arrow;		break;
		}
	}
}
#endif

} // namespace