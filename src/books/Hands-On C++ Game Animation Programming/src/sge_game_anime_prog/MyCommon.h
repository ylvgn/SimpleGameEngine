#pragma once

#include "sge_game_anime_prog-config.h"

#include <sge_core.h>

// OpenGL: https://glad.dav1d.de/
#include "glad/glad.h"
#pragma comment(lib, "opengl32.lib")

// Nuklear: https://immediate-mode-ui.github.io/Nuklear/doc/index.html
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear/nuklear.h"

namespace sge {

// EASTL
template<class T, class... Args> inline
UPtr<T> make_unique(Args&&... args) { return eastl::make_unique<T>(SGE_FORWARD(args)...); }

}