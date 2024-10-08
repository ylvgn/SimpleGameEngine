#pragma once

#include "Render_GL_Common.h"

namespace sge {

class RenderContext_GL;

class Shader_GL : public Shader {
	using Base = Shader;
	using Util = GLUtil;
public:
	Shader_GL(StrView filename);

	struct MyPass;

	static void _loadStageFile(StrView passPath, ShaderStageMask stageMask, Vector<u8>& outBytecode, ShaderStageInfo& outInfo);

	struct MyVertexStage : public ShaderVertexStage {
		~MyVertexStage() { destroy(); }

		void destroy();

		void load(MyPass* pass, StrView passPath);
		void bind(RenderContext_GL* ctx);
		ByteSpan bytecode() const { return _bytecode; }

	friend struct MyPass;
	private:
		Vector<u8>	_bytecode;
		MyPass*		_pass = nullptr;
		GLuint		_shader = 0;
	};

	struct MyPixelStage : public ShaderPixelStage {
		~MyPixelStage() { destroy(); }

		void destroy();

		void load(MyPass* pass, StrView passPath);
		void bind(RenderContext_GL* ctx);
		ByteSpan bytecode() const { return _bytecode; }

	friend struct MyPass;
	private:
		Vector<u8>	_bytecode;
		MyPass*		_pass = nullptr;
		GLuint		_shader = 0;
	};

	struct MyPass : public ShaderPass {
		using Base = ShaderPass;

		MyPass(Shader_GL* shader, StrView passPath, ShaderInfo::Pass& info);

		~MyPass() { destroy(); }

		void destroy();

		void bind();
		void unbind();

		GLuint program() const { return _myProgram; }

	private:
		MyVertexStage	_myVertexStage;
		MyPixelStage	_myPixelStage;
		GLuint			_myProgram = 0;

		void _linkProgram();
		void _getProgramInfoLog(GLuint program, String& outMsg);
	};
};

}