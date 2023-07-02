#include "DebugDraw.h"
#include "Uniform.h"
#include "Draw.h"

namespace sge {

void DebugDraw::draw(DebugDrawMode mode, const mat4& mvp, const Color4f& color /*= Color4f(1,0,0,1)*/) {
	_shader->bind();

	_attribs.bind(_shader->findAttributeByName("position"));
#if 1
	Uniform<mat4>::set(_shader->findUniformByName("mvp"), mvp);
#else
	Uniform<mat4>::set(_shader->findUniformByName("model"), model);
	Uniform<mat4>::set(_shader->findUniformByName("view"), view);
	Uniform<mat4>::set(_shader->findUniformByName("projection"), projection);
#endif

	Uniform<Color4f>::set(_shader->findUniformByName("color"), color);

	size_t vertexCount = _points.size();

	switch (mode)
	{
		case DebugDrawMode::Lines:	DrawUtil::draw(vertexCount, DrawMode::Lines); break;
		case DebugDrawMode::Loop:	DrawUtil::draw(vertexCount, DrawMode::LineLoop); break;
		case DebugDrawMode::Strip:	DrawUtil::draw(vertexCount, DrawMode::LineStrip); break;
		case DebugDrawMode::Points:	DrawUtil::draw(vertexCount, DrawMode::Points); break;
		default:					throw SGE_ERROR("not supported"); break;
	}

	_attribs.unbind(_shader->findAttributeByName("position"));
	_shader->unbind();
}

void DebugDraw::fromPose(const Pose& pose) {
	size_t jointCount = pose.getJointCount();

	_points.clear();
	_points.reserve(jointCount);

	for (int i = 0; i < jointCount; ++i) {
		int p = pose.getParent(i);
		if (p < 0) {
			// p == -1 means no parent
			continue;
		}
		// To draw a bone using debug lines
		// draw a line from the position of the joint to the position of its parent.
		_points.push_back(pose.getGlobalTransform(i).position);
		_points.push_back(pose.getGlobalTransform(p).position);
	}
}

}