#pragma once

#include "Pose.h"

namespace sge {
/*
	The Skeleton class contains
		- the rest pose
		- and bind pose of a character,
		- the name for every joint of the character,
		- and—most importantly, the inverse bind pose.

	The skeleton is shared;
	you can have many characters, each with a unique animated pose, but they can all share the same skeleton.

	Some engines call the skeleton as "armature" or "rig."
*/

struct GLTFInfo;

class Skeleton : public RefCountBase {
public:
	inline const Pose& restPose() const { return _restPose; }
	inline const Pose& bindPose() const { return _bindPose; }

	inline       Span<      mat4> invBindPose()			{ return _invBindPose; }
	inline const Span<const mat4> invBindPose() const	{ return _invBindPose; }

	inline       Span<      String> jointNames()		{ return _jointNames.span(); }
	inline const Span<const String> jointNames() const	{ return _jointNames.span(); }

	inline StrView getJointName(int i) const { return _jointNames[i]; }

	inline size_t getJointCount() const { return _restPose.getJointCount(); }

	void setRestPose(const Pose& p);
	void setBindPose(const Pose& p);
	void setJointNames(const Span<const String>& names);

	void create(const GLTFInfo& src);
	void clear();

private:

	// This function updates the inverse bind pose matrix any time that the bind pose is set
	// Any time the bind pose of the skeleton is updated, the inverse bind pose should be re-calculated as well.
	void _updateInverseBindPose();

	Pose			_restPose;
	Pose			_bindPose;
	Vector<mat4>	_invBindPose;
	Vector<String>	_jointNames;
};

}