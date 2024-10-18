#pragma once

#include <sge_render/shader/Material.h>
#include "Shader_GL.h"
#include "RenderGpuBuffer_GL.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {

class Material_GL : public Material {
	using Base = Material;
	using Util = GLUtil;
	sgeMaterial_InterfaceFunctions(GL);

	class MyPass;

	#if 0
	#pragma mark ========= Material_GL::MyVertexStage ============
	#endif
	class MyVertexStage : public Material::VertexStage {
		using Base = typename Material::VertexStage;
	public:
		using ShaderStage	= Shader_GL::MyVertexStage;
		using ShaderPass	= Shader_GL::MyPass;
		using Pass			= Material_GL::MyPass;

		MyVertexStage(MyPass* pass, ShaderVertexStage* shaderStage) noexcept
			: Base(pass, shaderStage)
		{}

		void bind(RenderContext_GL* ctx, const VertexLayout* vertexLayout);
		void bindInputLayout(RenderContext_GL* ctx, const VertexLayout* vertexLayout);

		Pass*			pass()			const { return static_cast<Pass*>(_pass); }
		ShaderStage*	shaderStage()	const { return static_cast<ShaderStage*>(_shaderStage); }
		ShaderPass*		shaderPass()	const { return pass()->shaderPass(); }
		GLuint			shaderProgram()	const { return shaderPass()->program(); }

		void _glSetConstBuffer(GLuint ubIndex, GLuint bindPoint, GLuint glHandle) {
			glBindBufferBase(GL_UNIFORM_BUFFER, bindPoint, glHandle);
//			glBindBufferRange(GL_UNIFORM_BUFFER, bindPoint, glHandle, cbInfo->variable[j]->offset, cbInfo->variable[j].datasize); // TODO
			glUniformBlockBinding(shaderProgram(), ubIndex, bindPoint);
			Util::throwIfError();
		}
	};

	#if 0
	#pragma mark ========= Material_GL::MyPixelStage ============
	#endif
	class MyPixelStage : public Material::PixelStage {
		using Base = typename Material::PixelStage;
	public:
		using Pass			= Material_GL::MyPass;
		using ShaderStage	= Shader_GL::MyPixelStage;
		using ShaderPass	= Shader_GL::MyPass;

		MyPixelStage(MyPass* pass, ShaderPixelStage* shaderStage) noexcept
			: Base(pass, shaderStage)
		{}

		void bind(RenderContext_GL* ctx, const VertexLayout* vertexLayout);

		Pass*			pass()			const { return static_cast<Pass*>(_pass); }
		ShaderStage*	shaderStage()	const { return static_cast<ShaderStage*>(_shaderStage); }
		ShaderPass*		shaderPass()	const { return pass()->shaderPass(); }
		GLuint			shaderProgram()	const { return shaderPass()->program(); }

		void _glSetConstBuffer(GLuint ubIndex, GLuint bindPoint, GLuint glBuf) {
			glBindBufferBase(GL_UNIFORM_BUFFER, bindPoint, glBuf);
			glUniformBlockBinding(shaderProgram(), ubIndex, bindPoint);
		}
	};

	#if 0
	#pragma mark ========= Material_GL::MyPass ============
	#endif
	class MyPass : public Material::Pass {
		using Base = typename Material::Pass;
	public:
		using MyShaderPass	= Shader_GL::MyPass;

		MyPass(Material_GL* material, ShaderPass* shaderPass) noexcept;

		virtual void onBind(RenderContext* ctx, const VertexLayout* vertexLayout) final;

		MyShaderPass* shaderPass() { return static_cast<MyShaderPass*>(_shaderPass); }

		MyVertexStage _vertexStage;
		 MyPixelStage  _pixelStage;
	};

	#if 0
	#pragma mark ========= Material_GL ============
	#endif
	using Pass			= MyPass;
	using VertexStage	= MyVertexStage;
	using PixelStage	= MyPixelStage;

	Shader_GL* shader() { return static_cast<Shader_GL*>(_shader.ptr()); }

	template<class STAGE>
	static void s_bindStageHelper(RenderContext_GL* ctx, STAGE* stage);
};

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL