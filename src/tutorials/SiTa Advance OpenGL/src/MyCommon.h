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

namespace sge {

#if SGE_OS_WINDOWS
inline
uint64_t my_to_uint64(const LARGE_INTEGER& v) {
	return static_cast<uint64_t>(v.HighPart) << 32 | v.LowPart;
}

class MyHiResTimer : public NonCopyable {
public:
	MyHiResTimer() {
		::LARGE_INTEGER f;
		::QueryPerformanceFrequency(&f);
		_freq = my_to_uint64(f);
		reset();
	}

	void reset() {
		_start = _getTick();
	}

	double get() {
		auto c = _getTick();
		return static_cast<double>(c - _start) / _freq;
	}

private:
	uint64_t _getTick() {
		::LARGE_INTEGER v;
		::QueryPerformanceCounter(&v);
		return my_to_uint64(v);
	}

	uint64_t _freq;
	uint64_t _start;
};
#endif
}
