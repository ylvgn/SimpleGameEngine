#include "RenderContext_GL3.h"
#include "RenderFalseContext_GL3.h"

namespace sge {

RenderContext_GL3::RenderContext_GL3(CreateDesc& desc)
	: Base(desc)
{
	_renderer = Renderer_GL3::current();

	RenderFalseContext_GL3 falseContext;
	falseContext.create();
	glewInit();

#if SGE_OS_WINDOWS
	auto* win = static_cast<NativeUIWindow_Win32*>(desc.window);
	const auto& win32_hwnd = win->_hwnd;
	SGE_ASSERT(win32_hwnd != nullptr);

	_win32_dc = ::GetDC(win32_hwnd);
	SGE_ASSERT(_win32_dc != nullptr);

	// create context
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

	int contextAttrs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,

		WGL_CONTEXT_PROFILE_MASK_ARB,	WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		WGL_CONTEXT_FLAGS_ARB,			WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0 // End of attributes list
	};
	
	int format, numFormat;
	if (!wglChoosePixelFormatARB(_win32_dc, formatAttrs, nullptr, 1, &format, reinterpret_cast<UINT*>(&numFormat))) {
		throw SGE_ERROR("wglChoosePixelFormatARB");
	}

	::PIXELFORMATDESCRIPTOR pfd;
	if (!::SetPixelFormat(_win32_dc, format, &pfd)) {
		throw SGE_ERROR("SetPixelFormat");
	}

	::HGLRC sharedContext = nullptr;
	_win32_rc = wglCreateContextAttribsARB(_win32_dc, sharedContext, contextAttrs);
	if (!_win32_rc)
		throw SGE_ERROR("wglCreateContextAttribsARB");
	
	if (!::wglMakeCurrent(_win32_dc, _win32_rc))
		throw SGE_ERROR("wglMakeCurrent");
#endif

	Util::throwIfError();

// just for test
	_testShader = new Shader_GL3();
	_testShader->loadFile("Assets/Shaders/test001.cg");
}

void RenderContext_GL3::onSetFrameBufferSize(const Vec2f& newSize) {
	glViewport(0, 0, static_cast<GLsizei>(newSize.x), static_cast<GLsizei>(newSize.y));

	Util::throwIfError();
}

void RenderContext_GL3::testRender() {
	onBeginRender();
		onClearColorAndDepthBuffer();
		onTestDraw();
		onSwapBuffers();
	onEndRender();
}

void RenderContext_GL3::onBeginRender() {
	
}

void RenderContext_GL3::onClearColorAndDepthBuffer() {
	glClearDepth(1);
	glClearStencil(0);
	glClearColor(0.0f, 0.2f, 0.2f, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Util::throwIfError();
}

void RenderContext_GL3::onTestDraw() {
	_testShader->bind();

	struct Vertex {
		Vec4f	pos;
		Color4f color;
	};

	float d = 0.5f;
	static Vertex vertexData[] = {
		{ Vec4f( 0,  d, 0, 1), Color4f( 1,0,0,1 ) },
		{ Vec4f( d, -d, 0, 1), Color4f( 0,1,0,1 ) },
		{ Vec4f(-d, -d, 0, 1), Color4f( 0,0,1,1 ) },
	};

	size_t vertexCount = sizeof(vertexData) / sizeof(vertexData[0]);

	_vertexArray.create();
	_vertexArray.bind();

	MyVertexBuffer vertexBuffer;
	Span<Vertex> vertexDataSpan(vertexData, 3);
	vertexBuffer.create<Vertex>(vertexDataSpan);
	vertexBuffer.bind();

	GLsizei stride = static_cast<GLsizei>(sizeof(Vertex));
	Scoped_RenderContext_GL3_VertexAttrib _pos  (_testShader->_program,	"cg_Vertex",	4, GL_FLOAT, true, stride, memberOffset(&Vertex::pos));
	Scoped_RenderContext_GL3_VertexAttrib _color(_testShader->_program, "COLOR",		4, GL_FLOAT, true, stride, memberOffset(&Vertex::color));

	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount));

	_vertexArray.unbind();
	vertexBuffer.unbind();
	_testShader->unbind();

	Util::throwIfError();
}

void RenderContext_GL3::onEndRender() {

}

void RenderContext_GL3::onSwapBuffers() {
#if SGE_OS_WINDOWS
	if (_win32_dc)
		::SwapBuffers(_win32_dc);
#endif
}

}
