#pragma once

#include <sge_game_anime_prog/math/vec3.h>
#include <sge_game_anime_prog/math/mat4.h>

namespace sge {

class SampleRequest;

class ExampleTestBase : public RefCountBase {
public:
	using Request = SampleRequest;

	static constexpr const float kCamPitch	= 45.0f;
	static constexpr const float kCamYaw	= 60.0f;
	static constexpr const float kCamDist	= 7.0f;
	static constexpr const float kGizmoSize	= 0.25f;

public:
	virtual ~ExampleTestBase() = default;

	virtual void create(Request& req)	{ _defaultSetCamera(req); onCreate(req); }
	virtual void update(Request& req)	{ onUpdate(req); }
	virtual void render(Request& req)	{ onRender(req); }

protected:
	virtual void onCreate(Request& req) {};
	virtual void onUpdate(Request& req) {};
	virtual void onRender(Request& req) {};

private:
	void _defaultSetCamera(Request& req);
};

}
	