#include "DebugDraw.h"
#include <sge_game_anime_prog/opengl/Uniform.h>
#include <sge_game_anime_prog/opengl/Draw.h>
#include <sge_game_anime_prog/animation/Pose.h>
#include <sge_game_anime_prog/ik/CCDSolver.h>
#include <sge_game_anime_prog/ik/FABRIKSolver.h>

namespace sge {

void DebugDraw::draw(DebugDrawMode mode, const mat4f& mvp, const Color4f& color /*= Color4f(1,0,0,1)*/) {
	size_t vertexCount = _points.size();
	if (vertexCount == 0) return;

	_shader->bind();

	_attribs.bind(_shader->findAttributeByName("position"));
#if 1
	Uniform<mat4f>::set(_shader->findUniformByName("mvp"), mvp);
#else
	Uniform<mat4f>::set(_shader->findUniformByName("model"), model);
	Uniform<mat4f>::set(_shader->findUniformByName("view"), view);
	Uniform<mat4f>::set(_shader->findUniformByName("projection"), projection);
#endif

	Uniform<Color4f>::set(_shader->findUniformByName("color"), color);

	switch (mode)
	{
		case DebugDrawMode::Lines:	DrawUtil::draw(vertexCount, DrawMode::Lines); break;
		case DebugDrawMode::Loop:	DrawUtil::draw(vertexCount, DrawMode::LineLoop); break;
		case DebugDrawMode::Strip:	DrawUtil::draw(vertexCount, DrawMode::LineStrip); break;
		case DebugDrawMode::Points:	DrawUtil::draw(vertexCount, DrawMode::Points); break;
		default:					throw SGE_ERROR("unsupported DebugDrawMode"); break;
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
		_points.push_back(pose.getWorldTransform(i).position);
		_points.push_back(pose.getWorldTransform(p).position);
	}
}

void DebugDraw::linesFromIKSolver(const CCDSolver& solver) {
	size_t jointCount =  solver.getJointCount();
	if (jointCount < 2) return;

	_points.resize( (jointCount-1) * 2);
	for (int i = 1, index = 0; i < jointCount; ++i) {
		_points[index++] = solver.getWorldTransform(i-1).position;
		_points[index++] = solver.getWorldTransform(i).position;
	}
}

void DebugDraw::pointsFromIKSolver(const CCDSolver& solver) {
	size_t jointCount = solver.getJointCount();
	if (jointCount == 0) return;

	_points.resize(jointCount);
	for (int i = 0; i < jointCount; ++i) {
		_points[i] = solver.getWorldTransform(i).position;
	}
}

void DebugDraw::linesFromIKSolver(const FABRIKSolver& solver) {
	size_t jointCount = solver.getJointCount();
	if (jointCount < 2) return;

	_points.resize((jointCount - 1) * 2);
	for (int i = 1, index = 0; i < jointCount; ++i) {
		_points[index++] = solver.getWorldTransform(i-1).position;
		_points[index++] = solver.getWorldTransform(i).position;
	}
}

void DebugDraw::pointsFromIKSolver(const FABRIKSolver& solver) {
	size_t jointCount = solver.getJointCount();
	if (jointCount == 0) return;

	_points.resize(jointCount);
	for (int i = 0; i < jointCount; ++i) {
		_points[i] = solver.getWorldTransform(i).position;
	}
}

template<class IKSolver>
void DebugDraw_PointLines::fromIKSolver(const IKSolver& solver) {
	_lines->linesFromIKSolver(solver);
}

void DebugDraw_PointLines::draw(const mat4f& mvp, Mask mask/*= Mask::Point | Mask::Line*/) {
	_lines->uploadToGpu();

	if (enumHas(mask, Mask::Line)) {
		_lines->draw(DebugDrawMode::Lines, mvp, _lineColor);
	}

	if (enumHas(mask, Mask::Point)) {
		_lines->draw(DebugDrawMode::Points, mvp, _pointColor);
	}
}

// explict template instantiation
template void DebugDraw_PointLines::fromIKSolver(const CCDSolver& solver);
template void DebugDraw_PointLines::fromIKSolver(const FABRIKSolver& solver);

}