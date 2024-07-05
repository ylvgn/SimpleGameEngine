#pragma once

#include <sge_core.h>

#include <GL/gl.h>
#pragma comment(lib, "Opengl32.lib")

#include <GL/glu.h>
#pragma comment(lib, "GLu32.lib")

namespace sge {

inline
const char* my_gettoken(String& outToken, StrView src, char delimiter = ' ') {
	outToken.clear();
	if (src.empty())
		return nullptr;

	//trim spaces
	auto* p = src.begin();
	auto* ed = src.end();
	while (p < ed) {
		if (*p != ' ') break;
		p++;
	}

	auto* e = strchr(p, delimiter);
	if (!e) {
		outToken.assign(p, ed);
		return nullptr;
	}
	else {
		outToken.assign(p, e);
		return e + 1;
	}
}

inline
const char* my_getline(String& outLine, StrView src) {
	outLine.clear();
	
	if (src.empty())
		return nullptr;

	auto* p = src.begin();
	auto* ed = src.end();

	auto* e = strchr(p, '\n');
	if (!e) {
		outLine.assign(p, ed);
		return nullptr;
	}
	else {
		outLine.assign(p, e);
		return e + 1;
	}
}

}