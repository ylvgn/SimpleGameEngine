#pragma once
#include "cgltf.h"

namespace sge {

cgltf_data* g_LoadGLTFFile(StrView path);
void g_FreeGLTFFile(cgltf_data* data);

}

