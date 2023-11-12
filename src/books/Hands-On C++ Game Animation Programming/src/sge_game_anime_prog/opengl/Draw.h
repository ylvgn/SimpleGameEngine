#pragma once

#include <sge_game_anime_prog/opengl/OpenGL_Common.h>
#include "glad/glad.h"
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
enum class DrawMode {
	Points,
	LineStrip,
	LineLoop,
	Lines,
	Triangles,
	TriangleStrip,
	TriangleFan
};

struct DrawUtil {
	DrawUtil() = delete;

	static GLenum getGLDrawMode(DrawMode v);

	static void draw(size_t vertexCount, DrawMode mode = DrawMode::Triangles);
	static void drawInstanced(size_t vertexCount, size_t instanceCount, DrawMode mode = DrawMode::Triangles);

	static void draw(const IndexBuffer& indexBuf, DrawMode mode = DrawMode::Triangles);
	static void drawInstanced(const IndexBuffer& indexBuf, size_t instanceCount, DrawMode mode = DrawMode::Triangles);
}; // DrawUtil

}