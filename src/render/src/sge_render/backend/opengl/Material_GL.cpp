#include "Material_GL.h"
#include "RenderContext_GL.h"
#include "Texture_GL.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {
#if 0
#pragma mark ========= Material_GL::MyVertexStage ============
#endif
void Material_GL::MyVertexStage::bind(RenderContext_GL* ctx, RenderCommand_DrawCall& drawCall) {
	bindInputLayout(ctx, drawCall);
	s_bindStageHelper(ctx, this);
}

void Material_GL::MyVertexStage::bindInputLayout(RenderContext_GL* ctx, RenderCommand_DrawCall& drawCall) {
	auto* vertexLayout = drawCall.vertexLayout;
	GLsizei stride = static_cast<GLsizei>(vertexLayout->stride);
	
	auto* vsInfo = info();
	for (auto& input : vsInfo->inputs) {
		auto* e = vertexLayout->find(input.semantic);
		if (!e)
			throw SGE_ERROR("vertex sematic {} not found", input.semantic);

		GLsizei size = Util::getComponentCount(e->dataType);
		GLenum	type = Util::getGlBaseFormat(e->dataType);

		auto offset = reinterpret_cast<const void*>(e->offset + drawCall.vertexOffset);

		glEnableVertexAttribArray(input.slot);
		Util::throwIfError();

		const bool normalized = GL_TRUE;
		glVertexAttribPointer(input.slot, size, type, normalized, stride, offset);
		Util::throwIfError();
	}
}


#if 0
#pragma mark ========= Material_GL::MyPixelStage ============
#endif
void Material_GL::MyPixelStage::bind(RenderContext_GL* ctx, RenderCommand_DrawCall& drawCall) {
	s_bindStageHelper(ctx, this);
}


#if 0
#pragma mark ========= Material_GL::MyPass ============
#endif
Material_GL::MyPass::MyPass(Material_GL* material, ShaderPass* shaderPass) noexcept
	: Base(material, shaderPass)
	, _vertexStage(this, shaderPass->vertexStage())
	, _pixelStage (this, shaderPass->pixelStage())
{
	Base::_vertexStage = &_vertexStage;
	Base::_pixelStage  = &_pixelStage;
}

void Material_GL::MyPass::onBind(RenderContext* ctx_, RenderCommand_DrawCall& drawCall) {
	auto* ctx = static_cast<RenderContext_GL*>(ctx_);

	shaderPass()->bind();

	_vertexStage.bind(ctx, drawCall);
	_pixelStage.bind(ctx, drawCall);

	_bindRenderState(ctx);
}

void Material_GL::MyPass::_bindRenderState(RenderContext_GL* ctx) {
	using Cull		= RenderState_Cull;
	using BlendOp	= RenderState_BlendOp;

	const auto& rs = renderState();
// -----
	glPolygonMode(GL_FRONT_AND_BACK, rs.wireframe ? GL_LINE : GL_FILL);

// -----
	if (rs.cull != Cull::None) {
		glEnable(GL_CULL_FACE);

		glFrontFace(GL_CCW); // Initially
		glCullFace(Util::getGlCullMode(rs.cull));
		Util::throwIfError();
	} else {
		glDisable(GL_CULL_FACE);
	}

// -----
	auto& depthTest = rs.depthTest;
	if (depthTest.isEnable()) {
		glEnable(GL_DEPTH_TEST);
		glDepthMask(depthTest.writeMask ? GL_TRUE : GL_FALSE);
		
		auto depthOp = Util::getGlDepthTestOp(depthTest.op);
		glDepthFunc(depthOp);
		// glDepthRange(0, 1); TODO
		Util::throwIfError();
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}

// -----
	auto stencilTest = rs.stencilTest;
	if (stencilTest.isEnable()) {
		glEnable(GL_STENCIL_TEST);

		auto stencilFace			 = Util::getGlCullMode(stencilTest.cull);
		auto stencilOp				 = Util::getGlDepthTestOp(stencilTest.op);

		auto stencilTestFail		 = Util::getGlStencilTestOp(stencilTest.sfail);
		auto depthTestFail			 = Util::getGlStencilTestOp(stencilTest.dfail);
		auto stencilAndDepthTestFail = Util::getGlStencilTestOp(stencilTest.bfail);

		auto stencilRef				 = GLint(stencilTest.ref);
		auto stencilMask			 = GLuint(stencilTest.mask);

		glStencilFuncSeparate(stencilFace, stencilOp, stencilRef, stencilMask); //glStencilFunc(stencilOp, stencilRef, stencilMask);
		Util::throwIfError();

		glStencilOpSeparate(stencilFace, stencilTestFail, depthTestFail, stencilAndDepthTestFail); //glStencilOp(stencilTestFail, depthTestFail, stencilAndDepthTestFail);
		Util::throwIfError();

		glStencilMaskSeparate(stencilFace, stencilMask); //glStencilMask(stencilMask);
		Util::throwIfError();
	}
	else {
		glDisable(GL_STENCIL_TEST);
	}

// -----
	auto& blend = rs.blend;
	if (blend.isEnable()) {
		glEnable(GL_BLEND);

		auto rgbSrcFactor	= Util::getGlBlendFactor(blend.rgb.srcFactor);
		auto rgbDstFactor	= Util::getGlBlendFactor(blend.rgb.dstFactor);
		auto alphaSrcFactor = Util::getGlBlendFactor(blend.alpha.srcFactor);
		auto alphaDstFactor = Util::getGlBlendFactor(blend.alpha.dstFactor);

		GLenum rgbOp = blend.rgb.op == BlendOp::Disable
						? GL_FUNC_ADD
						: Util::getGlBlendOp(blend.alpha.op);

		GLenum alphaOp = blend.alpha.op == BlendOp::Disable
						? GL_FUNC_ADD
						: Util::getGlBlendOp(blend.rgb.op);

		glBlendEquationSeparate(rgbOp, alphaOp);
		glBlendFuncSeparate(rgbSrcFactor, rgbDstFactor, alphaSrcFactor, alphaDstFactor);
		Util::throwIfError();
	}
	else {
		glDisable(GL_BLEND);
	}

	glBlendColor(blend.constColor.r
				, blend.constColor.g
				, blend.constColor.b
				, blend.constColor.a);

	Util::throwIfError();

// TODO
//	glDisable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(-1.f, 0.001f);
// 
//	glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
//	glDisable(GL_SAMPLE_COVERAGE);
//	glDisable(GL_SCISSOR_TEST); //!!<-- always enable scissor test
//	glDisable(GL_MULTISAMPLE);
//	glDisable(GL_DITHER);
}

#if 0
#pragma mark ========= Material_GL ============
#endif

sgeMaterial_InterfaceFunctions_Impl(GL);

template<class STAGE>
void Material_GL::s_bindStageHelper(RenderContext_GL* ctx, STAGE* stage) {
	auto* shaderStage = stage->shaderStage();
	if (!shaderStage) return;

	const auto& program = stage->shaderPass()->program();

	{ //const buffer
		for (auto& cb : stage->constBuffers()) {
			auto* gpuBuffer = static_cast<RenderGpuBuffer_GL*>(cb.gpuBuffer.ptr());
			if (!gpuBuffer)
				throw SGE_ERROR("cosnt buffer is null");

			auto handler = gpuBuffer->handle();
			if (!handler)
				throw SGE_ERROR("gl buffer is null");

			auto* cbInfo = cb.info();

			GLuint ubIndex = glGetUniformBlockIndex(program, cbInfo->name.c_str());
			if (ubIndex == GL_INVALID_INDEX)
				throw SGE_ERROR("shader program not found uniform block '{}'", cbInfo->name);

			GLuint bindPoint = cbInfo->bindPoint; // glsl uniform loc

			stage->_glSetConstBuffer(ubIndex, bindPoint, handler);

			cb.uploadToGpu();
		}
	}

	{ //bind texture
		GLuint texUnit = 0;
		for (auto& texParam : stage->texParams()) {
			auto* tex = texParam.getUpdatedTexture();
			int bindPoint = texParam.bindPoint(); // glsl uniform loc

			glActiveTexture(GL_TEXTURE0 + texUnit);
			glUniform1i(bindPoint, texUnit);
			Util::throwIfError();

			switch (texParam.dataType()) {
				case RenderDataType::Texture2D: {
					auto* tex2d = static_cast<Texture2D_GL*>(tex);
					if (!tex2d) throw SGE_ERROR("");

					const auto& samplerState = tex2d->samplerState();
					GLenum wrapS		= Util::getGlTextureWrap(samplerState.wrapU);
					GLenum wrapT		= Util::getGlTextureWrap(samplerState.wrapV);
					GLenum minFilter	= Util::getGLTextureMinFilter(samplerState.filter, tex2d->mipmapCount());
					GLenum magFilter	= Util::getGLTextureMagFilter(samplerState.filter);

					tex2d->bind();

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

					#if GL_EXT_texture_filter_anisotropic
						// https://www.khronos.org/opengl/wiki/Sampler_Object#Anisotropic_filtering
						GLfloat mamaximumAnisotropyx;
						glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &mamaximumAnisotropyx);
						GLfloat maxAnisotrory = Math::clamp(samplerState.maxAnisotrory, 1.f, mamaximumAnisotropyx);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotrory);
					#endif
				} break;

				default: throw SGE_ERROR("bind unsupported texture type '{}'", texParam.dataType());
			}

			++texUnit;
		}
		glActiveTexture(GL_TEXTURE0 /* + 0*/);
		Util::throwIfError();
	}
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL