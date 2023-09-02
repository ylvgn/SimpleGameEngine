#pragma once

#include <sge_game_anime_prog/math/vec3.h>
#include <sge_game_anime_prog/math/mat4.h>

namespace sge {

class ExampleTestBase : public RefCountBase {
public:
	const float kCamPitch	= 45.0f;
	const float kCamYaw		= 60.0f;
	const float kCamDist	= 7.0f;

	const float kGizmoSize	= 0.25f;

public:
	virtual ~ExampleTestBase() = default;

	virtual void create()				{ onCreate(); }
	virtual void update(float dt)		{ onUpdate(dt); }
	virtual void render(float aspect)	{ onRender(aspect); }

protected:
	virtual void onCreate() { };
	virtual void onUpdate(float dt) { };
	virtual void onRender(float aspect) { };

	inline vec3f getEyesPos() const
	{
		return vec3f(	kCamDist * cosf(Math::radians(kCamYaw))* sinf(Math::radians(kCamPitch)),
						kCamDist * cosf(Math::radians(kCamPitch)),
						kCamDist * sinf(Math::radians(kCamYaw))* sinf(Math::radians(kCamPitch))
		);
	}

	mat4f	_matrixView;
	mat4f	_matrixProj;
};

}
	