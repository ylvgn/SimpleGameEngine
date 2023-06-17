#pragma once

#include <sge_core/pointer/SPtr.h>
#include <sge_core/graph/Color.h>

#include <sge_game_anime_prog/math/vec3.h>
#include <sge_game_anime_prog/math/mat4.h>

#include <sge_game_anime_prog/opengl/Shader.h>
#include <sge_game_anime_prog/opengl/Attribute.h>

#include <sge_game_anime_prog/animation/Pose.h>

namespace sge {

/*
	The DebugDraw class is not very efficient;
	it's only meant to be used for debugging.
*/

enum class DebugDrawMode { Lines, Loop, Strip, Points };

class DebugDraw : public NonCopyable {

#if 0
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
#endif

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

	void fromPose(const Pose& pose);

private:
	Vector<vec3>	_points;
	Attribute<vec3> _attribs;
	SPtr<Shader>	_shader;
};

}
