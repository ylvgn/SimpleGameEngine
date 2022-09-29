#include "ImGui_SGE.h"
#include <sge_render/Renderer.h>
#include <sge_render/vertex/Vertex.h>

namespace sge {

ImGui_SGE::~ImGui_SGE() {
	destroy();
}

void ImGui_SGE::create(CreateDesc& desc) {
	IMGUI_CHECKVERSION();
	_ctx = ImGui::CreateContext();

	if (!_ctx)
		throw SGE_ERROR("ImGui error create context");

	ImGuiIO& io = ImGui::GetIO();

	io.BackendRendererUserData = this;
	io.BackendRendererName = "ImGui_SGE";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// Enable Keyboard Controls

	auto* renderer = Renderer::instance();

	ImGui::StyleColorsDark();

	{ // vertex layout
		Vertex vertex;
		_vertexLayout = vertex.s_layout();
	}

	{ // material
		auto shader = Renderer::instance()->createShader("Assets/Shaders/imgui.shader");
		_material = renderer->createMaterial();
		_material->setShader(shader);
	}
}

void ImGui_SGE::destroy() {
	if (!_ctx) return;
	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererUserData = nullptr;
	io.BackendRendererName = nullptr;
	ImGui::DestroyContext(_ctx);
	_ctx = nullptr;
}

void ImGui_SGE::_createFontsTexture() {
	auto* renderer = Renderer::instance();

	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int w, h;
	io.Fonts->GetTexDataAsAlpha8(&pixels, &w, &h);

	using Color = ColorRb;

	Texture2D_CreateDesc texDesc;
	auto& image = texDesc.imageToUpload;
	texDesc.size.set(w, h);
	texDesc.colorType = Color::kColorType;
	texDesc.mipmapCount = 1;
	image.create(Color::kColorType, w, h);
	image.copyToPixelData(ByteSpan(pixels, w * h));

	_fontsTexture = renderer->createTexture2D(texDesc);
}

void ImGui_SGE::beginRender(RenderContext* renderContext) {
	if (!_fontsTexture) {
		_createFontsTexture();
	}

	ImGuiIO& io = ImGui::GetIO();
	auto s = renderContext->frameBufferSize();
	io.DisplaySize = ImVec2(s.x, s.y);
	io.DeltaTime = 1.0f / 60.0f;

	ImGui::NewFrame();
}

void ImGui_SGE::render(RenderRequest& req) {
	// Rendering
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
		Mat4f mvp(
			{ 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
			{ 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
			{ 0.0f,         0.0f,           0.5f,       0.0f },
			{ (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f }
		);
		_material->setParam("ProjectionMatrix", mvp);
		_material->setParam("texture0", _fontsTexture);
	}

	auto vertexSize = sizeof(ImDrawVert);
	auto indexSize = sizeof(ImDrawIdx);

	SGE_ASSERT(vertexSize == _vertexLayout->stride);
	SGE_ASSERT(indexSize == sizeof(u16));

	auto totalVertexDataSize = data->TotalVtxCount * vertexSize;
	auto totalIndexDataSize = data->TotalIdxCount * indexSize;

	// vertexbuffer
	if (!_vertexBuffer || _vertexBuffer->bufferSize() < totalVertexDataSize)
	{
		if (_vertexBuffer) {
			_vertexBuffer.reset(nullptr);
		}
		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Vertex;
		desc.bufferSize = totalVertexDataSize;
		_vertexBuffer = renderer->createGpuBuffer(desc);
	}

	// indexbuffer
	if (!_indexBuffer || _indexBuffer->bufferSize() < totalIndexDataSize)
	{
		if (_indexBuffer) {
			_indexBuffer.reset(nullptr);
		}
		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Index;
		desc.bufferSize = totalIndexDataSize;
		_indexBuffer = renderer->createGpuBuffer(desc);
	}

	{
		_vertexData.clear();
		_indexData.clear();
		for (int n = 0; n < data->CmdListsCount; n++)
		{
			const ImDrawList* src = data->CmdLists[n];

			auto* cmd = req.commandBuffer.newCommand<RenderCommand_DrawCall>();
#if _DEBUG
			cmd->debugLoc = SGE_LOC;
#endif
			cmd->material			= _material;
			cmd->materialPassIndex	= 0;
			cmd->primitive			= RenderPrimitiveType::Triangles;

			cmd->vertexLayout		= _vertexLayout;
			cmd->vertexBuffer		= _vertexBuffer;
			cmd->vertexOffset		= _vertexData.size();
			cmd->vertexCount		= src->VtxBuffer.Size;

			cmd->indexType			= RenderDataTypeUtil::get<ImDrawIdx>();
			cmd->indexBuffer		= _indexBuffer;
			cmd->indexOffset		= _indexData.size();
			cmd->indexCount			= src->IdxBuffer.Size;

			_vertexData.appendRange(Span<const u8>(reinterpret_cast<const u8*>(src->VtxBuffer.Data), src->VtxBuffer.Size * vertexSize));
			 _indexData.appendRange(Span<const u8>(reinterpret_cast<const u8*>(src->IdxBuffer.Data), src->IdxBuffer.Size * indexSize));
		}

		_vertexBuffer->uploadToGpu(_vertexData);
		 _indexBuffer->uploadToGpu(_indexData);
	}
}

void ImGui_SGE::endRender() {
}

void ImGui_SGE::onUIMouseEvent(UIMouseEvent& ev) {
	ImGuiIO& io = ImGui::GetIO();
	
	using Type = UIMouseEventType;
	switch (ev.type) {
		case Type::Move: {
			io.AddMousePosEvent(ev.pos.x, ev.pos.y);
		} break;

		case Type::Down: {
			io.AddMouseButtonEvent(_mouseButton(ev.pressedButtons), true);
		} break;

		case Type::Up: {
			io.AddMouseButtonEvent(_mouseButton(ev.pressedButtons), false);
		} break;

		case Type::Scroll: {
			io.AddMouseWheelEvent(ev.scroll.x, ev.scroll.y);
		} break;
	}
}

const bool ImGui_SGE::wantCaptureMouse() const {
	auto io = ImGui::GetIO();
	return io.WantCaptureMouse;
}

int ImGui_SGE::_mouseButton(UIMouseEventButton v) {
	using Button = UIMouseEventButton;
	switch (v) {
		case Button::Left:		return 0;
		case Button::Right:		return 1;
		case Button::Middle:	return 2;
		case Button::Button4:	return 3;
		case Button::Button5:	return 4;
	}
	return 0;
}

void ImGui_SGE::onUIMouseCursor(UIMouseEvent& ev) {
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
		return;

	using Cursor = UIMouseEvent::Cursor;

	ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
	if (cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
		ev.cursor = Cursor::None;
	} else {
		ev.cursor = Cursor::Arrow;
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
		}
	}
}

void ImGui_SGE::onUIKeyboardEvent(UIKeyboardEvent& ev) {
	using Type = UIKeyboardEventType;
	ImGuiIO& io = ImGui::GetIO();

	for (auto i = Type::None; i != Type::_End; i += 1)
	{
		if (ev.isUp(i)) {
			io.AddKeyEvent(_keyCode(i), false);
		}
		else if (ev.isDown(i)) {
			io.AddKeyEvent(_keyCode(i), true);
		}
	}
}

ImGuiKey ImGui_SGE::_keyCode(UIKeyboardEventType v)
{
	using Type = UIKeyboardEventType;

	switch (v)
	{
	case Type::Tab: return ImGuiKey_Tab;
	case Type::LeftArrow: return ImGuiKey_LeftArrow;
	case Type::RightArrow: return ImGuiKey_RightArrow;
	case Type::UpArrow: return ImGuiKey_UpArrow;
	case Type::DownArrow: return ImGuiKey_DownArrow;
	case Type::PageUp: return ImGuiKey_PageUp;
	case Type::PageDown: return ImGuiKey_PageDown;
	case Type::Home: return ImGuiKey_Home;
	case Type::End: return ImGuiKey_End;
	case Type::Insert: return ImGuiKey_Insert;
	case Type::Delete: return ImGuiKey_Delete;
	case Type::Backspace: return ImGuiKey_Backspace;
	case Type::Space: return ImGuiKey_Space;
	case Type::Enter: return ImGuiKey_Enter;
	case Type::Escape: return ImGuiKey_Escape;
	//case Type::Apostrophe: return ImGuiKey_Apostrophe;
	case Type::Comma: return ImGuiKey_Comma;
	case Type::Minus: return ImGuiKey_Minus;
	case Type::Period: return ImGuiKey_Period;
	case Type::Slash: return ImGuiKey_Slash;
	case Type::Semicolon: return ImGuiKey_Semicolon;
	//case Type::Equal: return ImGuiKey_Equal;
	case Type::LeftBracket: return ImGuiKey_LeftBracket;
	case Type::Backslash: return ImGuiKey_Backslash;
	case Type::RightBracket: return ImGuiKey_RightBracket;
	//case Type::GraveAccent: return ImGuiKey_GraveAccent;
	case Type::CapsLock: return ImGuiKey_CapsLock;
	case Type::ScrollLock: return ImGuiKey_ScrollLock;
	//case Type::NumLock: return ImGuiKey_NumLock;
	//case Type::PrintScreen: return ImGuiKey_PrintScreen;
	case Type::Pause: return ImGuiKey_Pause;
	case Type::Keypad0: return ImGuiKey_Keypad0;
	case Type::Keypad1: return ImGuiKey_Keypad1;
	case Type::Keypad2: return ImGuiKey_Keypad2;
	case Type::Keypad3: return ImGuiKey_Keypad3;
	case Type::Keypad4: return ImGuiKey_Keypad4;
	case Type::Keypad5: return ImGuiKey_Keypad5;
	case Type::Keypad6: return ImGuiKey_Keypad6;
	case Type::Keypad7: return ImGuiKey_Keypad7;
	case Type::Keypad8: return ImGuiKey_Keypad8;
	case Type::Keypad9: return ImGuiKey_Keypad9;
	//case Type::KeypadDecimal: return ImGuiKey_KeypadDecimal;
	case Type::KeypadDivide: return ImGuiKey_KeypadDivide;
	case Type::KeypadMultiply: return ImGuiKey_KeypadMultiply;
	//case Type::KeypadSubtract: return ImGuiKey_KeypadSubtract;
	//case Type::KeypadAdd: return ImGuiKey_KeypadAdd;
	case Type::LeftShift: return ImGuiKey_LeftShift;
	case Type::LeftCtrl: return ImGuiKey_LeftCtrl;
	case Type::LeftAlt: return ImGuiKey_LeftAlt;
	//case Type::LeftSuper: return ImGuiKey_LeftSuper;
	case Type::RightShift: return ImGuiKey_RightShift;
	case Type::RightCtrl: return ImGuiKey_RightCtrl;
	case Type::RightAlt: return ImGuiKey_RightAlt;
	//case Type::RightSuper: return ImGuiKey_RightSuper;
	//case Type::Menu: return ImGuiKey_Menu;
	case Type::Alpha0: return ImGuiKey_0;
	case Type::Alpha1: return ImGuiKey_1;
	case Type::Alpha2: return ImGuiKey_2;
	case Type::Alpha3: return ImGuiKey_3;
	case Type::Alpha4: return ImGuiKey_4;
	case Type::Alpha5: return ImGuiKey_5;
	case Type::Alpha6: return ImGuiKey_6;
	case Type::Alpha7: return ImGuiKey_7;
	case Type::Alpha8: return ImGuiKey_8;
	case Type::Alpha9: return ImGuiKey_9;
	case Type::A: return ImGuiKey_A;
	case Type::B: return ImGuiKey_B;
	case Type::C: return ImGuiKey_C;
	case Type::D: return ImGuiKey_D;
	case Type::E: return ImGuiKey_E;
	case Type::F: return ImGuiKey_F;
	case Type::G: return ImGuiKey_G;
	case Type::H: return ImGuiKey_H;
	case Type::I: return ImGuiKey_I;
	case Type::J: return ImGuiKey_J;
	case Type::K: return ImGuiKey_K;
	case Type::L: return ImGuiKey_L;
	case Type::M: return ImGuiKey_M;
	case Type::N: return ImGuiKey_N;
	case Type::O: return ImGuiKey_O;
	case Type::P: return ImGuiKey_P;
	case Type::Q: return ImGuiKey_Q;
	case Type::R: return ImGuiKey_R;
	case Type::S: return ImGuiKey_S;
	case Type::T: return ImGuiKey_T;
	case Type::U: return ImGuiKey_U;
	case Type::V: return ImGuiKey_V;
	case Type::W: return ImGuiKey_W;
	case Type::X: return ImGuiKey_X;
	case Type::Y: return ImGuiKey_Y;
	case Type::Z: return ImGuiKey_Z;
	case Type::F1: return ImGuiKey_F1;
	case Type::F2: return ImGuiKey_F2;
	case Type::F3: return ImGuiKey_F3;
	case Type::F4: return ImGuiKey_F4;
	case Type::F5: return ImGuiKey_F5;
	case Type::F6: return ImGuiKey_F6;
	case Type::F7: return ImGuiKey_F7;
	case Type::F8: return ImGuiKey_F8;
	case Type::F9: return ImGuiKey_F9;
	case Type::F10: return ImGuiKey_F10;
	case Type::F11: return ImGuiKey_F11;
	case Type::F12: return ImGuiKey_F12;
	default: return ImGuiKey_None;
	}
}
} // namespace