#pragma once

#include <sge_game_anime_prog/opengl/OpenGL_Common.h>
#include "IndexBuffer.h"

namespace sge {
/*
	You have classes for dealing with vertex data, uniforms, and index buffers, but no code to draw any of it.
	Drawing will be handled by four global functions.
	You will have two Draw functions and two DrawInstanced functions.
	ps: Draw.h is provided helper function only.
*/

// most of the time you will only need lines, points, or triangles,
// but some additional types may be useful
#define DrawMode_ENUM_LIST(E) \
	E(Points,			= GL_POINTS) \
	E(Lines,			= GL_LINES) \
	E(LineLoop,			= GL_LINE_LOOP) \
	E(LineStrip,		= GL_LINE_STRIP) \
	E(Triangles,		= GL_TRIANGLES) \
	E(TriangleStrip,	= GL_TRIANGLE_STRIP) \
	E(TriangleFan,		= GL_TRIANGLE_FAN) \
//----
SGE_ENUM_CLASS(DrawMode, u8)

struct DrawUtil {
	DrawUtil() = delete;

	static void draw(size_t vertexCount, DrawMode mode = DrawMode::Triangles);
	static void drawInstanced(size_t vertexCount, size_t instanceCount, DrawMode mode = DrawMode::Triangles);

	static void draw(const IndexBuffer& indexBuf, DrawMode mode = DrawMode::Triangles);
	static void drawInstanced(const IndexBuffer& indexBuf, size_t instanceCount, DrawMode mode = DrawMode::Triangles);
}; // DrawUtil

}