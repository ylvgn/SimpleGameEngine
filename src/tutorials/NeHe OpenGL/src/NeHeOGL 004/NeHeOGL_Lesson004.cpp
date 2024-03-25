#include "NeHeOGL_Lesson004.h"

namespace sge {

void NeHeOGL_Lesson004::onDraw() {
	auto uptime = static_cast<float>(_uptime.get());
	_example1(uptime);
	//_example2(uptime);
}

void NeHeOGL_Lesson004::_example1(float upTime) {
	glViewport(0, 0, static_cast<int>(_clientRect.w), static_cast<int>(_clientRect.h));
	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	OGL::color4f(OGL::kWhite);
	glLineWidth(2);
	glBegin(GL_LINES);
		glVertex3f(-1, 0, 0); glVertex3f(1,0,0); // x
		glVertex3f(0, -1, 0); glVertex3f(0,1,0); // y
	glEnd();

	//_example1_rotateTheory(upTime);
	_example1_rotateInOpenGL(upTime);

	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson004::_example1_rotateTheory(float upTime) {
	float angle = upTime * 90.0f;

	float x = 0.3f;
	float y = 0.0f;

	float s, c;
	Math::sincos(g_rad(angle), s, c);

	// 2d rotate
	float nx = x * c + y * -s;
	float ny = x * s + y *  c;

	OGL::color4f(OGL::kRed);
	glPointSize(12);
	glBegin(GL_POINTS);
		glVertex3f(x, y, 0);
		glVertex3f(nx, ny, 0);
	glEnd();
}

void NeHeOGL_Lesson004::_example1_rotateInOpenGL(float upTime) {
	float angleInDegrees = upTime * 90.0f;

	float d = 0.1f;
	{
		static float v[4][2] = {
			{ d,  d }, // 0
			{-d,  d }, // 1
			{-d, -d }, // 2
			{ d, -d }, // 3
		};
	
		glRotatef(angleInDegrees, 0, 0, 1);
		glTranslatef(0.5f, 0, 0);
		glBegin(GL_QUADS);
			OGL::color4f(OGL::kRed); glVertex2fv(v[0]); glVertex2fv(v[1]); glVertex2fv(v[2]); glVertex2fv(v[3]);
		glEnd();
	}

	{
		d *= 0.25f;
		static float v[4][2] = {
			{ d,  d }, // 0
			{-d,  d }, // 1
			{-d, -d }, // 2
			{ d, -d }, // 3
		};
		glRotatef(angleInDegrees, 0, 0, 1);
		glTranslatef(-0.2f, 0, 0);
		glBegin(GL_QUADS);
			OGL::color4f(OGL::kBlue); glVertex2fv(v[0]); glVertex2fv(v[1]); glVertex2fv(v[2]); glVertex2fv(v[3]);
		glEnd();
	}
}

void NeHeOGL_Lesson004::_example2(float upTime) {
	float angleInDegrees = upTime * 90.0f;

	float width  = _clientRect.w;
	float height = _clientRect.h;

	if (height == 0) height = 1;

	float angleInDegreeFov = 45.f;
	float aspect = width / height;

	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(angleInDegreeFov, aspect, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	glLoadIdentity();
	glTranslatef(-1.5f, 0.f, -6.f);
	glRotatef(angleInDegrees, 0.0f, 1.0f, 0.0f);	// Rotate The Triangle On The Y axis
	float d = 0.5f;
	glBegin(GL_TRIANGLES);
		OGL::color4f(OGL::kRed);	glVertex3f( 0,  d, 0);
		OGL::color4f(OGL::kGreen);	glVertex3f(-d, -d, 0);
		OGL::color4f(OGL::kBlue);	glVertex3f( d, -d, 0);
	glEnd();

	glLoadIdentity();
	glTranslatef(1.f, 0.f, -6.0f);
	glRotatef(angleInDegrees, 1.0f, 0.0f, 0.0f); // Rotate The Quad On The X axis
	glColor3f(0.5f, 0.5f, 1.0f);
	glBegin(GL_QUADS);
		glVertex3f(-d,  d, 0);
		glVertex3f( d,  d, 0);
		glVertex3f( d, -d, 0);
		glVertex3f(-d, -d, 0);
	glEnd();

	swapBuffers();
	drawNeeded();
}

}