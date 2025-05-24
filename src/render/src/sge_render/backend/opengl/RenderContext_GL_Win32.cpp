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

		auto hInstance = ::GetModuleHandle(nullptr);

		::WNDCLASSEX wc;
		bool isRegistered = (0 != ::GetClassInfoEx(hInstance, kClassName, &wc));
		if (!isRegistered) {
			memset(&wc, 0, sizeof(wc));

			wc.cbSize			= sizeof(wc);
			wc.hInstance		= hInstance;
			wc.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc		= DefWindowProc;
			wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
			wc.hbrBackground	= nullptr;
			wc.lpszMenuName		= nullptr;
			wc.lpszClassName	= kClassName;

			if (!RegisterClassEx(&wc))
				throw SGE_ERROR("RegisterClassEx");
		}

		::DWORD dwExStyle = 0;
		::DWORD dwStyle	= WS_POPUP;

		_hwnd = ::CreateWindowEx(dwExStyle, kClassName, kClassName, dwStyle,
							   0,0,0,0,
							   nullptr, nullptr, hInstance, nullptr);
		if (!_hwnd)
			throw SGE_ERROR("CreateWindowEx");

		_dc = ::GetDC(_hwnd);
		if (!_dc)
			throw SGE_ERROR("GetDC");

		::PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(pfd));
		pfd.nSize		= sizeof(pfd);
		pfd.nVersion	= 1;
		pfd.dwFlags		= PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
		pfd.iPixelType	= PFD_TYPE_RGBA;
		pfd.cColorBits	= 32;
		pfd.cDepthBits	= 32;
		pfd.iLayerType	= PFD_MAIN_PLANE;

		int nPixelFormat = ::ChoosePixelFormat(_dc, &pfd);
		if (!nPixelFormat)
			throw SGE_ERROR("ChoosePixelFormat");

		if (!::SetPixelFormat(_dc, nPixelFormat, &pfd))
			throw SGE_ERROR("SetPixelFormat");

		_rc = ::wglCreateContext(_dc);
		if (!_rc)
			throw SGE_ERROR("wglCreateContext");

		if (!wglMakeCurrent(_dc, _rc))
			throw SGE_ERROR("wglMakeCurrent");
	}

	void FalseContext::destroy() {
		if (_rc) {
			::wglDeleteContext(_rc);
			_rc = nullptr;
		}
		if (_dc) {
			SGE_ASSERT(_hwnd != nullptr);
			::ReleaseDC(_hwnd, _dc);
			_dc = nullptr;
		}
		if (_hwnd) {
			::DestroyWindow(_hwnd);
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
	, _renderer(Renderer_GL::instance())
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
		auto	primitive	 = Util::getGlPrimitiveTopology(cmd.primitive);
		GLsizei stride		 = static_cast<GLsizei>(cmd.vertexLayout->stride); SGE_UNUSED(stride);
		GLsizei vertexCount  = static_cast<GLsizei>(cmd.vertexCount);
		GLsizei indexCount   = static_cast<GLsizei>(cmd.indexCount);
		GLsizei vertexOffset = static_cast<GLsizei>(cmd.vertexOffset);
		SGE_UNUSED(vertexOffset);

		if (auto* pass = cmd.getMaterialPass()) {
			pass->bind(this, cmd);
		} else {
			_setTestShaders(cmd.vertexLayout);
		}

		if (indexCount > 0) {
			indexBuffer->bind();
			glDrawElements(primitive, indexCount, Util::getGlFormat(cmd.indexType), reinterpret_cast<const void*>(cmd.indexOffset));
			indexBuffer->unbind();
			Util::throwIfError();
		} else {
			glDrawArrays(primitive, 0, static_cast<GLsizei>(vertexCount));
			Util::throwIfError();
		}
	}
	vertexBuffer->unbind();
}

void RenderContext_GL_Win32::onCmd_SetScissorRect(RenderCommand_SetScissorRect& cmd) {
	Rect2f rc = cmd.rect;
	rc.y = _frameBufferSize.y - rc.yMax(); // OpenGL using bottom-left coordinate
	glScissor(GLint(rc.x), GLint(rc.y), GLsizei(rc.w), GLsizei(rc.h));
}
#if 0 // no use now
void RenderContext_GL_Win32::onCmd_SetViewport(RenderCommand_SetViewport& cmd) {
	Rect2f& rc = cmd.rect;
	glViewport(GLint(rc.x), GLint(rc.y), GLsizei(rc.w), GLsizei(rc.h));
	glDepthRange(GLclampd(cmd.depthRange.x), GLclampd(cmd.depthRange.y));
}
#endif
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

	glViewport(0, 0, newWidth, newHeight);
	glScissor(0, 0, newWidth, newHeight);
	Util::throwIfError();
}

void RenderContext_GL_Win32::onBeginRender() {
	_vao.bind();
}

void RenderContext_GL_Win32::onEndRender() {
}

void RenderContext_GL_Win32::_setTestShaders(const VertexLayout* vertexLayout) {
	static bool s_enterOnce = false;

	if (!s_enterOnce) {
		s_enterOnce = true;
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

		if (!_testShaderProgram) {
			_testShaderProgram = glCreateProgram();
			Util::throwIfError();
		}
		if (_testVertexShader) {
			glAttachShader(_testShaderProgram, _testVertexShader);
			Util::throwIfError();
		}
		if (_testPixelShader) {
			glAttachShader(_testShaderProgram, _testPixelShader);
			Util::throwIfError();
		}

		glLinkProgram(_testShaderProgram);
		Util::throwIfError();

		GLint linked;
		glGetProgramiv(_testShaderProgram, GL_LINK_STATUS, &linked);
		if (linked != GL_TRUE) {
			String errmsg;
			Util::getProgramInfoLog(_testShaderProgram, errmsg);

			TempString tmp;
			FmtTo(tmp, "link shader error: {}", errmsg);

			throw SGE_ERROR("{}", tmp.c_str());
		}

		glValidateProgram(_testShaderProgram);
		Util::throwIfError();

		GLint validated;
		glGetProgramiv(_testShaderProgram, GL_VALIDATE_STATUS, &validated);
		if (validated != GL_TRUE) {
			String errmsg;
			Util::getProgramInfoLog(_testShaderProgram, errmsg);

			TempString tmp;
			FmtTo(tmp, "validate shader error: {}", errmsg);

			throw SGE_ERROR("{}", tmp.c_str());
		}
		SGE_ASSERT(glIsProgram(_testShaderProgram) == GL_TRUE);
	}

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

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL
#endif // SGE_OS_WINDOWS