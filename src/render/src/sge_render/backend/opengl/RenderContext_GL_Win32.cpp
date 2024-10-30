#include "RenderContext_GL_Win32.h"

#if SGE_OS_WINDOWS
#if SGE_RENDER_HAS_OPENGL

#include "Renderer_GL.h"
#include "RenderGpuBuffer_GL.h"
#include "Shader_GL.h"

namespace sge {

const wchar_t* RenderContext_GL_Win32::kClassName = L"RenderContext_GL_Win32";

#if 0
#pragma mark ========= RenderContext_GL_Win32::FalseContext ============
#endif
// https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)#Create_a_False_Context
// The key problem is this: the function you use to get WGL extensions is, itself, an OpenGL extension
class RenderContext_GL_Win32::FalseContext : public NonCopyable {
public:
	void FalseContext::create() {
		destroy();

		auto hInstance = GetModuleHandle(nullptr);

		WNDCLASSEX wc;
		bool isRegistered = (0 != GetClassInfoEx(hInstance, kClassName, &wc));
		if (!isRegistered) {
			memset(&wc, 0, sizeof(wc));

			wc.cbSize			= sizeof(wc);
			wc.hInstance		= hInstance;
			wc.style			= CS_OWNDC;
			wc.lpfnWndProc		= DefWindowProc;
			wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
			wc.hbrBackground	= nullptr;
			wc.lpszMenuName		= nullptr;
			wc.lpszClassName	= kClassName;

			if (!RegisterClassEx(&wc))
				throw SGE_ERROR("RegisterClassEx");
		}

		DWORD dwExStyle = 0;
		DWORD dwStyle	= WS_POPUP;

		_hwnd = CreateWindowEx(dwExStyle, kClassName, kClassName, dwStyle,
							   0,0,0,0,
							   nullptr, nullptr, hInstance, nullptr);
		if (!_hwnd)
			throw SGE_ERROR("CreateWindowEx");

		_dc = GetDC(_hwnd);
		if (!_dc)
			throw SGE_ERROR("GetDC");

		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(pfd));
		pfd.nSize		= sizeof(pfd);
		pfd.nVersion	= 1;
		pfd.dwFlags		= PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
		pfd.iPixelType	= PFD_TYPE_RGBA;
		pfd.cColorBits	= 32;
		pfd.cDepthBits	= 32;
		pfd.iLayerType	= PFD_MAIN_PLANE;

		int nPixelFormat = ChoosePixelFormat(_dc, &pfd);
		if (!nPixelFormat)
			throw SGE_ERROR("ChoosePixelFormat");

		if (!SetPixelFormat(_dc, nPixelFormat, &pfd))
			throw SGE_ERROR("SetPixelFormat");

		_rc = wglCreateContext(_dc);
		if (!_rc)
			throw SGE_ERROR("wglCreateContext");

		if (!wglMakeCurrent(_dc, _rc))
			throw SGE_ERROR("wglMakeCurrent");
	}

	void FalseContext::destroy() {
		if (_rc) {
			wglDeleteContext(_rc);
			_rc = nullptr;
		}
		if (_dc) {
			SGE_ASSERT(_hwnd != nullptr);
			ReleaseDC(_hwnd, _dc);
			_dc = nullptr;
		}
		if (_hwnd) {
			DestroyWindow(_hwnd);
			_hwnd = nullptr;
		}
	}

	~FalseContext() { destroy(); }

private:
	HWND	_hwnd	= nullptr;
	HDC		_dc		= nullptr;
	HGLRC	_rc		= nullptr;
};

#if 0
#pragma mark ========= RenderContext_GL_Win32 ============
#endif
RenderContext_GL_Win32::RenderContext_GL_Win32(CreateDesc& desc)
	: Base(desc)
	, _renderer(Renderer_GL::current())
	, _window(static_cast<NativeUIWindow_Win32*>(desc.window))
{
	FalseContext falseContext;

	/*TODO: create more than 1 render context*/
	falseContext.create();
	glewInit();

	if (!_window) {
		/*TODO: create more than 1 render context*/
	}
	else {
		_win32_dc = GetDC(_window->_hwnd);
	}

	SGE_ASSERT(_win32_dc != nullptr);

	const int formatAttrs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB,	GL_TRUE,
		WGL_PIXEL_TYPE_ARB,		WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB,		32,
		WGL_DEPTH_BITS_ARB,		24,
		WGL_STENCIL_BITS_ARB,	8,
		0 // End of attributes list
	};

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	int contextAttrs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major, // 3
		WGL_CONTEXT_MINOR_VERSION_ARB, minor, // 3

		WGL_CONTEXT_PROFILE_MASK_ARB,	WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		WGL_CONTEXT_FLAGS_ARB,			WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0 // End of attributes list
	};
	
	int format, numFormat;
	if (!wglChoosePixelFormatARB(_win32_dc, formatAttrs, nullptr, 1, &format, reinterpret_cast<UINT*>(&numFormat))) {
		throw SGE_ERROR("wglChoosePixelFormatARB");
	}

	PIXELFORMATDESCRIPTOR pfd; SGE_UNUSED(pfd);
	if (!SetPixelFormat(_win32_dc, format, &pfd))
		throw SGE_ERROR("SetPixelFormat");

	HGLRC sharedContext = nullptr;
	_win32_rc = wglCreateContextAttribsARB(_win32_dc, sharedContext, contextAttrs);
	if (!_win32_rc) {
		Util::throwIfError();
		throw SGE_ERROR("wglCreateContextAttribsARB");
	}

	if (!wglMakeCurrent(_win32_dc, _win32_rc))
		throw SGE_ERROR("wglMakeCurrent");

	Util::throwIfError();
}

RenderContext_GL_Win32::~RenderContext_GL_Win32() {
	_destroyTestShaders();
#if MY_TEST_FRAMEBUFFER
	_destroyTestScreenFrameBuffer();
#endif
	if (_win32_rc) {
		wglDeleteContext(_win32_rc);
		_win32_rc = nullptr;
	}
	if (_win32_dc) {
		ReleaseDC(_window->_hwnd, _win32_dc);
		_win32_dc = nullptr;
	}
}

void RenderContext_GL_Win32::onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd) {
	GLenum clearFlag = 0;
	if (cmd.color.has_value()) {
		glClearColor(cmd.color->r, cmd.color->g, cmd.color->b, cmd.color->a);
		clearFlag |= GL_COLOR_BUFFER_BIT;
	}
	if (cmd.depth.has_value()) {
		glClearDepth(*cmd.depth);
		clearFlag |= GL_DEPTH_BUFFER_BIT;
	}
	if (cmd.stencil.has_value()) {
		glClearStencil(GLint(*cmd.stencil));
		clearFlag |= GL_STENCIL_BUFFER_BIT;
	}
	glClear(clearFlag);

	Util::throwIfError();
}

void RenderContext_GL_Win32::onCmd_DrawCall(RenderCommand_DrawCall& cmd) {
	if (!cmd.vertexLayout) { SGE_ASSERT(false); return; }

	auto* vertexBuffer = static_cast<RenderGpuBuffer_GL*>(cmd.vertexBuffer.ptr());
	RenderGpuBuffer_GL* indexBuffer = nullptr;

	if (!vertexBuffer && cmd.vertexCount <= 0) { SGE_ASSERT(false); return; }
	if (cmd.primitive == RenderPrimitiveType::None) { SGE_ASSERT(false); return; }

	if (cmd.indexCount > 0) {
		indexBuffer = static_cast<RenderGpuBuffer_GL*>(cmd.indexBuffer.ptr());
		if (!indexBuffer) { SGE_ASSERT(false); return; }
	}

	vertexBuffer->bind();
	{
		if (auto* pass = cmd.getMaterialPass()) {
			pass->bind(this, cmd.vertexLayout);
		} else {
			_setTestShaders(cmd.vertexLayout);
		}
		auto	primitive	= Util::getGlPrimitiveTopology(cmd.primitive);
		GLsizei stride		= static_cast<GLsizei>(cmd.vertexLayout->stride); SGE_UNUSED(stride);
		GLsizei vertexCount = static_cast<GLsizei>(cmd.vertexCount);
		GLsizei  indexCount = static_cast<GLsizei>(cmd.indexCount);

		if (indexCount > 0) {
			indexBuffer->bind();
			glDrawElements(primitive, indexCount, Util::getGlFormat(cmd.indexType), nullptr);
			indexBuffer->unbind();
		} else {
			glDrawArrays(primitive, 0, static_cast<GLsizei>(vertexCount));
		}
	}
	vertexBuffer->unbind();

	Util::throwIfError();
}

void RenderContext_GL_Win32::onCmd_SetScissorRect(RenderCommand_SetScissorRect& cmd) {
	auto rc = cmd.rect; // TODO some bug, cuz glScissor is global space, but how to fix it ???
	glScissor(GLint(rc.x), GLint(rc.y), GLsizei(rc.w), GLsizei(rc.h));
}

void RenderContext_GL_Win32::onCmd_SwapBuffers(RenderCommand_SwapBuffers& cmd) {
	if (_win32_dc)
		SwapBuffers(_win32_dc);
	Util::throwIfError();
}

void RenderContext_GL_Win32::onCommit(RenderCommandBuffer& cmdBuf) {
	_dispatch(this, cmdBuf);
}

void RenderContext_GL_Win32::onSetFrameBufferSize(const Vec2f& newSize) {
	int newWidth = int(newSize.x);
	int newHeight = int(newSize.y);
#if MY_TEST_FRAMEBUFFER
	// TODO: reset frame buffer size
	if (_viewFramebuffer && newWidth > 0 && newHeight > 0) {
		_createBuffers();
	}
#endif
	glViewport(0, 0, newWidth, newHeight);
	Util::throwIfError();
}

void RenderContext_GL_Win32::_destroyBuffers() {
	if (_viewFramebuffer) {
		glDeleteFramebuffers(1, &_viewFramebuffer);
		_viewFramebuffer = 0;
	}
	if (_viewRenderbuffer) {
		glDeleteRenderbuffers(1, &_viewRenderbuffer);
		_viewRenderbuffer = 0;
	}
	if (_depthRenderbuffer) {
		glDeleteRenderbuffers(1, &_depthRenderbuffer);
		_depthRenderbuffer = 0;
	}
	if (!_testScreenQuadTexturebuffer) { // TODO tmp
		glDeleteTextures(1, &_testScreenQuadTexturebuffer);
		_testScreenQuadTexturebuffer = 0;
	}
}

void RenderContext_GL_Win32::onBeginRender() {
	_vao.bind();
#if MY_TEST_FRAMEBUFFER
	if (!_viewFramebuffer)
		_createBuffers();
	glBindFramebuffer(GL_FRAMEBUFFER, _viewFramebuffer);
	Util::throwIfError();
#endif
}

void RenderContext_GL_Win32::onEndRender() {
#if MY_TEST_FRAMEBUFFER
	_setTestFrameBufferScreenShaders();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, _testScreenQuadTexturebuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
#endif
}

void RenderContext_GL_Win32::_setTestFrameBufferScreenShaders() {
	static const char* s_vs_source = "#version 330\n"
		"layout(location = 0) in vec2 i_positionOS;\n"
		"layout(location = 1) in vec2 i_uv;\n"
		"out vec2 uv;\n"
		"void main()\n"
		"{\n"
			"gl_Position = vec4(i_positionOS.xy, 0.0, 1.0);\n"
			"uv = i_uv;\n"
		"}";

	static const char* s_ps_source = "#version 330\n"
		"uniform sampler2D _73;\n"
		"in vec2 uv;\n"
		"layout(location = 0) out vec4 _entryPointOutput;\n"
		"void main()\n"
		"{\n"
			"_entryPointOutput = vec4(texture(_73, uv).xyz, 1.0);\n"
		"}";

	static float s_quadVertices[] = {
		// pos(xy)     // uv
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	if (!_testFrameBufferVertexShader) {
		auto& shader = _testFrameBufferVertexShader;

		shader = glCreateShader(GL_VERTEX_SHADER);
		Util::throwIfError();

		GLint sourceLen = static_cast<GLint>(charStrlen(s_vs_source));
		glShaderSource(shader, 1, &s_vs_source, &sourceLen);
		glCompileShader(shader);
		Util::throwIfError();
	}

	if (!_testFrameBufferPixelShader) {
		auto& shader = _testFrameBufferPixelShader;

		shader = glCreateShader(GL_FRAGMENT_SHADER);
		Util::throwIfError();

		GLint sourceLen = static_cast<GLint>(charStrlen(s_ps_source));
		glShaderSource(shader, 1, &s_ps_source, &sourceLen);
		glCompileShader(shader);
		Util::throwIfError();
	}

	if (!_testFrameBufferShaderProgram)
		Util::linkShader(_testFrameBufferShaderProgram, _testFrameBufferVertexShader, _testFrameBufferPixelShader);

//---- use shader program
	glUseProgram(_testFrameBufferShaderProgram);
	Util::throwIfError();

	if (!_testScreenQuadVertexbuffer) {
		glGenBuffers(1, &_testScreenQuadVertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _testScreenQuadVertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(s_quadVertices), &s_quadVertices, GL_STATIC_DRAW);
		Util::throwIfError();
	}

	constexpr size_t stride = 4 * sizeof(float);

	glBindBuffer(GL_ARRAY_BUFFER, _testScreenQuadVertexbuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (const void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(2 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	Util::throwIfError();
}

void RenderContext_GL_Win32::_setTestShaders(const VertexLayout* vertexLayout) {
	TempString shaderFile("Assets/Shaders/test.hlsl");

	//---- compile shader
	{
		if (!_testVertexShader) {
			TempString tmp(shaderFile + ".spv_vs.vert");
			Util::compileShader(_testVertexShader, Util::getGlShaderType(ShaderStageMask::Vertex), tmp);
		}
	}
	{
		if (!_testPixelShader) {
			TempString tmp(shaderFile + ".spv_ps.frag");
			Util::compileShader(_testPixelShader, Util::getGlShaderType(ShaderStageMask::Pixel), tmp);
		}
	}

	if (!_testShaderProgram)
		Util::linkShader(_testShaderProgram, _testVertexShader, _testPixelShader);

//---- use shader program
	glUseProgram(_testShaderProgram);
	Util::throwIfError();

	GLsizei stride = static_cast<GLsizei>(vertexLayout->stride);
	auto* pos	= vertexLayout->find(VertexSemantic::POSITION);
	auto* color = vertexLayout->find(VertexSemantic::COLOR0);

//	Util::dumpActiveAttrib(_testShaderProgram);

	glVertexAttribPointer(0, 3, GL_FLOAT,			true, stride, reinterpret_cast<const void*>(pos->offset));
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE,	true, stride, reinterpret_cast<const void*>(color->offset));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	Util::throwIfError();
}

void RenderContext_GL_Win32::_createBuffers() {
	_destroyBuffers();

	GLint width  = GLint(_frameBufferSize.x);
	GLint height = GLint(_frameBufferSize.y);

	// TODO
	// view render buffer
	glGenFramebuffers(1, &_viewFramebuffer);
	//glGenRenderbuffers(1, &_viewRenderbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _viewFramebuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, _viewRenderbuffer);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _viewRenderbuffer);
	Util::throwIfError();

	SGE_ASSERT(width > 0 && height > 0);
	glGenTextures(1, &_testScreenQuadTexturebuffer);
	glBindTexture(GL_TEXTURE_2D, _testScreenQuadTexturebuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _testScreenQuadTexturebuffer, 0);
	Util::throwIfError();

	// depth/stencil render buffer
	bool needDepthBuffer = true;
	if (needDepthBuffer) {
		glGenRenderbuffers(1, &_depthRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthRenderbuffer);
		Util::throwIfError();
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw SGE_ERROR("failed to create frame buffer");
	}
}

void RenderContext_GL_Win32::_destroyTestShaders() {
	if (_testVertexShader) {
		glDeleteShader(_testVertexShader);
		_testVertexShader = 0;
	}
	if (_testPixelShader) {
		glDeleteShader(_testPixelShader);
		_testPixelShader = 0;
	}
	if (_testShaderProgram) {
		glDeleteProgram(_testShaderProgram);
		_testShaderProgram = 0;
	}
}

void RenderContext_GL_Win32::_destroyTestScreenFrameBuffer() {
	if (!_testScreenQuadVertexbuffer) {
		glDeleteBuffers(1, &_testScreenQuadVertexbuffer);
		_testScreenQuadVertexbuffer = 0;
	}
	if (_testScreenQuadRenderbuffer) {
		glDeleteRenderbuffers(1, &_testScreenQuadRenderbuffer);
		_testScreenQuadRenderbuffer = 0;
	}
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL
#endif // SGE_OS_WINDOWS