#include "Material_GL.h"
#include "RenderContext_GL.h"

namespace sge {

template<class STAGE>
void Material_GL::_bindStageHelper(RenderContext_GL* ctx, STAGE* stage) {
	auto* shaderStage = stage->shaderStage();
	if (!shaderStage) return;
	shaderStage->bind(ctx);

	const auto& program = stage->shaderPass()->program();

	for (auto& cb : stage->constBuffers()) {
		auto* gpuBuffer = static_cast<RenderGpuBuffer_GL*>(cb.gpuBuffer.ptr());
		if (!gpuBuffer)
			throw SGE_ERROR("cosnt buffer is null");

		auto glBuf = gpuBuffer->glBuf();
		if (!glBuf)
			throw SGE_ERROR("gl buffer is null");

		auto* cbInfo = cb.info();

		GLuint ubIndex = glGetUniformBlockIndex(program, cbInfo->name.c_str());
		if (ubIndex == GL_INVALID_INDEX)
			throw SGE_ERROR("shader program not found uniform block '{}'", cbInfo->name);

		GLuint bindPoint = cbInfo->bindPoint;

		stage->_glSetConstBuffer(ubIndex, bindPoint, glBuf);

		cb.uploadToGpu();

		Util::throwIfError();
	}

	Util::throwIfError();
}

void Material_GL::MyVertexStage::bind(RenderContext_GL* ctx, const VertexLayout* vertexLayout) {
	_bindStageHelper(ctx, this);

	bindInputLayout(ctx, vertexLayout);
}

void Material_GL::MyVertexStage::bindInputLayout(RenderContext_GL* ctx, const VertexLayout* vertexLayout) {
	GLsizei stride = static_cast<GLsizei>(vertexLayout->stride);
	
	auto* vsInfo = info();
	for (auto& input : vsInfo->inputs) {
		auto* e = vertexLayout->find(input.semantic);
		if (!e)
			throw SGE_ERROR("vertex sematic {} not found", input.semantic);

		GLsizei size = Util::getComponentCount(e->dataType);
		GLenum	type = Util::getGlBaseFormat(e->dataType);

		glVertexAttribPointer(input.slot, size, type, GL_TRUE, stride, reinterpret_cast<const void*>(e->offset));
		glEnableVertexAttribArray(input.slot);
	}

	Util::throwIfError();
}

void Material_GL::MyPixelStage::bind(RenderContext_GL* ctx, const VertexLayout* vertexLayout) {
	_bindStageHelper(ctx, this);
}

Material_GL::MyPass::MyPass(Material* material, ShaderPass* shaderPass)
	: Base(material, shaderPass)
	, _myVertexStage(this, shaderPass->vertexStage())
	,  _myPixelStage(this, shaderPass->pixelStage())
{
	_vertexStage = &_myVertexStage;
	 _pixelStage = &_myPixelStage;
}

void Material_GL::MyPass::onBind(RenderContext* ctx_, const VertexLayout* vertexLayout) {
	auto* ctx = static_cast<RenderContext_GL*>(ctx_);

	this->shaderPass()->bind(); // use shader program

	_myVertexStage.bind(ctx, vertexLayout);
	 _myPixelStage.bind(ctx, vertexLayout);

//	_bindRenderState(ctx); TODO
}

}