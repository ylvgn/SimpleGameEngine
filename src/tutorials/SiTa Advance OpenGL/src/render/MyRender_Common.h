#pragma once

#include "../MyCommon.h"

#define SGE_GL_CALL(_CALL) do { \
	_CALL; \
	GLenum gl_err = glGetError(); \
	if (gl_err != 0) { \
		SGE_LOG("GL error 0x{:x} returned from '{}'.\n", gl_err, #_CALL); \
		SGE_ASSERT(false); \
	} \
} while (0) \
//----

namespace sge {

enum class RenderPrimitiveType {
	None,
	Points,
	Lines,
	Triangles,
	Quads,
};

inline
void my_throwIfError() {
	auto e = glGetError();
	if (e != GL_NO_ERROR) {
		auto* errStr = reinterpret_cast<const char*>(gluErrorString(e));
		TempString str(errStr);
		throw SGE_ERROR("glGetError = (0x{:X}) {}", static_cast<u32>(e), str);
	}
}

inline
GLenum my_getGlPrimitiveTopology(RenderPrimitiveType v) {
	using SRC = RenderPrimitiveType;
	switch (v) {
		case SRC::Points:		return GL_POINTS;
		case SRC::Lines:		return GL_LINES;
		case SRC::Triangles:	return GL_TRIANGLES;
		case SRC::Quads:		return GL_QUADS;
		default: throw SGE_ERROR("unsupported RenderPrimitiveType");
	}
}

inline
void my_drawGrid() {
	glLineWidth(1);
	glColor4f(0.5f, 0.5f, 0.5f, 1);
	glBegin(GL_LINES);
	for (float x = -10; x <= 10; x++) {
		glVertex3f(x, 0, -10);
		glVertex3f(x, 0, 10);
	}

	for (float z = -10; z <= 10; z++) {
		glVertex3f(-10, 0, z);
		glVertex3f(10, 0, z);
	}
	glEnd();
	glColor4f(1, 1, 1, 1);
}

inline
void my_drawOriginAxis() {
	glLineWidth(2);
	glBegin(GL_LINES);
		glColor4f(1, 0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(1, 0, 0);
		glColor4f(0, 1, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 1, 0);
		glColor4f(0, 0, 1, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1);
	glEnd();

	glColor4f(1, 1, 1, 1);
	glLineWidth(1);
}

}