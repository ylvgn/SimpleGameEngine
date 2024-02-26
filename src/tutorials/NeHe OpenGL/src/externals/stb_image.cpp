#if SGE_COMPILER_VC
#pragma warning(push) 
#pragma warning(disable: 4244)
#endif

// https://github.com/nothings/stb/blob/master/stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#if SGE_COMPILER_VC
#pragma warning(pop) 
#endif