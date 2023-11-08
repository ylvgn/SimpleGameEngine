#pragma once

#include <sge_game_anime_prog/animation/Track.h>
#include <sge_game_anime_prog/animation/Skeleton.h>

#include "CCDSolver.h"
#include "FABRIKSolver.h"

namespace sge {

class IKLeg : public RefCountBase {
	using IKSolver = FABRIKSolver;
public:
	IKLeg();

	inline int hip()	const	{ return _hipIndex;		}
	inline int knee()	const	{ return _kneeIndex;	}
	inline int ankle()	const	{ return _ankleIndex;	}
	inline int toe()	const	{ return _toeIndex;		}

	inline int setHip(int index)	{ _hipIndex		= index; }
	inline int setKnee(int index)	{ _kneeIndex	= index; }
	inline int setAnkle(int index)	{ _ankleIndex	= index; }
	inline int setToe(int index)	{ _toeIndex		= index; }

	inline float ankleToGroundOffset() const { return _ankleToGroundOffset; }
	inline void setAnkleToGroundOffset(float offset) { _ankleToGroundOffset = offset; }

	inline const ScalarTrack& pinTrack() const		{ return _pinTrack; }
	inline void setPinTrack(const ScalarTrack& t)	{ _pinTrack = t; }

	inline const Pose& outPose()	const	{ return _outPose; }
	inline const IKSolver& solver()	const	{ return _solver; }

	void setByJointNames(const Skeleton* skeleton,
						 StrView hipJointName,
						 StrView kneeJointName,
						 StrView ankleJointName,
						 StrView toeJointName);

	void solveForLeg(const Transform& model, const Pose& animatedPose, const vec3f& ankleTargetPosition);

private:
	int				_hipIndex;
	int				_kneeIndex;
	int				_ankleIndex;
	int				_toeIndex;

	float			_ankleToGroundOffset; // model-specific

	IKSolver		_solver;
	ScalarTrack		_pinTrack;

	Pose			_outPose;
};

}