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

	int hip()	const		{ return _hipIndex;		}
	int knee()	const		{ return _kneeIndex;	}
	int ankle()	const		{ return _ankleIndex;	}
	int toe()	const		{ return _toeIndex;		}

	int setHip(int index)	{ _hipIndex		= index; }
	int setKnee(int index)	{ _kneeIndex	= index; }
	int setAnkle(int index)	{ _ankleIndex	= index; }
	int setToe(int index)	{ _toeIndex		= index; }

	float ankleToGroundOffset() const { return _ankleToGroundOffset; }
	void setAnkleToGroundOffset(float offset) { _ankleToGroundOffset = offset; }

	const ScalarTrack& pinTrack() const		{ return _pinTrack; }
	void setPinTrack(const ScalarTrack& t)	{ _pinTrack = t; }

	const Pose& outPose()		const	{ return _outPose; }
	const IKSolver& solver()	const	{ return _solver; }

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