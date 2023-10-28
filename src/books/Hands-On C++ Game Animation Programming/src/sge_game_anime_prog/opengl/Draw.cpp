#include "Draw.h"

namespace sge {

void DrawUtil::draw(size_t vertexCount, DrawMode mode/*= DrawMode::Triangles*/) {
	glDrawArrays(DrawUtil::getGLDrawMode(mode),
				 0,
				 static_cast<GLsizei>(vertexCount)
	);
}

void DrawUtil::drawInstanced(size_t vertexCount, size_t instanceCount, DrawMode mode/*=DrawMode::Triangles*/) {
	glDrawArraysInstanced(DrawUtil::getGLDrawMode(mode),
						  0,
						  static_cast<GLsizei>(vertexCount),
						  static_cast<GLsizei>(instanceCount)
	);
}

void DrawUtil::draw(const IndexBuffer& indexBuf, DrawMode mode/*=DrawMode::Triangles*/) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf.handle());
	{
		glDrawElements(	DrawUtil::getGLDrawMode(mode),
						static_cast<GLuint>(indexBuf.count()),
						OpenGLUtil::getGLFormat<IndexBuffer::DataType>(),
						0
		);
		/*
			Note:
			it depends on what data size upload to gpu:
			when you overwrite the signature:
					IndexBuffer::uploadToGpu(const u32* data, size_t len)
				to	IndexBuffer::uploadToGpu(const u8* data, size_t len)

			be careful that u32(GL_UNSIGNED_INT) is different from u8(GL_UNSIGNED_BYTE)

			you'd better overwrite this impl, like that:
			from:
				IndexBuffer::uploadToGpu(const u32* data, size_t len)
				glDrawElements(DrawUtil::getGLEnum(mode), static_cast<GLsizei>(indexBuf.count()), GL_UNSIGNED_INT, 0);
			to:
				IndexBuffer::uploadToGpu(const u8* data, size_t len)
				glDrawElements(DrawUtil::getGLEnum(mode), static_cast<GLsizei>(indexBuf.count()), GL_UNSIGNED_BYTE, 0);

			and the input data

			from:
				Vector<u32> indices = { 0,1,2, 2,1,3 };
			to:
				Vector<u8> indices = { 0,1,2, 2,1,3 };
		*/
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DrawUtil::drawInstanced(const IndexBuffer& indexBuf, size_t instanceCount, DrawMode mode/*=DrawMode::Triangles*/) {
	// This instanceCount variable controls how many instances of the geometry will be rendered
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf.handle());
	{
		glDrawElementsInstanced(DrawUtil::getGLDrawMode(mode),
								static_cast<GLsizei>(indexBuf.count()),
								OpenGLUtil::getGLFormat<IndexBuffer::DataType>(),
								0,
								static_cast<GLsizei>(instanceCount)
		);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}