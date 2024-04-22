#pragma once

#include "sge_nehe_ogl-config.h"
#include <sge_core.h>
#include <sge_core/native_ui/NativeUI.h>

#if SGE_OS_WINDOWS
	#include <windowsx.h>

	#undef max
	#undef min
#endif

namespace sge {

enum class RenderPrimitiveType {
	None,
	Points,
	Lines,
	Triangles,
};

template<class T> inline void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

inline float g_rad(float degree) {
	return degree / 360.f * (2.f * Math::PI<float>());
}

inline u64 g_to_uint64(LARGE_INTEGER v) {
	return static_cast<u64>(v.HighPart) << 32 | v.LowPart;
}

class MyTimer : public NonCopyable {
public:
	MyTimer() {
		reset();
	}

	void reset() {
		_start = _getTick();
	}

	double get() {
		auto c = _getTick();
		return (c - _start) / 1000.0f;
	}

private:
	u64 _getTick() {
		return ::GetTickCount64(); // 10~16 ms, resolution of GetTickCount64 is limited by OS
	}

	u64 _start;
};

class MyHiResTimer : public NonCopyable {
public:
	MyHiResTimer() {
		LARGE_INTEGER f;
		bool ok = ::QueryPerformanceFrequency(&f);
		if (!ok) {
			DWORD errCode = GetLastError();
			throw SGE_ERROR("QueryPerformanceFrequency errCode = {}", errCode);
		}
		_freq = g_to_uint64(f);
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
	u64 _getTick() {
		LARGE_INTEGER v;
		bool ok = ::QueryPerformanceCounter(&v); // <1 us
		if (!ok) {
			DWORD errCode = GetLastError();
			throw SGE_ERROR("QueryPerformanceCounter errCode = {}", errCode);
		}
		return g_to_uint64(v);
	}

	u64 _freq;
	u64 _start;
};

}