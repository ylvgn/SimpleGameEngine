#pragma once

#include "Render_GL_Common.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {

class RenderContext_GL;

class Shader_GL : public Shader {
	using Base = Shader;
	sgeShader_InterfaceFunctions(GL);
public:
	using Util = GLUtil;

	struct MyPass;

	Shader_GL(StrView filename);

	#if 0
	#pragma mark ========= Shader_GL::MyVertexStage ============
	#endif
	struct MyVertexStage : public ShaderVertexStage {
		using Pass = MyPass;

		MyVertexStage() noexcept = default;
		~MyVertexStage() noexcept;

	friend struct MyPass;
	private:
		MyPass*	_pass	= nullptr;
		GLuint	_shader = 0;
	}; // MyVertexStage

	#if 0
	#pragma mark ========= Shader_GL::MyPixelStage ============
	#endif
	struct MyPixelStage : public ShaderPixelStage {
		using Pass = MyPass;

		MyPixelStage() noexcept = default;
		~MyPixelStage() noexcept;

	friend struct MyPass;
	private:
		MyPass*	_pass		= nullptr;
		GLuint	_shader		= 0;
	}; // MyPixelStage

	#if 0
	#pragma mark ========= Shader_GL::MyPass ============
	#endif
	struct MyPass : public ShaderPass {
		using Base = ShaderPass;

		MyPass(Shader* shader, int passIndex) noexcept;
		~MyPass() noexcept;

		void bind();
		void unbind();

		GLuint program() const { return _program; }

	private:
		virtual void onInit() override;

		MyVertexStage	_vertexStage;
		MyPixelStage	_pixelStage;
		GLuint			_program = 0;
	}; // MyPass

	using VertexStage	= MyVertexStage;
	using PixelStage	= MyPixelStage;
	using Pass			= MyPass;

}; // Shader_GL

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL