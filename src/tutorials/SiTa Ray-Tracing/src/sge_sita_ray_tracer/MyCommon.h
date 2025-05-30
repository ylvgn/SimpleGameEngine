#pragma once

#include <sge_core.h>

#include <GL/gl.h>
#pragma comment(lib, "Opengl32.lib")

#include <GL/glu.h>
#pragma comment(lib, "GLu32.lib")

namespace sge {

inline
Color4b my_toColor4b(Color4f c) {
	using T = Color4b::ElementType;
	return Color4b(
		static_cast<T>(Math::clamp<float>(c.r * 255, 0, 255)),
		static_cast<T>(Math::clamp<float>(c.g * 255, 0, 255)),
		static_cast<T>(Math::clamp<float>(c.b * 255, 0, 255)),
		static_cast<T>(Math::clamp<float>(c.a * 255, 0, 255))
	);
}

#if SGE_OS_WINDOWS
class MyTimer {
public:
	MyTimer() { 
		reset(); 
	}

	void reset() {
		m_start = getTick();
	}

	double get() {
		auto c = getTick();
		return (c - m_start) / 1000.0f;
	}

private:
	uint64_t getTick() { 
		return GetTickCount64();
	}

	uint64_t m_start;
};
#endif // SGE_OS_WINDOWS

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

class Scoped_glPushMatrix : public NonCopyable {
public:
	Scoped_glPushMatrix() noexcept { glPushMatrix(); }
	~Scoped_glPushMatrix() noexcept { glPopMatrix(); }
};

class Scoped_glBegin : public NonCopyable {
public:
	Scoped_glBegin(GLenum mode) noexcept { glBegin(mode); }
	~Scoped_glBegin() noexcept { glEnd(); }
};

class Scoped_glEnable : public NonCopyable {
public:
	Scoped_glEnable(GLenum v) noexcept : _v(v) { glEnable(_v); }
	~Scoped_glEnable() noexcept { glDisable(_v); }
private:
	GLenum _v;
};

class Scoped_glDisable : public NonCopyable {
public:
	Scoped_glDisable(GLenum v) noexcept : _v(v) { glDisable(_v); }
	~Scoped_glDisable() noexcept { glEnable(_v); }
private:
	GLenum _v;
};

inline
void my_drawGrid() {
	glLineWidth(1);
	glColor4f(0.5f, 0.5f, 0.5f, 1);
	{
		Scoped_glBegin begin(GL_LINES);
		for (float x = -10; x <= 10; x++) {
			glVertex3f(x, 0, -10);
			glVertex3f(x, 0,  10);
		}
		for (float z = -10; z <= 10; z++) {
			glVertex3f(-10, 0, z);
			glVertex3f( 10, 0, z);
		}
	}
	glColor4f(1,1,1,1);
}

inline
void my_drawOriginAxis() {
	glLineWidth(2);
	{
		Scoped_glBegin begin(GL_LINES);
		glColor4f(1, 0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(1, 0, 0);
		glColor4f(0, 1, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 1, 0);
		glColor4f(0, 0, 1, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1);
	}

	glColor4f(1,1,1,1);
	glLineWidth(1);
}

inline void my_glVertex3(GLfloat x, GLfloat y, GLfloat z) { glVertex3f(x, y, z); }
inline void my_glVertex3(GLdouble x, GLdouble y, GLdouble z) { glVertex3d(x, y, z); }

inline void my_glVertex3v(GLfloat* v) { glVertex3fv(v); }
inline void my_glVertex3v(GLdouble* v) { glVertex3dv(v); }

inline void my_glColor4(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { glColor4f(r,g,b,a); }
inline void my_glColor4(GLdouble r, GLdouble g, GLdouble b, GLdouble a) { glColor4d(r,g,b,a); }

inline void my_glTranslate(GLfloat x, GLfloat y, GLfloat z) { glTranslatef(x, y, z); }
inline void my_glTranslate(GLdouble x, GLdouble y, GLdouble z) { glTranslated(x, y, z); }

}