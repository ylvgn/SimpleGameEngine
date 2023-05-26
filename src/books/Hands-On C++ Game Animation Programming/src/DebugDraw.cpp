#include "DebugDraw.h"
#include "Uniform.h"
#include "Draw.h"

namespace sge {

void DebugDraw::draw(DebugDrawMode mode, const mat4& mvp, const vec3& color /*= vec3(1,0,0)*/) {
	_shader->bind();

	_attribs.bind(_shader->findAttributeByName("position"));
#if 1
	Uniform<mat4>::set(_shader->findUniformByName("mvp"), mvp);
#else
	Uniform<mat4>::set(_shader->findUniformByName("model"), model);
	Uniform<mat4>::set(_shader->findUniformByName("view"), view);
	Uniform<mat4>::set(_shader->findUniformByName("projection"), projection);
#endif

	Uniform<vec3>::set(_shader->findUniformByName("color"), color);

	switch (mode)
	{
	case sge::DebugDrawMode::Lines:
		g_Draw(size(), DrawMode::Lines);
		break;
	case sge::DebugDrawMode::Loop:
		g_Draw(size(), DrawMode::LineLoop);
		break;
	case sge::DebugDrawMode::Strip:
		g_Draw(size(), DrawMode::LineStrip);
		break;
	case sge::DebugDrawMode::Points:
		g_Draw(size(), DrawMode::Points);
		break;
	default:
		throw SGE_ERROR("not supported");
		break;
	}

	_attribs.unbind(_shader->findAttributeByName("position"));
	_shader->unbind();
}

void DebugDraw::draw(DebugDrawMode mode, const mat4& model, const mat4& view, const mat4& projection, const vec3& color /*= vec3(1, 0, 0)*/) {
	draw(mode, projection * view * model, color);
}

}