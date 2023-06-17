#pragma once

#include "cgltf.h"
#include <sge_game_anime_prog/animation/Clip.h>

namespace sge {

cgltf_data* g_loadGLTFFile(StrView path);
void g_freeGLTFFile(cgltf_data* data);

// In this book, we will assume that a glTF file only contains one animated character.
// It's safe to assume that the entire hierarchy of the glTF file can be treated as the skeleton of the model.
Pose g_loadRestPose(cgltf_data* data);

Vector<String> g_loadJointNames(cgltf_data* data);
Vector<Clip> g_loadAnimationClips(cgltf_data* data);

}

