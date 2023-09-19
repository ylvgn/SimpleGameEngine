#pragma once

#include <sge_game_anime_prog/math/vec2.h>
#include <sge_game_anime_prog/math/vec3.h>
#include <sge_game_anime_prog/math/vec4.h>
#include <sge_game_anime_prog/math/quat.h>
#include <sge_game_anime_prog/math/mat4.h>
#include <sge_game_anime_prog/math/MathGeometry.h>
#include <sge_game_anime_prog/math/Intersections.h>

#include <sge_game_anime_prog/opengl/Shader.h>
#include <sge_game_anime_prog/opengl/Attribute.h>
#include <sge_game_anime_prog/opengl/IndexBuffer.h>
#include <sge_game_anime_prog/opengl/Uniform.h>
#include <sge_game_anime_prog/opengl/Draw.h>
#include <sge_game_anime_prog/opengl/Texture.h>

#include <sge_game_anime_prog/rendering/Mesh.h>

#include <sge_game_anime_prog/animation/Interpolation.h>
#include <sge_game_anime_prog/animation/Frame.h>
#include <sge_game_anime_prog/animation/Track.h>
#include <sge_game_anime_prog/animation/TransformTrack.h>
#include <sge_game_anime_prog/animation/Pose.h>
#include <sge_game_anime_prog/animation/Clip.h>
#include <sge_game_anime_prog/animation/Skeleton.h>
#include <sge_game_anime_prog/animation/RearrangeBones.h>

#include <sge_game_anime_prog/animation/blending/Blending.h>
#include <sge_game_anime_prog/animation/blending/CrossFadeController.h>

#include <sge_game_anime_prog/ik/CCDSolver.h>
#include <sge_game_anime_prog/ik/FABRIKSolver.h>

#include <sge_game_anime_prog/ik/IKLeg.h>

#include <sge_game_anime_prog/gltf/GLTFLoader.h>

#include <sge_game_anime_prog/opengl/DebugDraw.h>
