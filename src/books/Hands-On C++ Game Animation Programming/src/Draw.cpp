#include "Draw.h"

namespace sge {

void g_Draw(size_t vertexCount, DrawMode mode) {
	glDrawArrays(DrawUtil::getGLEnum(mode), 0, static_cast<GLsizei>(vertexCount));
}

void g_DrawInstanced(size_t vertexCount, DrawMode mode, u32 numInstances) {
	glDrawArraysInstanced(DrawUtil::getGLEnum(mode), 0, static_cast<GLsizei>(vertexCount), numInstances);
}

void g_Draw(const IndexBuffer& indexBuf, DrawMode mode) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf.handle());
	{
		glDrawElements(DrawUtil::getGLEnum(mode), indexBuf.count(), GL_UNSIGNED_INT, 0);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void g_DrawInstanced(const IndexBuffer& indexBuf, DrawMode mode, u32 instanceCount) {
	// This instanceCount variable controls how many instances of the geometry will be rendered
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf.handle());
	{
		glDrawElementsInstanced(DrawUtil::getGLEnum(mode), indexBuf.count(), GL_UNSIGNED_INT, 0, instanceCount);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}