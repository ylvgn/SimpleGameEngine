#include "DebugDraw.h"
#include "Uniform.h"
#include "Draw.h"

#include <sge_game_anime_prog/animation/Pose.h>
#include <sge_game_anime_prog/ik/CCDSolver.h>

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

void DebugDraw::lineFromPose(const Pose& pose) {
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

void DebugDraw::linesFromIKSolver(const CCDSolver& solver) {
	size_t jointCount =  solver.getJointCount();
	if (jointCount < 2) return;

	_points.resize( (jointCount-1) * 2);
	for (int i = 1, index = 0; i < jointCount; ++i) {
		_points[index++] = solver.getGlobalTransform(i-1).position;
		_points[index++] = solver.getGlobalTransform(i).position;
	}
}

void DebugDraw::pointsFromIKSolver(const CCDSolver& solver) {
	size_t jointCount = solver.getJointCount();
	if (jointCount == 0) return;

	_points.resize(jointCount);
	for (int i = 0; i < jointCount; ++i) {
		_points[i] = solver.getGlobalTransform(i).position;
	}
}

}