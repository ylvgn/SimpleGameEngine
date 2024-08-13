#pragma once
#include "Render_GL3_Common.h"

namespace sge {

class Shader_GL3 : public Object /*Shader*/ {
public:
	~Shader_GL3() { destroy(); }

	void loadFile(StrView filename);
	void destroy();

	void reload();

	void bind();
	void unbind();

	GLint glGetAttribLoc(StrView name);
	GLint glGetUniformLoc(StrView name);

private:
	void _compileShader(GLuint& shader, GLenum type, StrView filename);
	void _getShaderInfoLog(GLuint shader, String& outMsg);

	void _linkProgram();
	void _getProgramInfoLog(GLuint program, String& outMsg);

public:
	String _filename;
	GLuint _program = 0;
	GLuint _vertexShader = 0;
	GLuint _pixelShader = 0;
};

}