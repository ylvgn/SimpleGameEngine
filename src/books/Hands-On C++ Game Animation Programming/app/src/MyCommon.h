#pragma once

namespace sge {

struct AnimationAttribLocation {
	int pos;
	int normal;
	int uv;
	int weights;
	int joints;

	void setBySkinnedShader(const Shader* const shader);
	void setByStaticShader(const Shader* const shader);
};

struct AnimationInstance {
	Transform model; // model matrix

	// used to clip sampling
	Pose  animatedPose;
	int   clip = 0;
	float playback = 0.f;

	// gpu skinning
	Vector<mat4f> posePalette;

	// blending
	Pose additivePose;
	int  additiveClip = 0;
	Pose additiveBasePose;

	void animatedSample(const Span<const Clip> clips, float dt) {
		if (clips.size() <= clip) return;
		animatedSample(clips[clip], dt);
	}

	template<class T>
	void animatedSample(const ClipT<T>& clip, float dt);

	float getNormalizedTime(const Span<const Clip> clips) {
		if (clips.size() <= clip) return 0;
		return getNormalizedTime(clips[clip]);
	}

	template<class T>
	float getNormalizedTime(const ClipT<T>& clip);

	inline Transform getAnimatedPoseGlobalTransform(int i) const { return animatedPose.getGlobalTransform(i); }
	inline Transform getAnimatedPoseLocalTransform(int i)  const { return animatedPose.getLocalTransform(i);  }
};

template<class T> inline
void AnimationInstance::animatedSample(const ClipT<T>& clip, float dt) {
	playback = clip.sample(animatedPose, playback + dt);
}

template<class T> inline
float AnimationInstance::getNormalizedTime(const ClipT<T>& clip) {
	return (playback - clip.getStartTime()) / clip.getDuration();
}

}