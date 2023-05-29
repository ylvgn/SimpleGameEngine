#pragma once

#include "vec3.h"
#include "mat4.h"
#include "Shader.h"
#include "Attribute.h"

// The DebugDraw class is not very efficient;
// it's only meant to be used for debugging.

namespace sge {

enum class DebugDrawMode { Lines, Loop, Strip, Points };

class DebugDraw : public NonCopyable {

	static constexpr const char* kVert = "#version 330 core\n"
		"uniform mat4 mvp;\n"
		"in vec3 position;\n"
		"void main() {\n"
		"	gl_Position = mvp * vec4(position, 1.0);\n"
		"}";

	static constexpr const char* kFrag = "#version 330 core\n"
		"uniform vec4 color;\n"
		"out vec4 FragColor;\n"
		"void main() {\n"
		"	FragColor = color;\n"
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

	inline void uploadToGpu()            { _attribs.uploadToGpu(_points); }

	inline       vec3& operator[] (u32 index)       { return _points[index]; }
	inline const vec3& operator[] (u32 index) const { return _points[index]; }

	void draw(DebugDrawMode mode, const mat4& mvp, const Color4f& color = Color4f(1,1,0,1));

private:
	Vector<vec3>	_points;
	Attribute<vec3> _attribs;
	SPtr<Shader>	_shader;
};

}
