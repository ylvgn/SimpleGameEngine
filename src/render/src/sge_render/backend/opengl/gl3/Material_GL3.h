#pragma once

#include <sge_render/shader/Material.h>
#include "Shader_GL3.h"
#include "RenderGpuBuffer_GL3.h"

namespace sge {

class Material_GL3 : public Material {
	using Util = GL3Util;

	struct MyPass;

	Shader_GL3* shader() { return static_cast<Shader_GL3*>(_shader.ptr()); }

	template<class STAGE>
	static void _bindStageHelper(RenderContext_GL3* ctx, STAGE* stage);

	struct MyVertexStage : public VertexStage {
		using Base			= VertexStage;
		using Pass			= Material_GL3::MyPass;
		using MyShaderStage = Shader_GL3::MyVertexStage;
		using MyShaderPass  = Shader_GL3::MyPass;

		MyVertexStage(MaterialPass* pass, ShaderVertexStage* shaderStage)
			: Base(pass, shaderStage)
		{}

		void bind(RenderContext_GL3* ctx, const VertexLayout* vertexLayout);
		void bindInputLayout(RenderContext_GL3* ctx, const VertexLayout* vertexLayout);

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
		using Pass			= Material_GL3::MyPass;
		using MyShaderStage = Shader_GL3::MyPixelStage;
		using MyShaderPass  = Shader_GL3::MyPass;

		MyPixelStage(MaterialPass* pass, ShaderPixelStage* shaderStage)
			: Base(pass, shaderStage)
		{}

		void bind(RenderContext_GL3* ctx, const VertexLayout* vertexLayout);

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
		using MyShaderPass	= Shader_GL3::MyPass;

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

}