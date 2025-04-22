#include "MyImGuiWindow.h"
#include "MyImGuiApp.h"

#include "../render/my_render.h"
#include "imgui.h"

#define MyImGuiUIWindow_TEST_RENDER_MESH 1

namespace sge {

class MyImGuiUIWindow::Impl : public MyOpenGLNativeUIWindow {
    using Base		 = MyOpenGLNativeUIWindow;
	using This		 = typename MyImGuiUIWindow::Impl;
	using VertexType = ImDrawVert;
	using IndexType  = ImDrawIdx;
public:
// FYI: https://github.com/ocornut/imgui/blob/9701810bc36577d443422b2894ba1a580faef13b/backends/imgui_impl_opengl3.cpp

//	GLuint          GlVersion;               // Extracted at runtime using GL_MAJOR_VERSION, GL_MINOR_VERSION queries (e.g. 320 for GL 3.2)
//	char            GlslVersionString[32];   // Specified by user or detected based on compile time GL settings.
//	GLint           GlProfileMask;
//	GLuint          FontTexture;
//	GLuint          ShaderHandle;
//	GLint           AttribLocationTex;       // Uniforms location
//	GLint           AttribLocationProjMtx;
	GLuint          AttribLocationVtxPos;    // Vertex attributes location
	GLuint          AttribLocationVtxUV;
	GLuint          AttribLocationVtxColor;
#if !MyImGuiUIWindow_TEST_RENDER_MESH
	unsigned int    VboHandle, ElementsHandle;
#endif
//	GLsizeiptr      VertexBufferSize;
//	GLsizeiptr      IndexBufferSize;
//	bool            GlProfileIsES2   : 1;
//	bool            GlProfileIsES3   : 1;
//	bool            GlProfileIsCompat: 1;
//	bool            HasPolygonMode   : 1;
//	bool            HasClipOrigin	 : 1;
//	bool            UseBufferSubData : 1;

    Impl(MyImGuiUIWindow* owner, CreateDesc& desc) {
        _owner = owner;
        Base::create(desc);
    }

    ~Impl() {
        if (!_ctx) return;

        ImGuiIO& io					= ImGui::GetIO();
        io.BackendRendererUserData	= nullptr;
        io.BackendRendererName		= nullptr;
		io.BackendFlags				&= ~ImGuiBackendFlags_RendererHasVtxOffset;
#if !MyImGuiUIWindow_TEST_RENDER_MESH
		if (VboHandle)		{ glDeleteBuffers(1, &VboHandle); VboHandle = 0; }
		if (ElementsHandle) { glDeleteBuffers(1, &ElementsHandle); ElementsHandle = 0; }
#endif
        ImGui::DestroyContext();
        _ctx = nullptr;
    }

	virtual void onInitGL() override {
//		glEnable(GL_SCISSOR_TEST); Dont glEnable GL_SCISSOR_TEST once

		if (!IMGUI_CHECKVERSION())
			throw SGE_ERROR("ImGui version error");

		SGE_ASSERT(_ctx == nullptr);
		_ctx = ImGui::CreateContext();
		if (!_ctx)
			throw SGE_ERROR("ImGui error create context");

		ImGuiIO& io = ImGui::GetIO();
		io.BackendRendererUserData = this;
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		io.BackendRendererName = "MyImGuiUIWindow - Impl";
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
		io.ConfigFlags	|= ImGuiConfigFlags_NavEnableKeyboard;

		_shader = new MyShader();
		_shader->loadFile("Assets/Shaders/imgui");

//		AttribLocationTex		= _shader->getUniformLoc("sampler0");	// bd->AttribLocationTex = glGetUniformLocation(bd->ShaderHandle, "Texture");
//		AttribLocationProjMtx	= _shader->getUniformLoc("matMVP");		// bd->AttribLocationProjMtx = glGetUniformLocation(bd->ShaderHandle, "ProjMtx");

		AttribLocationVtxPos	= _shader->getAttribLoc("v_pos");		// bd->AttribLocationVtxPos = (GLuint)glGetAttribLocation(bd->ShaderHandle, "Position");
		AttribLocationVtxUV		= _shader->getAttribLoc("v_uv");		// bd->AttribLocationVtxUV = (GLuint)glGetAttribLocation(bd->ShaderHandle, "UV");
		AttribLocationVtxColor	= _shader->getAttribLoc("v_color");		// bd->AttribLocationVtxColor = (GLuint)glGetAttribLocation(bd->ShaderHandle, "Color")

#if !MyImGuiUIWindow_TEST_RENDER_MESH
		// Create buffers
		glGenBuffers(1, &VboHandle);
		glGenBuffers(1, &ElementsHandle);
#endif

		_createFontTex();
	}
    
    virtual void onActive(bool isActive) override {
        Base::onActive(isActive);
		_owner->_onWindowActive(isActive);
    }

    virtual void onCloseButton() override {
		if (!_owner->onWindowCloseButton()) return;
		sge_delete(_owner);
	}

    bool _imgui_onUIMouseEvent(UIMouseEvent& ev);
    bool _imgui_onUIKeyboardEvent(UIKeyboardEvent& ev);

    virtual void onUIMouseEvent(UIMouseEvent& ev) override {
        if (_imgui_onUIMouseEvent(ev)) return;
        if (_owner->onUIMouseEvent(ev)) return;
        Base::onUIMouseEvent(ev);
    }

	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override {
        if (_imgui_onUIKeyboardEvent(ev)) return;
        if (_owner->onUIKeyboardEvent(ev)) return;
        Base::onUIKeyboardEvent(ev);
    }

	void onUpdate(float dt) {
		_owner->onUpdate(dt);
	}

	void onRender() {
		beginRender();
			_owner->onRender();
			_render();
		endRender();

		drawNeeded();
	}

	void _createFontTex();
	void _render();

	void virtual onBeginRender() override {
		Base::onBeginRender();

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(_frameBufferSize.x, _frameBufferSize.y);
		io.DeltaTime = NativeUIApp::current()->targetFrequency();
		ImGui::NewFrame();
	}

    int	_mouseButton(UIMouseEventButton v);

	void ImGui_ImplOpenGL3_Init();
	void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data);
	void ImGui_ImplOpenGL3_SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height);

	SPtr<MyTexture2D> _fontTex;
	SPtr<MyShader>	  _shader;

	MyVertexArray	  _vertexArray;

#if MyImGuiUIWindow_TEST_RENDER_MESH
	MyRenderMesh	  _mesh;
#endif

    MyImGuiUIWindow*  _owner = nullptr;
    ImGuiContext*	  _ctx   = nullptr;
};

void MyImGuiUIWindow::Impl::_render() {
	ImGui::Render();
	if (!_shader) return;

	auto* draw_data = ImGui::GetDrawData();
	if (!draw_data) return;

	ImGui_ImplOpenGL3_RenderDrawData(draw_data);
}

void MyImGuiUIWindow::Impl::ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data) {
	int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0)
		return;

	if (draw_data->CmdListsCount <= 0)
		return;

	// Backup GL state
	GLenum last_active_texture;		 glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
	glActiveTexture(GL_TEXTURE0);
	GLuint last_program;			 glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&last_program);
	GLuint last_texture;			 glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&last_texture);

	GLuint last_array_buffer;		 glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&last_array_buffer);
	GLuint last_vertex_array_object; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&last_vertex_array_object);
	GLint  last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer); // no need ???

	GLint last_viewport[4];				glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4];			glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLenum last_blend_src_rgb;			glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
	GLenum last_blend_dst_rgb;			glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
	GLenum last_blend_src_alpha;		glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
	GLenum last_blend_dst_alpha;		glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
	GLenum last_blend_equation_rgb;		glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
	GLenum last_blend_equation_alpha;	glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);

	GLboolean last_enable_blend			= glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face		= glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test	= glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_stencil_test	= glIsEnabled(GL_STENCIL_TEST);
//	GLboolean last_enable_scissor_test	= glIsEnabled(GL_SCISSOR_TEST);

	// Setup desired GL state
	// Recreate the VAO every time (this is to easily allow multiple GL contexts to be rendered to. VAO are not shared among GL contexts)
	// The renderer would actually work without any VAO bound, but then our VertexAttrib calls would overwrite the default one currently bound.
	_vertexArray.create();

	ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height);

	// Will project scissor/clipping rectangles into framebuffer space
	ImVec2 clip_off		= draw_data->DisplayPos;       // (0,0) unless using multi-viewports
	ImVec2 clip_scale	= draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

	auto vertexSize = sizeof(VertexType);
	auto indexSize  = sizeof(IndexType);

	// Render command lists
	for (int n = 0; n < draw_data->CmdListsCount; n++) {
		const ImDrawList* draw_list = draw_data->CmdLists[n];
		if (draw_list->IdxBuffer.empty())
			continue;

		auto vtx_buffer_size = draw_list->VtxBuffer.Size * vertexSize;
		auto idx_buffer_size = draw_list->IdxBuffer.Size * indexSize;

#if MyImGuiUIWindow_TEST_RENDER_MESH
		auto vertexData = Span<const u8>(reinterpret_cast<const u8*>(draw_list->VtxBuffer.Data), vtx_buffer_size);
		auto indexData  = Span<const u8>(reinterpret_cast<const u8*>(draw_list->IdxBuffer.Data), idx_buffer_size);
		_mesh.vertexBuffer.uploadToGpu(vertexData);
		_mesh.indexBuffer.uploadToGpu(indexData);

		_mesh.vertexBuffer.bind();

		SGE_GL_CALL(glEnableVertexAttribArray(AttribLocationVtxPos));
		SGE_GL_CALL(glEnableVertexAttribArray(AttribLocationVtxUV));
		SGE_GL_CALL(glEnableVertexAttribArray(AttribLocationVtxColor));
		auto stride = static_cast<GLsizei>(vertexSize);
		SGE_GL_CALL(glVertexAttribPointer(AttribLocationVtxPos,   2, GL_FLOAT,		   GL_FALSE, stride, reinterpret_cast<GLvoid*>(memberOffset(&VertexType::pos))));
		SGE_GL_CALL(glVertexAttribPointer(AttribLocationVtxUV,	  2, GL_FLOAT,		   GL_FALSE, stride, reinterpret_cast<GLvoid*>(memberOffset(&VertexType::uv))));
		SGE_GL_CALL(glVertexAttribPointer(AttribLocationVtxColor, 4, GL_UNSIGNED_BYTE, GL_TRUE,  stride, reinterpret_cast<GLvoid*>(memberOffset(&VertexType::col))));
#else
		GLenum usage = GL_STREAM_DRAW;
		SGE_GL_CALL(glBufferData(GL_ARRAY_BUFFER,		  vtx_buffer_size, (const GLvoid*)draw_list->VtxBuffer.Data, usage));
		SGE_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_size, (const GLvoid*)draw_list->IdxBuffer.Data, usage));
#endif

		for (int cmd_i = 0; cmd_i < draw_list->CmdBuffer.Size; cmd_i++) {
			const ImDrawCmd* pcmd = &draw_list->CmdBuffer[cmd_i];
			GLsizei indexCount = pcmd->ElemCount;

			// Project scissor/clipping rectangles into framebuffer space
			ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
			ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);

			if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
				continue;

			// Apply scissor/clipping rectangle (Y is inverted in OpenGL)
			SGE_GL_CALL(glScissor(
				(int)clip_min.x, (int)((float)fb_height - clip_max.y),
				(int)(clip_max.x - clip_min.x), (int)(clip_max.y - clip_min.y)));

			// Bind texture, Draw
			_fontTex->bind(); // SGE_GL_CALL(glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->GetTexID()));
#if MyImGuiUIWindow_TEST_RENDER_MESH
			_mesh.indexBuffer.bind();
#endif
			auto indexBufferOffset = pcmd->IdxOffset * indexSize;
			SGE_GL_CALL(glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void*)(intptr_t)indexBufferOffset));
		}
	}
	_fontTex->unbind();
#if MyImGuiUIWindow_TEST_RENDER_MESH
	_mesh.vertexBuffer.unbind();
	_mesh.indexBuffer.unbind();
#endif
	_shader->unbind();
	_vertexArray.unbind();

	// Restore modified GL state
	// This "glIsProgram()" check is required because if the program is "pending deletion" at the time of binding backup, it will have been deleted by now and will cause an OpenGL error. See #6220.
	if (last_program == 0 || glIsProgram(last_program))
		glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);

	glActiveTexture(last_active_texture);
	glBindVertexArray(last_vertex_array_object);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer); // no need ???

	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
	if (last_enable_blend)			glEnable(GL_BLEND);		   else glDisable(GL_BLEND);
	if (last_enable_cull_face)		glEnable(GL_CULL_FACE);	   else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test)		glEnable(GL_DEPTH_TEST);   else glDisable(GL_DEPTH_TEST);
	if (last_enable_stencil_test)	glEnable(GL_STENCIL_TEST); else glDisable(GL_STENCIL_TEST);
//	if (last_enable_scissor_test)	glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);

	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

void MyImGuiUIWindow::Impl::ImGui_ImplOpenGL3_SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height) {
	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	glEnable(GL_SCISSOR_TEST); // must enable GL_SCISSOR_TEST each frame

	// Setup viewport, orthographic projection matrix
	// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
	SGE_GL_CALL(glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height));
	float L = draw_data->DisplayPos.x;
	float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	float T = draw_data->DisplayPos.y;
	float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

	Mat4f ortho_projectio = {
		{ 2.0f / (R - L),   0.0f,				  0.0f,   0.0f },
		{ 0.0f,				2.0f / (T - B),		  0.0f,   0.0f },
		{ 0.0f,				0.0f,				  -1.0f,  0.0f },
		{ (R + L) / (L - R),  (T + B) / (B - T),  0.0f,   1.0f },
	};
	_shader->bind(); //glUseProgram(bd->ShaderHandle);
	_shader->setUniform("matMVP", ortho_projectio);
	_shader->setUniform("sampler0", _fontTex);

	auto vertexSize = sizeof(VertexType);
	auto indexSize  = sizeof(IndexType);

	SGE_ASSERT(indexSize == sizeof(u16));

	_vertexArray.bind();
	// Bind vertex/index buffers and setup attributes for ImDrawVert
#if MyImGuiUIWindow_TEST_RENDER_MESH
	_mesh.vertexCount = draw_data->TotalVtxCount;
	_mesh.indexCount = draw_data->TotalIdxCount;

	_mesh.vertexBuffer.create(draw_data->TotalVtxCount * vertexSize);
	_mesh.indexBuffer.create(draw_data->TotalIdxCount * indexSize);
#else
	SGE_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VboHandle));
	SGE_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementsHandle));
	SGE_GL_CALL(glEnableVertexAttribArray(AttribLocationVtxPos));
	SGE_GL_CALL(glEnableVertexAttribArray(AttribLocationVtxUV));
	SGE_GL_CALL(glEnableVertexAttribArray(AttribLocationVtxColor));
	auto stride = static_cast<GLsizei>(vertexSize);
	SGE_GL_CALL(glVertexAttribPointer(AttribLocationVtxPos,   2, GL_FLOAT,		   GL_FALSE, stride, reinterpret_cast<GLvoid*>(memberOffset(&VertexType::pos))));
	SGE_GL_CALL(glVertexAttribPointer(AttribLocationVtxUV,	  2, GL_FLOAT,		   GL_FALSE, stride, reinterpret_cast<GLvoid*>(memberOffset(&VertexType::uv))));
	SGE_GL_CALL(glVertexAttribPointer(AttribLocationVtxColor, 4, GL_UNSIGNED_BYTE, GL_TRUE,  stride, reinterpret_cast<GLvoid*>(memberOffset(&VertexType::col))));
#endif
}

void MyImGuiUIWindow::Impl::_createFontTex() {
	if (_fontTex) return;
	_fontTex = new MyTexture2D();
	unsigned char* pixels;
	int w, h;
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->GetTexDataAsRGBA32(&pixels, &w, &h);
	_fontTex->create(w, h, reinterpret_cast<Color4b*>(pixels));
}

int MyImGuiUIWindow::Impl::_mouseButton(UIMouseEventButton v) {
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

bool MyImGuiUIWindow::Impl::_imgui_onUIMouseEvent(UIMouseEvent& ev) {
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

bool MyImGuiUIWindow::Impl::_imgui_onUIKeyboardEvent(UIKeyboardEvent& ev) {
    return false; // TODO
}

MyImGuiUIWindow::MyImGuiUIWindow(CreateDesc& desc)
	: _windowActive(false)
{
	_impl = new Impl(this, desc);
}

MyImGuiUIWindow::~MyImGuiUIWindow() {
	auto* p = _impl;
	_impl = nullptr;
	if (p) {
		p->_owner = nullptr;
		sge_delete(p);
	}
}

MyOpenGLNativeUIWindow* MyImGuiUIWindow::getNativeWindow() {
	return _impl;
}

void MyImGuiUIWindow::setWindowTitle(StrView title) {
	if (_impl) {
		_impl->setWindowTitle(title);
	}
}

void MyImGuiUIWindow::_onWindowActive(bool b) {
	_windowActive = b;
	onWindowActive(b);
}

void MyImGuiUIWindow::update(float dt) {
	if (_impl) {
		_impl->onUpdate(dt);
		_impl->onRender();
	}
}

} // namespace sge
