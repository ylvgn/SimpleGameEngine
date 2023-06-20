#include "Draw.h"

namespace sge {

void DrawUtil::draw(size_t vertexCount, DrawMode mode/*= DrawMode::Triangles*/) {
	glDrawArrays(DrawUtil::getGLEnum(mode), 0, static_cast<GLuint>(vertexCount));
}

void DrawUtil::drawInstanced(size_t vertexCount, u32 instanceCount, DrawMode mode/*=DrawMode::Triangles*/) {
	glDrawArraysInstanced(DrawUtil::getGLEnum(mode), 0, static_cast<GLuint>(vertexCount), instanceCount);
}

void DrawUtil::draw(const IndexBuffer& indexBuf, DrawMode mode/*=DrawMode::Triangles*/) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf.handle());
	{
		glDrawElements(DrawUtil::getGLEnum(mode), static_cast<GLuint>(indexBuf.count()), GL_UNSIGNED_INT, 0);
		/*
			it depends on what data size upload to gpu
			when you overwrite the signature: IndexBuffer::uploadToGpu(const u32* data, size_t len)
			to IndexBuffer::uploadToGpu(const u8* data, size_t len)
			because u32 (GL_UNSIGNED_INT) is different from u8 (GL_UNSIGNED_BYTE)
			you'd better overwrite this impl, like that:
				glDrawElements(DrawUtil::getGLEnum(mode), static_cast<GLuint>(indexBuf.count()), GL_UNSIGNED_BYTE, 0);
			and the input data
			from
				Vector<u32> indices = { 0,1,2, 2,1,3 };
			to
				Vector<u8> indices = { 0,1,2, 2,1,3 };
		*/
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DrawUtil::drawInstanced(const IndexBuffer& indexBuf, u32 instanceCount, DrawMode mode/*=DrawMode::Triangles*/) {
	// This instanceCount variable controls how many instances of the geometry will be rendered
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf.handle());
	{
		glDrawElementsInstanced(DrawUtil::getGLEnum(mode), static_cast<GLuint>(indexBuf.count()), GL_UNSIGNED_INT, 0, instanceCount);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


}