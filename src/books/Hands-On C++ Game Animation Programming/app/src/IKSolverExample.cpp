#include "IKSolverExample.h"

#include <sge_game_anime_prog/ik/CCDSolver.h>
#include <sge_game_anime_prog/ik/FABRIKSolver.h>

namespace sge {

template IKSolverExample<CCDSolver>;
template IKSolverExample<FABRIKSolver>;

}