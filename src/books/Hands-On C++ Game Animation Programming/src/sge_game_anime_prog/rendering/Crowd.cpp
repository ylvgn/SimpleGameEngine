#include "Crowd.h"

#include <sge_game_anime_prog/opengl/Uniform.h>
#include <sge_game_anime_prog/opengl/Shader.h>

namespace sge {

void Crowd::resize(size_t newCrowdCount) {
	SGE_ASSERT(newCrowdCount <= kCrowdMaxCount);

	if (newCrowdCount > kCrowdMaxCount) {
		newCrowdCount = kCrowdMaxCount;
	}

	_positions.resize(newCrowdCount, vec3f::s_zero());
	_rotations.resize(newCrowdCount, quat4f::s_identity());
	_scales.resize(newCrowdCount, vec3f::s_one());

	_currentAnimPlaybackTimes.resize(newCrowdCount);
	_nextAnimPlaybackTimes.resize(newCrowdCount);

	_frames.resize(newCrowdCount);

	_blendTimes.resize(newCrowdCount);
}

void Crowd::update(float dt, const Clip& clip, size_t texSize) {
	SGE_ASSERT(texSize > 0);

	MyRequest req;
	req.dt			= dt;
	req.start		= clip.getStartTime();
	req.end			= clip.getEndTime();
	req.duration	= clip.getDuration();
	req.texSize		= texSize;

	_sr.isLoop		= clip.isLoop();

	_updatePlaybackTimes(req);
	_updateFrameIndices(req);
	_updateInterplation(req);
}

void Crowd::bindUniforms() {
	Uniform<vec3f>::set(_shader->findUniformByName("model_pos"),  _positions);	// uniform vec3 model_pos[MAX_INSTANCES];
	Uniform<quat4f>::set(_shader->findUniformByName("model_rot"), _rotations);	// uniform vec4 model_rot[MAX_INSTANCES];
	Uniform<vec3f>::set(_shader->findUniformByName("model_scl"),  _scales);		// uniform vec3 model_scl[MAX_INSTANCES];
	Uniform<vec2i>::set(_shader->findUniformByName("frames"), _frames);			// uniform ivec2 frames[MAX_INSTANCES];
	Uniform<float>::set(_shader->findUniformByName("time"), _blendTimes);		// uniform float time[MAX_INSTANCES];
}

float Crowd::_getAdjustClipTime(const MyRequest& req) const {
	const auto& sr			= _sr;
	const auto& start		= req.start;
	const auto& end			= req.end;
	const auto& duration	= req.duration;

	if (!sr.isLoop) {
		return Math::clamp(sr.time, start, end);
	}

	float time = Math::fmod(sr.time - start, duration);
	if (time < 0.f) time += duration;
	return time + start;
}

void Crowd::_updatePlaybackTimes(const MyRequest& req) {
	const auto& dt = req.dt;
	size_t crowdCount = getCrowdCount();

	for (int i = 0; i < crowdCount; ++i) {
		_sr.time = _currentAnimPlaybackTimes[i] + dt;
		_currentAnimPlaybackTimes[i] = _getAdjustClipTime(req);

		_sr.time = _currentAnimPlaybackTimes[i] + dt;
		_nextAnimPlaybackTimes[i] = _getAdjustClipTime(req);
	}
}

void Crowd::_updateFrameIndices(const MyRequest& req) {
	// This function convert the current/next playback time into pixel coordinates
	// along the animation texture's x axis:

	const auto& start		= req.start;
	const auto& duration	= req.duration;
	const auto& texSize		= req.texSize;

	float durationFrame		= static_cast<float>(texSize - 1);

	size_t crowdCount = getCrowdCount();
	for (int i = 0; i < crowdCount; ++i) {

		float t = (_currentAnimPlaybackTimes[i] - start) / duration;
		int currentFrame = static_cast<int>(t * durationFrame);

		t = (_nextAnimPlaybackTimes[i] - start) / duration;
		int nextFrame = static_cast<int>(t * durationFrame);

		_frames[i] = { currentFrame, nextFrame };
	}
}

void Crowd::_updateInterplation(const MyRequest& req) {
/*
	The shader will need to interpolate between the current animated pose
	and the next animated pose.
	To do this, it needs to know the current normalized time between the frames of the two poses.

	This function should find the interpolation time(0~1)
	between the current and next animated frames:
*/

	const auto& texSize		= req.texSize;
	const auto& start		= req.start;
	const auto& duration	= req.duration;

	float durationFrame		= static_cast<float>(texSize - 1);

	size_t crowdCount = getCrowdCount();
	for (int i = 0; i < crowdCount; ++i) {

		const auto& currentFrame	= _frames[i].x;
		const auto& nextFrame		= _frames[i].y;

		if (currentFrame == nextFrame) {
			_blendTimes[i] = 1.0f;
			continue;
		}

		float t = static_cast<float>(currentFrame) / durationFrame;
		float curFrameTime = start + t * duration; // same to _currentAnimPlaybackTimes[i] ???

		t = static_cast<float>(nextFrame) / durationFrame;
		float nextFrameTime = start + t * duration;

		if (nextFrameTime < curFrameTime) {
			nextFrameTime += duration;
		}

		float frameDuration = nextFrameTime - curFrameTime;

		_blendTimes[i] = (_currentAnimPlaybackTimes[i] - curFrameTime) / frameDuration;
	}
}

void Crowd::randomizeTimes(const Clip& clip) {
	float start			= clip.getStartTime();
	float duration		= clip.getDuration();

	size_t crwodCount = getCrowdCount();
	for (int i = 0; i < crwodCount; ++i) {
		float random01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		_currentAnimPlaybackTimes[i] = start + (random01 * duration);
	}
}

void Crowd::randomizePositions(Span<vec3f> existPositions, const vec3f& min, const vec3f& max, float crowdGapRadius) {
	Vector<vec3f> newPositions;
	size_t crwodCount = getCrowdCount();

	float sqRadius	= crowdGapRadius * crowdGapRadius;
	vec3f delta		= max - min;

	newPositions.reserve(crwodCount);

	for (auto& p : existPositions) {
		if (p.x < min.x || p.y < min.y || p.z < min.z) continue;
		if (p.x > max.x || p.y > max.y || p.z > max.z) continue;

		size_t size = newPositions.size();
		for (int i = 0; i < size; ++i) {
			if ((newPositions[i] - p).lenSq() >= sqRadius) {
				newPositions.push_back(p);
			}
		}
	}

	static constexpr const int kMaxLoopTimes = 2000;

	int breakLoop = 0;
	while (newPositions.size() < crwodCount) {
		if (breakLoop >= kMaxLoopTimes) {
			break;
		}

		vec3f random01 (static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
						static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
						static_cast<float>(rand()) / static_cast<float>(RAND_MAX));

		vec3f point = min + delta * random01;

		bool isValid = true;
		size_t size = newPositions.size();
		for (int i = 0; i < size; ++i) {
			if ((newPositions[i] - point).lenSq() < sqRadius) {
				isValid = false;
				breakLoop += 1;
				break;
			}
		}

		if (isValid) {
			breakLoop = 0;
			newPositions.push_back(point);
		}
	}

	if (newPositions.size() != _positions.size()) {
		resize(newPositions.size());
	}

	std::memcpy(_positions.data()->ptr(), newPositions.data()->ptr(), sizeof(float) * 3 * newPositions.size());
}

}