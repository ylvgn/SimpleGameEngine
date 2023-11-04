#pragma once

#include <sge_core/pointer/SPtr.h>
#include <sge_core/graph/Color.h>

#include <sge_game_anime_prog/math/mat4.h>

#include <sge_game_anime_prog/opengl/Shader.h>
#include <sge_game_anime_prog/opengl/Attribute.h>

#include <sge_game_anime_prog/math/MathGeometry.h>

namespace sge {

/*
	The DebugDraw class is not very efficient
	it's only meant to be used for debugging
*/

enum class DebugDrawMode { Lines, Loop, Strip, Points };

class CCDSolver;
class FABRIKSolver;
class Pose;

class DebugDraw : public RefCountBase {

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
	constexpr static const Color4f kWhite	=	{1,1,1,1};
	constexpr static const Color4f kBlack	=	{0,0,0,1};
	constexpr static const Color4f kRed		=	{1,0,0,1};
	constexpr static const Color4f kGreen	=	{0,1,0,1};
	constexpr static const Color4f kBlue	=	{0,0,1,1};
	constexpr static const Color4f kYellow	=	{1,1,0,1};
	constexpr static const Color4f kPurple	=	{1,0,1,1};
	constexpr static const Color4f kCyan	=	{0,1,1,1};
	constexpr static const Color4f kOrange	=	{1,165.f/255.f,0,1};
	constexpr static const Color4f kGray	=	{175.f/255.f,175.f/255.f,175.f/255.f,1};
public:

#if 0
	DebugDraw(size_t newSize = 0) : _shader(new Shader(kVert, kFrag)) { resize(newSize); }
#else
	DebugDraw(size_t newSize = 0)
		: _shader(new Shader("Assets/Shaders/debug.vert", "Assets/Shaders/debug.frag"))
	{
		resize(newSize);
	}
#endif

	inline size_t size() const							{ return _points.size(); }
	inline void resize(size_t newSize)					{ _points.resize(newSize); }
	inline void clear()									{ _points.clear(); }
	inline void push_back(const vec3f& v)				{ _points.push_back(v); }
	inline void appendRange(const Span<const vec3f>& v)	{ _points.appendRange(v); }
	inline void uploadToGpu()							{ _attribs.uploadToGpu(spanCast<const u8>(_points.span())); }

	inline Span<      vec3f> span()						{ return _points.span(); }
	inline Span<const vec3f> span() const				{ return _points.span(); }

	inline       vec3f& operator[] (int index)       { return _points[index]; }
	inline const vec3f& operator[] (int index) const { return _points[index]; }

	void draw(DebugDrawMode mode, const mat4f& mvp, const Color4f& color = kYellow);

	void lineFromPose(const Pose& pose);

	void linesFromIKSolver (const CCDSolver& solver);
	void pointsFromIKSolver(const CCDSolver& solver);

	void linesFromIKSolver (const FABRIKSolver& solver);
	void pointsFromIKSolver(const FABRIKSolver& solver);

private:

	Vector<vec3f>		_points;
	Attribute<vec3f>	_attribs;
	SPtr<Shader>		_shader;
};

#define DebugDraw_PointLines_Mask_ENUM_LIST(E) \
	E(None,  = 0) \
	E(Point, = 1 << 0) \
	E(Line,  = 1 << 1) \
//----
SGE_ENUM_CLASS(DebugDraw_PointLines_Mask, u8)
SGE_ENUM_ALL_OPERATOR(DebugDraw_PointLines_Mask)

class DebugDraw_PointLines : public RefCountBase {
public:
	using Mask = DebugDraw_PointLines_Mask;

	inline DebugDraw_PointLines()
		: _lines(new DebugDraw())
		, _pointColor(DebugDraw::kPurple)
		, _lineColor(DebugDraw::kYellow) {}

	inline void clear() { _lines->clear(); }

	inline void add(const vec3f& from, const vec3f& to) {
		_lines->push_back(from);
		_lines->push_back(to);
	}

	inline void add(const Triangle& triangle) {
		add(triangle.v0, triangle.v1);
		add(triangle.v0, triangle.v2);
		add(triangle.v1, triangle.v2);
	}

	template<class IKSolver>
	void fromIKSolver(const IKSolver& solver);

	inline void lineFromPose(const Pose& pose)	{ _lines->lineFromPose(pose); }

	inline Color4f pointColor() const	{ return _pointColor; }
	inline Color4f lineColor () const	{ return _lineColor; }

	inline void setPointColor(const Color4f& color) { _pointColor = color; }
	inline void setLineColor (const Color4f& color)	{ _lineColor  = color; }
	inline void setColor(const Color4f& color)		{ setPointColor(color); setLineColor(color); }

	void draw(const mat4f& mvp, Mask mask = Mask::Point | Mask::Line);

private:
	UPtr<DebugDraw>	_lines;

	Color4f			_pointColor;
	Color4f			_lineColor;
};
}
