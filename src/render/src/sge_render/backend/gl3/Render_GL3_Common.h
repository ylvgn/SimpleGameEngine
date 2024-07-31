#pragma once

#if SGE_RENDER_HAS_GL3

// GLEW --------
#define GLEW_STATIC 1
#include <GL/glew.h>

#if SGE_OS_WINDOWS
	#include <GL/wglew.h>
#endif
// GLEW --------

#include <GL/gl.h>
#include <GL/glu.h>

#pragma comment(lib, "Opengl32.lib")
#pragma comment(lib, "GLu32.lib")

namespace sge {

class Renderer_GL3;

struct GL3Util {
	GL3Util() = delete;

	static void reportError(GLenum errCode) {
		auto* sz = reinterpret_cast<const char*>(gluErrorString(errCode));
		TempStringW str = UtfUtil::toStringW(sz);
		SGE_LOG("glGetError = (0x{:X}) {}", static_cast<u32>(errCode), str);
	}

	static void throwIfError() {
		auto errCode = glGetError();
		if (_checkError(errCode)) {
			reportError(errCode);
			throw SGE_ERROR("glGetError = (0x{:0X})", errCode);
		}
	}

	static bool assertIfError() {
		auto errCode = glGetError();
		if (_checkError(errCode)) {
			reportError(errCode);
			SGE_ASSERT(false);
			return false;
		}
		return true;
	}

	static GLenum getErrorCode() { return glGetError(); }

private:
	static bool _checkError(GLenum errCode) {
		return errCode != GL_NO_ERROR; // if got error, return true
	}
};

}

#endif