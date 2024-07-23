#pragma once

#include "MyRenderMesh.h"

namespace sge {

class MyVertexArray : public NonCopyable {
public:
	~MyVertexArray() { destroy(); }

	void create();
	void destroy();

	void bind	() const { glBindVertexArray(_p); }
	void unbind	() const { glBindVertexArray(0); }

private:
	GLuint _p = 0;
};

class MyShader : public RefCountBase {
public:
	~MyShader() { destroy(); }

	void loadFile(StrView filename);
	void destroy();

	void reload();

	void bind();
	void unbind();

	GLint getUniformLoc(StrView name);
	void setUniform(StrView name, const i32& value);
	void setUniform(StrView name, const u32& value);
	void setUniform(StrView name, const float& value);
	void setUniform(StrView name, const Tuple2i& value);
	void setUniform(StrView name, const Tuple3i& value);
	void setUniform(StrView name, const Tuple4i& value);
	void setUniform(StrView name, const Tuple2f& value);
	void setUniform(StrView name, const Tuple3f& value);
	void setUniform(StrView name, const Tuple4f& value);
	void setUniform(StrView name, const Color4f& value);
	void setUniform(StrView name, const Quat4f& value);
	void setUniform(StrView name, const Mat4f& value);

	GLint getAttribLoc(StrView name);

	void draw(const MyRenderMesh& mesh);

private:

	void _compileShader(GLuint& shader, GLenum type, StrView filename);
	void _getShaderInfoLog(GLuint shader, String& outMsg);

	void _linkProgram();
	void _getProgramInfoLog(GLuint program, String& outMsg);

	String _filename;

	GLuint _program = 0;
	GLuint _vertexShader = 0;
	GLuint _pixelShader = 0;

	MyVertexArray _vertexArray;
};

}