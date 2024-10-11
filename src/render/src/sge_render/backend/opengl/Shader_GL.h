#pragma once

#include "Render_GL_Common.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {

class RenderContext_GL;

class Shader_GL : public Shader {
	using Base = Shader;
public:
	struct MyPass;

	using Util = GLUtil;

	Shader_GL(StrView filename);

	struct MyVertexStage : public ShaderVertexStage {
		using Pass = MyPass;
		using Util = GLUtil;

		MyVertexStage() = default;
		~MyVertexStage();

		void bind(RenderContext_GL* ctx);

	friend struct MyPass;
	private:
		MyPass*	_pass	= nullptr;
		GLuint	_shader = 0;
	};

	struct MyPixelStage : public ShaderPixelStage {
		using Pass = MyPass;
		using Util = GLUtil;

		MyPixelStage() = default;
		~MyPixelStage();

		void bind(RenderContext_GL* ctx);

	friend struct MyPass;
	private:
		MyPass*	_pass		= nullptr;
		GLuint	_shader		= 0;
	};

	struct MyPass : public ShaderPass {
		using Base = ShaderPass;
		using Util = GLUtil;

		MyPass(Shader* shader, int passIndex) noexcept;
		~MyPass();

		static void s_compileStage		(GLuint& shader, GLenum type, StrView filename);
		static void s_getShaderInfoLog	(GLuint shader, String& outMsg);

		void bind();
		void unbind();

		GLuint program() const { return _program; }

	private:
		virtual void onInit() override;

		MyVertexStage	_vertexStage;
		MyPixelStage	_pixelStage;
		GLuint			_program = 0;

		void _linkProgram();
		void _getProgramInfoLog(GLuint program, String& outMsg);
	};
protected:
	virtual UPtr<ShaderPass> onCreateShaderPass(Shader* shader, int passIndex) override;
}; // Shader_GL

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL