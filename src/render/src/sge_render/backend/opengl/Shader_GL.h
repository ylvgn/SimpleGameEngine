#pragma once

#include "Render_GL_Common.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {

class RenderContext_GL;

class Shader_GL : public Shader {
	using Base = Shader;
	using Util = GLUtil;
	sgeShader_InterfaceFunctions(GL);
public:

	class MyPass;

	#if 0
	#pragma mark ========= Shader_GL::MyVertexStage ============
	#endif
	class MyVertexStage : public Shader::VertexStage {
		using Base = typename Shader::VertexStage;
	public:
		using Pass = MyPass;

		MyVertexStage(MyPass* pass) noexcept
			: Base(pass)
		{}

		~MyVertexStage() noexcept;

		MyPass* pass() const { return static_cast<MyPass*>(_pass); }

	friend class MyPass;
	private:
		GLuint _handle = 0;
	}; // MyVertexStage

	#if 0
	#pragma mark ========= Shader_GL::MyPixelStage ============
	#endif
	class MyPixelStage : public Shader::PixelStage {
		using Base = typename Shader::PixelStage;
	public:
		using Pass = MyPass;

		MyPixelStage(MyPass* pass) noexcept
			: Base(pass)
		{}

		~MyPixelStage() noexcept;

		MyPass* pass() const { return static_cast<MyPass*>(_pass); }

	friend class MyPass;
	private:
		GLuint _handle = 0;
	}; // MyPixelStage

	#if 0
	#pragma mark ========= Shader_GL::MyPass ============
	#endif
	class MyPass : public Shader::Pass {
		using Base = typename Shader::Pass;
	public:

		MyPass(Shader_GL* shader, int passIndex) noexcept;
		~MyPass() noexcept;

		void bind();
		void unbind();

		Shader_GL* shader() const { return static_cast<Shader_GL*>(_shader); }
		GLuint program() const { return _program; }

	private:
		virtual void onInit() final;

		MyVertexStage	_vertexStage;
		MyPixelStage	_pixelStage;
		GLuint			_program = 0;
	}; // MyPass

	#if 0
	#pragma mark ========= Shader_GL ============
	#endif
	using Pass			= MyPass;
	using VertexStage	= MyVertexStage;
	using PixelStage	= MyPixelStage;

}; // Shader_GL

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL