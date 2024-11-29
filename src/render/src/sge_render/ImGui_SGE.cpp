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
	io.ConfigFlags  |= ImGuiConfigFlags_DockingEnable;	// docking: require imgui[docking-experimental]
	io.ConfigFlags  |= ImGuiConfigFlags_ViewportsEnable; // multiViewport: require imgui[docking-experimental]
	io.IniSavingRate = desc.iniSavingRate;

	setStyle(desc.style);

	{
		auto* renderer = Renderer::instance();
		auto shader	= Renderer::instance()->createShader("Assets/Shaders/imgui.shader");
		_material = renderer->createMaterial();
		_material->setShader(shader);
	}
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
		style->WindowPadding		= ImVec2(15,15);
		style->WindowRounding		= 5.0f;
		style->FramePadding			= ImVec2(5,5);
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
        style->WindowBorderSize		= 1.0f;
        style->FrameBorderSize		= 0.0f;
        style->PopupBorderSize		= 1.0f;

        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]					= ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
        colors[ImGuiCol_TextDisabled]			= ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg]				= ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
        colors[ImGuiCol_ChildBg]				= ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
        colors[ImGuiCol_PopupBg]				= ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
        colors[ImGuiCol_Border]					= ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_BorderShadow]			= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]				= ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]			= ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_FrameBgActive]			= ImVec4(0.27f, 0.27f, 0.29f, 1.00f);
        colors[ImGuiCol_TitleBg]				= ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_TitleBgActive]			= ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]		= ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_MenuBarBg]				= ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]			= ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab]			= ImVec4(0.31f, 0.31f, 0.32f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]	= ImVec4(0.41f, 0.41f, 0.42f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]	= ImVec4(0.51f, 0.51f, 0.53f, 1.00f);
        colors[ImGuiCol_CheckMark]				= ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_SliderGrab]				= ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]		= ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
        colors[ImGuiCol_Button]					= ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_ButtonHovered]			= ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_ButtonActive]			= ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
        colors[ImGuiCol_Header]					= ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_HeaderHovered]			= ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_HeaderActive]			= ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
        colors[ImGuiCol_Separator]				= ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
        colors[ImGuiCol_SeparatorHovered]		= ImVec4(0.44f, 0.44f, 0.47f, 0.39f);
        colors[ImGuiCol_SeparatorActive]		= ImVec4(0.44f, 0.44f, 0.47f, 0.59f);
        colors[ImGuiCol_ResizeGrip]				= ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
        colors[ImGuiCol_ResizeGripHovered]		= ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
        colors[ImGuiCol_ResizeGripActive]		= ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
        colors[ImGuiCol_Tab]					= ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_TabHovered]				= ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_TabActive]				= ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_TabUnfocused]			= ImVec4(0.20f, 0.20f, 0.22f, 0.39f);
        colors[ImGuiCol_TabUnfocusedActive]		= ImVec4(0.44f, 0.44f, 0.47f, 0.39f);
        colors[ImGuiCol_DockingPreview]			= ImVec4(0.91f, 0.62f, 0.00f, 0.78f);
        colors[ImGuiCol_DockingEmptyBg]			= ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines]				= ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]		= ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
        colors[ImGuiCol_PlotHistogram]			= ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]	= ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
        colors[ImGuiCol_TextSelectedBg]			= ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget]			= ImVec4(0.91f, 0.62f, 0.00f, 1.00f);
        colors[ImGuiCol_NavHighlight]			= ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]	= ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]		= ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]		= ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

// ----
	if (isMultiViewportEnabled()) {
		style->WindowRounding = 0.0f;
		style->Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
}

void ImGui_SGE::_createFontsTexture() {
	unsigned char* pixels;
	int w, h;

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->GetTexDataAsAlpha8(&pixels, &w, &h);

	Texture2D_CreateDesc texDesc;
	Texture2D::UploadRequest texUploadRequest;
	texDesc.uploadRequest = &texUploadRequest;
	texDesc.size.set(w, h);
	texDesc.colorType = ColorRb::kColorType;
	texDesc.mipmapCount = 1;
	auto& image	= texUploadRequest.imageToUpload;
	image.create(texDesc.colorType, w, h);
	image.copyToPixelData(ByteSpan(pixels, w * h));

	auto* renderer = Renderer::instance();
	_fontsTexture = renderer->createTexture2D(texDesc);
}

void ImGui_SGE::setDockingEnable(bool isEnable) {
	return isEnable
		? _setConfigFlag(ImGuiConfigFlags_DockingEnable)
		: _unsetConfigFlag(ImGuiConfigFlags_DockingEnable);
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

		Mat4f sge_matrix_proj ({ 2.0f/(R-L),  0.0f,        0.0f, 0.0f },
							   { 0.0f,        2.0f/(T-B),  0.0f, 0.0f },
							   { 0.0f,        0.0f,        0.5f, 0.0f },
							   { (R+L)/(L-R), (T+B)/(B-T), 0.5f, 1.0f });
		_material->setParam("sge_matrix_proj", sge_matrix_proj);
		_material->setParam("texture0", _fontsTexture);
	}

	auto vertexSize = sizeof(ImDrawVert);
	auto indexSize = sizeof(ImDrawIdx);

	SGE_ASSERT(vertexSize == _vertexLayout->stride);
	SGE_ASSERT(indexSize == sizeof(u16));

	auto totalVertexDataSize = data->TotalVtxCount * vertexSize;
	auto totalIndexDataSize  = data->TotalIdxCount * indexSize;

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

	ImGuiIO& io	= ImGui::GetIO();

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
	using SRC = UIKeyboardEventKeyCode;

	switch (v) {
		case SRC::Tab:				return ImGuiKey_Tab;
		case SRC::LeftArrow:		return ImGuiKey_LeftArrow;
		case SRC::RightArrow:		return ImGuiKey_RightArrow;
		case SRC::UpArrow:			return ImGuiKey_UpArrow;
		case SRC::DownArrow:		return ImGuiKey_DownArrow;
		case SRC::PageUp:			return ImGuiKey_PageUp;
		case SRC::PageDown:			return ImGuiKey_PageDown;
		case SRC::Home:				return ImGuiKey_Home;
		case SRC::End:				return ImGuiKey_End;
		case SRC::Insert:			return ImGuiKey_Insert;
		case SRC::Delete:			return ImGuiKey_Delete;
		case SRC::Backspace:		return ImGuiKey_Backspace;
		case SRC::Space:			return ImGuiKey_Space;
		case SRC::Enter:			return ImGuiKey_Enter;
		case SRC::Escape:			return ImGuiKey_Escape;
		case SRC::Quote:			return ImGuiKey_Apostrophe;
		case SRC::Comma:			return ImGuiKey_Comma;
		case SRC::Hyphen:			return ImGuiKey_Minus;
		case SRC::Period:			return ImGuiKey_Period;
		case SRC::Slash:			return ImGuiKey_Slash;
		case SRC::Semicolon:		return ImGuiKey_Semicolon;
		case SRC::Equals:			return ImGuiKey_Equal;
		case SRC::LeftBracket:		return ImGuiKey_LeftBracket;
		case SRC::Backslash:		return ImGuiKey_Backslash;
		case SRC::RightBracket:		return ImGuiKey_RightBracket;
		case SRC::BackQuote:		return ImGuiKey_GraveAccent;
		case SRC::CapsLock:			return ImGuiKey_CapsLock;
		case SRC::ScrollLock:		return ImGuiKey_ScrollLock;
		case SRC::NumLock:			return ImGuiKey_NumLock;
		case SRC::PrintScreen:		return ImGuiKey_PrintScreen;
		case SRC::Pause:			return ImGuiKey_Pause;
		case SRC::Keypad0:			return ImGuiKey_Keypad0;
		case SRC::Keypad1:			return ImGuiKey_Keypad1;
		case SRC::Keypad2:			return ImGuiKey_Keypad2;
		case SRC::Keypad3:			return ImGuiKey_Keypad3;
		case SRC::Keypad4:			return ImGuiKey_Keypad4;
		case SRC::Keypad5:			return ImGuiKey_Keypad5;
		case SRC::Keypad6:			return ImGuiKey_Keypad6;
		case SRC::Keypad7:			return ImGuiKey_Keypad7;
		case SRC::Keypad8:			return ImGuiKey_Keypad8;
		case SRC::Keypad9:			return ImGuiKey_Keypad9;
		case SRC::KeypadPeriod:		return ImGuiKey_KeypadDecimal;
		case SRC::KeypadDivide:	 	return ImGuiKey_KeypadDivide;
		case SRC::KeypadMultiply:	return ImGuiKey_KeypadMultiply;
		case SRC::KeypadMinus:		return ImGuiKey_KeypadSubtract;
		case SRC::KeypadPlus:	 	return ImGuiKey_KeypadAdd;
		case SRC::Ctrl:				return ImGuiKey_ModCtrl;
		case SRC::Shift:			return ImGuiKey_ModShift;
		case SRC::Alt:				return ImGuiKey_ModAlt;
		case SRC::LShift:			return ImGuiKey_LeftShift;
		case SRC::LCtrl:			return ImGuiKey_LeftCtrl;
		case SRC::LAlt:				return ImGuiKey_LeftAlt;
		case SRC::LCmd: 			return ImGuiKey_LeftSuper;
		case SRC::RShift:  			return ImGuiKey_RightShift;
		case SRC::RCtrl:   			return ImGuiKey_RightCtrl;
		case SRC::RAlt:	   			return ImGuiKey_RightAlt;
		case SRC::RCmd:				return ImGuiKey_RightSuper;
		case SRC::Cmd:				return ImGuiKey_Menu;
		case SRC::Alpha0:			return ImGuiKey_0;
		case SRC::Alpha1:			return ImGuiKey_1;
		case SRC::Alpha2:			return ImGuiKey_2;
		case SRC::Alpha3:			return ImGuiKey_3;
		case SRC::Alpha4:			return ImGuiKey_4;
		case SRC::Alpha5:			return ImGuiKey_5;
		case SRC::Alpha6:			return ImGuiKey_6;
		case SRC::Alpha7:			return ImGuiKey_7;
		case SRC::Alpha8:			return ImGuiKey_8;
		case SRC::Alpha9:			return ImGuiKey_9;
		case SRC::A:				return ImGuiKey_A;
		case SRC::B:				return ImGuiKey_B;
		case SRC::C:				return ImGuiKey_C;
		case SRC::D:				return ImGuiKey_D;
		case SRC::E:				return ImGuiKey_E;
		case SRC::F:				return ImGuiKey_F;
		case SRC::G:				return ImGuiKey_G;
		case SRC::H:				return ImGuiKey_H;
		case SRC::I:				return ImGuiKey_I;
		case SRC::J:				return ImGuiKey_J;
		case SRC::K:				return ImGuiKey_K;
		case SRC::L:				return ImGuiKey_L;
		case SRC::M:				return ImGuiKey_M;
		case SRC::N:				return ImGuiKey_N;
		case SRC::O:				return ImGuiKey_O;
		case SRC::P:				return ImGuiKey_P;
		case SRC::Q:				return ImGuiKey_Q;
		case SRC::R:				return ImGuiKey_R;
		case SRC::S:				return ImGuiKey_S;
		case SRC::T:				return ImGuiKey_T;
		case SRC::U:				return ImGuiKey_U;
		case SRC::V:				return ImGuiKey_V;
		case SRC::W:				return ImGuiKey_W;
		case SRC::X:				return ImGuiKey_X;
		case SRC::Y:				return ImGuiKey_Y;
		case SRC::Z:				return ImGuiKey_Z;
		case SRC::F1:				return ImGuiKey_F1;
		case SRC::F2:				return ImGuiKey_F2;
		case SRC::F3:				return ImGuiKey_F3;
		case SRC::F4:				return ImGuiKey_F4;
		case SRC::F5:				return ImGuiKey_F5;
		case SRC::F6:				return ImGuiKey_F6;
		case SRC::F7:				return ImGuiKey_F7;
		case SRC::F8:				return ImGuiKey_F8;
		case SRC::F9:				return ImGuiKey_F9;
		case SRC::F10:				return ImGuiKey_F10;
		case SRC::F11:				return ImGuiKey_F11;
		case SRC::F12:				return ImGuiKey_F12;
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

} // namespace sge