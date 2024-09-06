#pragma once

#if SGE_OS_WINDOWS
	#define SGE_RENDER_HAS_DX11 1
	#define SGE_RENDER_HAS_GL 1

#if SGE_RENDER_HAS_GL
	#define SGE_RENDER_HAS_GL3 1
#endif

#endif