#pragma once

#include "IndexBuffer.h"

/*
	You have classes for dealing with vertex data, uniforms, and index buffers, but no code to draw any of it.
	Drawing will be handled by four global functions.
	You will have two Draw functions and two DrawInstanced functions.
	ps: Draw.h is provided helper function only.
*/

namespace sge {

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
	static GLenum getGLEnum(DrawMode mode);
};

GLenum DrawUtil::getGLEnum(DrawMode mode) {
	switch (mode)
	{
		case DrawMode::Points:			return GL_POINTS;
		case DrawMode::LineStrip:		return GL_LINE_STRIP;
		case DrawMode::LineLoop:		return GL_LINE_LOOP;
		case DrawMode::Lines:			return GL_LINES;
		case DrawMode::Triangles:		return GL_TRIANGLES;
		case DrawMode::TriangleStrip:	return GL_TRIANGLE_STRIP;
		case DrawMode::TriangleFan:		return GL_TRIANGLE_FAN;
		default:						throw  SGE_ERROR("unsupported DrawMode");
	}
}

void g_Draw(size_t vertexCount, DrawMode mode);
void g_DrawInstanced(size_t vertexCount, DrawMode mode, u32 numInstances);

void g_Draw(const IndexBuffer& indexBuf, DrawMode mode);
void g_DrawInstanced(const IndexBuffer& indexBuf, DrawMode mode, u32 instanceCount);

}