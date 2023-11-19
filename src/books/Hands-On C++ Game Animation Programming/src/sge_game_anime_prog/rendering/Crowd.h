#pragma once

#include <sge_game_anime_prog/animation/Clip.h>
#include <sge_game_anime_prog/opengl/Shader.h>

namespace sge {

class Crowd : public NonCopyable {
	using SampleRequest = Track_SampleRequest;
	static const int kCrowdMaxCount = 80; // same as crowd.vert `MAX_INSTANCES`

	struct MyRequest {
		float  dt;
		float  start;
		float  end;
		float  duration;
		size_t texSize;
	};

public:

	void resize(size_t newCrowdCount);

	void setShader(Shader* shader)	{ _shader = shader; };
	size_t getCrowdCount() const	{ return _currentAnimPlaybackTimes.size(); }

	void update(float dt, const Clip& clip, size_t texSize);
	void bindUniforms();

	void randomizeTimes(const Clip& clip);
	void randomizePositions(Span<vec3f> existPositions, const vec3f& min, const vec3f& max, float crowdGapRadius);

private:
	// similar to `Clip::_adjustTimeToFitRange`
	float _getAdjustClipTime(const MyRequest& req) const;

	void _updatePlaybackTimes(const MyRequest& req);
	void _updateFrameIndices(const MyRequest& req);
	void _updateInterplation(const MyRequest& req);

/*
	Actor-specific data (instance-specific uniforms) includes
		-the actor's world transform,
		-as well as data related to its animation playback (which allows the two sampled frames to interpolate.).
			-> _currentAnimPlaybackTimes/_nextAnimPlaybackTimes
		The animation data is what frames are being interpolated,
			-the interpolation value, --> _blendTimes
			-and the key times for the current and next frames. --> _frames
*/

	SPtr<Shader>	_shader;
	SampleRequest	_sr;

	// model matrix
	Vector<mat4f>	_models;

	// per-instance data
	Vector<float>	_currentAnimPlaybackTimes;
	Vector<float>	_nextAnimPlaybackTimes; // estimated next time in the animation
	Vector<vec2i>	_frames; // uv-coordinate, The v texture coordinate is the joint index.
	Vector<float>	_blendTimes;
};

}