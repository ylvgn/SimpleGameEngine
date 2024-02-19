#include "NeheOGL_Lesson004.h"

namespace sge {

void NeheOGL_Lesson004::onDraw() {
	_example1();
}

void NeheOGL_Lesson004::_example1() {
	float width = _clientRect.w;
	float height = _clientRect.h;

	if (height == 0) {
		height = 1;
	}

	float angleInDegreeFov = 45.f;
	float aspect = width / height;

	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angleInDegreeFov, aspect, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(-1.5f, 0.f, -6.f);

	glRotatef(_rtri, 0.0f, 1.0f, 0.0f);	// Rotate The Triangle On The Y axis
	float d = 0.5f;
	glBegin(GL_TRIANGLES);
		OGL::glColor(OGL::kRed);	glVertex3f( 0,  d, 0);
		OGL::glColor(OGL::kGreen);	glVertex3f(-d, -d, 0);
		OGL::glColor(OGL::kBlue);	glVertex3f( d, -d, 0);
	glEnd();

	glLoadIdentity();
	glTranslatef(1.f, 0.f, -6.0f);
	glRotatef(_rquad, 1.0f, 0.0f, 0.0f); // Rotate The Quad On The X axis
	glColor3f(0.5f, 0.5f, 1.0f);
	glBegin(GL_QUADS);
		glVertex3f(-d,  d, 0);
		glVertex3f( d,  d, 0);
		glVertex3f( d, -d, 0);
		glVertex3f(-d, -d, 0);
	glEnd();

	_rtri  += 1.f;
	_rquad -= -1.f;

	swapBuffers();
	drawNeeded();
}

}