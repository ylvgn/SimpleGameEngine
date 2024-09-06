#include "RenderContext_GL3.h"
#include "RenderFalseContext_GL3.h"
#include "RenderGpuBuffer_GL3.h"

namespace sge {

RenderContext_GL3::RenderContext_GL3(CreateDesc& desc)
	: Base(desc)
	, _renderer(Renderer_GL3::current())
{
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
}

void RenderContext_GL3::onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd) {
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

void RenderContext_GL3::onCmd_DrawCall(RenderCommand_DrawCall& cmd) {
	if (!cmd.vertexLayout) { SGE_ASSERT(false); return; }

	auto* vertexBuffer = static_cast<RenderGpuBuffer_GL3*>(cmd.vertexBuffer.ptr());
	RenderGpuBuffer_GL3* indexBuffer = nullptr;

	if (!vertexBuffer && cmd.vertexCount <= 0) { SGE_ASSERT(false); return; }
	if (cmd.primitive == RenderPrimitiveType::None) { SGE_ASSERT(false); return; }

	if (cmd.indexCount > 0) {
		indexBuffer = static_cast<RenderGpuBuffer_GL3*>(cmd.indexBuffer.ptr());
		if (!indexBuffer) { SGE_ASSERT(false); return; }
	}

	if (!_testVertexArrayObject) { // TODO
		glGenVertexArrays(1, &_testVertexArrayObject);
		Util::throwIfError();
	}
	glBindVertexArray(_testVertexArrayObject);

	vertexBuffer->glBind();
	{
		if (auto* pass = cmd.getMaterialPass()) {
			pass->bind(this, cmd.vertexLayout);
		}
		else {
			_setTestShaders(cmd.vertexLayout);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe ON tmp

		auto primitive		= Util::getGlPrimitiveTopology(cmd.primitive);
//		GLsizei stride		= static_cast<GLsizei>(cmd.vertexLayout->stride);
		GLsizei vertexCount = static_cast<GLsizei>(cmd.vertexCount);
		GLsizei  indexCount = static_cast<GLsizei>(cmd.indexCount);

		if (indexCount > 0) {
			indexBuffer->glBind();
			glDrawElements(primitive, indexCount, Util::getGlFormat(cmd.indexType), nullptr);
			indexBuffer->glUnbind();
		}
		else {
			glDrawArrays(primitive, 0, static_cast<GLsizei>(vertexCount));
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // wireframe Off tmp
	}
	vertexBuffer->glUnbind();

	Util::throwIfError();
}

void RenderContext_GL3::onCmd_SwapBuffers(RenderCommand_SwapBuffers& cmd) {
#if SGE_OS_WINDOWS
	if (_win32_dc)
		::SwapBuffers(_win32_dc);
#endif
	Util::throwIfError();
}

void RenderContext_GL3::onCommit(RenderCommandBuffer& cmdBuf) {
	_dispatch(this, cmdBuf);
}

void RenderContext_GL3::onSetFrameBufferSize(const Vec2f& newSize) {
	glViewport(0, 0, static_cast<GLsizei>(newSize.x), static_cast<GLsizei>(newSize.y));
	Util::throwIfError();
}

void RenderContext_GL3::onBeginRender() {

}

void RenderContext_GL3::onEndRender() {

}

void RenderContext_GL3::_setTestShaders(const VertexLayout* vertexLayout) {
	TempString shaderFile("Assets/Shaders/test.hlsl");

//---- compile shader
	{
		if (!_testVertexShader) {
			TempString tmp(shaderFile + ".spv_vs.vert");
			Util::compileShader(_testVertexShader, GL_VERTEX_SHADER, tmp.c_str());
			Util::throwIfError();
		}
	}
	{
		if (!_testPixelShader) {
			TempString tmp(shaderFile + ".spv_ps.frag");
			Util::compileShader(_testPixelShader, GL_FRAGMENT_SHADER, tmp.c_str());
			Util::throwIfError();
		}
	}

//---- link shader program
	if (!_testShaderProgram) {
		_testShaderProgram = glCreateProgram();
		Util::throwIfError();

		if (_testVertexShader) {
			glAttachShader(_testShaderProgram, _testVertexShader);
			Util::throwIfError();
		}
		if (_testPixelShader) {
			glAttachShader(_testShaderProgram, _testPixelShader);
			Util::throwIfError();
		}
		
		glLinkProgram(_testShaderProgram);
		GLint linked;
		glGetProgramiv(_testShaderProgram, GL_LINK_STATUS, &linked);
		if (linked != GL_TRUE) {
			String errmsg;
			Util::getProgramInfoLog(_testShaderProgram, errmsg);

			TempString tmp;
			FmtTo(tmp, "Error link shader filename = {}\n{}", shaderFile, errmsg);

			throw SGE_ERROR("{}", tmp.c_str());
		}
		Util::throwIfError();

		//---- validate shader program
		glValidateProgram(_testShaderProgram);
		GLint validated;
		glGetProgramiv(_testShaderProgram, GL_VALIDATE_STATUS, &validated);
		if (validated != GL_TRUE) {
			String errmsg;
			Util::getProgramInfoLog(_testShaderProgram, errmsg);

			TempString tmp;
			FmtTo(tmp, "Error validate shader filename = {}\n{}", shaderFile, errmsg);

			throw SGE_ERROR("{}", tmp.c_str());
		}
		Util::throwIfError();

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

void RenderContext_GL3::_destroy() {
	_destroyTestShaders();
	if (_testVertexArrayObject) {
		glDeleteVertexArrays(1, &_testVertexArrayObject);
		_testVertexArrayObject = 0;
	}
	glBindVertexArray(0);
}

void RenderContext_GL3::_destroyTestShaders() {
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

}
