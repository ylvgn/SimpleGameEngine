#pragma once

#include "vec3.h"
#include "mat4.h"
#include "Shader.h"
#include "Attribute.h"

namespace sge {

enum class DebugDrawMode { Lines, Loop, Strip, Points };

// The DebugDraw class is not very efficient; it's only meant to be used for debugging.

class DebugDraw : public NonCopyable {

	static constexpr const char* kVert = "#version 330 core\n"
		"uniform mat4 mvp;\n"
		"in vec3 position;\n"
		"void main() {\n"
		"	gl_Position = mvp * vec4(position, 1.0);\n"
		"}";

	static constexpr const char* kFrag = "#version 330 core\n"
		"uniform vec3 color;\n"
		"out vec4 FragColor;\n"
		"void main() {\n"
		"	FragColor = vec4(color, 1.0);\n"
		"}";

public:
#if 0
	DebugDraw() : _shader(new Shader(kVert, kFrag)) { }
#else
	DebugDraw() : _shader(new Shader(
		"Assets/Shaders/debug.vert",
		"Assets/Shaders/debug.frag"
	)) { }
#endif

	inline size_t size() const			 { return _points.size(); }
	inline void resize(size_t newSize)   { _points.resize(newSize); }
	inline void clear()					 { _points.clear(); }
	inline void push_back(const vec3& v) { _points.push_back(v); }
	inline void push_back(Span<vec3> v)  { for (auto& s : v) _points.push_back(s); }

	inline       vec3& operator[] (u32 index)       { return _points[index]; }
	inline const vec3& operator[] (u32 index) const { return _points[index]; }

	inline void uploadToGpu() { _attribs.set(_points); }

	void draw(DebugDrawMode mode, const mat4& mvp, const vec3& color = vec3(1,0,0));
	void draw(DebugDrawMode mode, const mat4& model, const mat4& view, const mat4& projection, const vec3& color = vec3(1,0,0));

private:
	Vector<vec3>	_points;
	Attribute<vec3> _attribs;
	SPtr<Shader>	_shader;
};

}
