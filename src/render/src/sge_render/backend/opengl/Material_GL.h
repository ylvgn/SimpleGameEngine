#pragma once

#include <sge_render/shader/Material.h>
#include "Shader_GL.h"
#include "RenderGpuBuffer_GL.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {

class Material_GL : public Material {
	using Util = GLUtil;

	struct MyPass;

	Shader_GL* shader() { return static_cast<Shader_GL*>(_shader.ptr()); }

	template<class STAGE>
	static void _bindStageHelper(RenderContext_GL* ctx, STAGE* stage);

	struct MyVertexStage : public VertexStage {
		using Base			= VertexStage;
		using Pass			= Material_GL::MyPass;
		using MyShaderStage = Shader_GL::MyVertexStage;
		using MyShaderPass  = Shader_GL::MyPass;

		MyVertexStage(MaterialPass* pass, ShaderVertexStage* shaderStage)
			: Base(pass, shaderStage)
		{}

		void bind(RenderContext_GL* ctx, const VertexLayout* vertexLayout);
		void bindInputLayout(RenderContext_GL* ctx, const VertexLayout* vertexLayout);

		Pass*			pass()			const { return static_cast<Pass*>(_pass); }
		MyShaderStage*	shaderStage()	const { return static_cast<MyShaderStage*>(_shaderStage); }
		MyShaderPass*	shaderPass()	const { return pass()->shaderPass(); }
		GLuint			shaderProgram()	const { return shaderPass()->program(); }

		void _glSetConstBuffer(GLuint ubIndex, GLuint bindPoint, GLuint glBuf) {
			glBindBufferBase(GL_UNIFORM_BUFFER, bindPoint, glBuf);
//			glBindBufferRange(GL_UNIFORM_BUFFER, bindPoint, glBuf, cbInfo->variable[j]->offset, cbInfo->variable[j].datasize); TODO
			glUniformBlockBinding(shaderProgram(), ubIndex, bindPoint);
		}
	};

	struct MyPixelStage : public PixelStage {
		using Base			= PixelStage;
		using Pass			= Material_GL::MyPass;
		using MyShaderStage = Shader_GL::MyPixelStage;
		using MyShaderPass  = Shader_GL::MyPass;

		MyPixelStage(MaterialPass* pass, ShaderPixelStage* shaderStage)
			: Base(pass, shaderStage)
		{}

		void bind(RenderContext_GL* ctx, const VertexLayout* vertexLayout);

		Pass*			pass()			const { return static_cast<Pass*>(_pass); }
		MyShaderStage*	shaderStage()	const { return static_cast<MyShaderStage*>(_shaderStage); }
		MyShaderPass*	shaderPass()	const { return pass()->shaderPass(); }
		GLuint			shaderProgram()	const { return shaderPass()->program(); }

		void _glSetConstBuffer(GLuint ubIndex, GLuint bindPoint, GLuint glBuf) {
			glBindBufferBase(GL_UNIFORM_BUFFER, bindPoint, glBuf);
			glUniformBlockBinding(shaderProgram(), ubIndex, bindPoint);
		}
	};

	struct MyPass : public Pass {
		using Base			= Pass;
		using MyShaderPass	= Shader_GL::MyPass;

		MyPass(Material* material, ShaderPass* shaderPass);

		virtual void onBind(RenderContext* ctx, const VertexLayout* vertexLayout) override;

		MyShaderPass* shaderPass() { return static_cast<MyShaderPass*>(_shaderPass); }

		MyVertexStage _myVertexStage;
		 MyPixelStage  _myPixelStage;
	};

	virtual UPtr<Pass> onCreatePass(ShaderPass* shaderPass) override {
		return UPtr<Pass>(new MyPass(this, shaderPass));
	}
};

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL