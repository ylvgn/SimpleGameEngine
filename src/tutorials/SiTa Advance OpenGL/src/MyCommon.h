#pragma once
#include <sge_core.h>

#include "sge_sita_advance_opengl-config.h"

#define GLEW_STATIC 1
#include <GL/glew.h>
#if SGE_OS_WINDOWS
	#include <GL/wglew.h>
#endif

#include <GL/gl.h>
#pragma comment(lib, "Opengl32.lib")

#include <GL/glu.h>
#pragma comment(lib, "GLu32.lib")
