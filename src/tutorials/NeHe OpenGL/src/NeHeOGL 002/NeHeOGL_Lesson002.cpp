#include "NeHeOGL_Lesson002.h"

namespace sge {

void NeHeOGL_Lesson002::onDraw() {
	//_example1();
	//_example2();
	_example3();
}

void NeHeOGL_Lesson002::_example1() {
	glViewport(0, 0, static_cast<int>(_clientRect.w), static_cast<int>(_clientRect.h));

	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// (0,0,0) is center (the viewport middle-center is origin)
		// x axis:
			// To the left of center would be a negative number;
			// To the right would be a positive number
		// y axis:
			// Moving towards the top of the screen would be a positive number
			// moving to the bottom of the screen would be a negative number
		// z axis: (see _example2)
			// Moving deeper into the screen is a negative number,
			// moving towards the viewer (out of the screen) would be a positive number
	glLoadIdentity();			// Reset The View: moving back to the center of the screen
	glTranslatef(-0.5f, 0, 0);	// Move Left 0.5, and now the center is (-0.5,0)
	float d = 0.5f;
	glBegin(GL_TRIANGLES);		// Drawing Using Triangles
		glVertex3f( 0,  d, 0);	// Top
		glVertex3f(-d, -d, 0);	// Bottom Left
		glVertex3f( d, -d, 0);	// Bottom Right
	glEnd();					// Finished Drawing The Triangle

	glTranslatef(1, 0, 0);		// Move Right 1 Units: last(-0.5,0,0) + offset(1,0,0) = now(0.5,0,0), and now the center is (0.5,0)
	glBegin(GL_QUADS);			// Draw A Quad
		glVertex3f(-d,  d, 0);	// Top Left
		glVertex3f( d,  d, 0);	// Top Right
		glVertex3f( d, -d, 0);	// Bottom Right
		glVertex3f(-d, -d, 0);	// Bottom Left
	glEnd();					// Done Drawing The Quad

	swapBuffers();
}

void NeHeOGL_Lesson002::_example2() {
	float width  = _clientRect.w;
	float height = _clientRect.h;

	if (height == 0) { // Prevent A Divide By Zero
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

	// inches and millimeters - do these really have anything to do with OpenGL? The answer is, in a word, no
	// The projection and other transformations are inheritly unitless
	// The only rule is that you have to use consistent unit of measurement
	glTranslatef(-0.5f, 0.f, -6.f);	// Move Left 0.5 Units And Into The Screen 6.0

	// clockwise to set the vertex
		// By drawing in a clockwise order,
		// the square will be drawn as a back face.
		// Meaning the side of the quad we see is actually the back.
		// Objects drawn in a counter clockwise order will be facing us.
	float d = 0.5f;
	glBegin(GL_TRIANGLES);
		glVertex3f( 0.f, d, 0.f);
		glVertex3f(-d,  -d, 0.f);
		glVertex3f( d,  -d, 0.f);
	glEnd();

	glTranslatef(1.f, 0.f, 0.f);	// Move Right 1 Units: last(-0.5,0,6) + offset(1,0,0) = now(0.5,0,6), and now the center is (0.5,6)
	glBegin(GL_QUADS);
		glVertex3f(-d,  d, 0.f);
		glVertex3f( d,  d, 0.f);
		glVertex3f( d, -d, 0.f);
		glVertex3f(-d, -d, 0.f);
	glEnd();

	swapBuffers();
}

void NeHeOGL_Lesson002::_example3() {
	glViewport(0, 0, static_cast<int>(_clientRect.w), static_cast<int>(_clientRect.h));

	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float d = 0.25f;

	glLoadIdentity();
	glTranslatef(-0.5f, 0, 0);
	{
		OGL::Scoped_glBegin begin(OGL::Scoped_glBegin::Mode::Triangles);
		glVertex3f( 0,  d, 0);
		glVertex3f(-d, -d, 0);
		glVertex3f( d, -d, 0);
	}

	glTranslatef(1, 0, 0);
	{
		OGL::Scoped_glBegin begin(OGL::Scoped_glBegin::Mode::Quads);
		glVertex3f(-d,  d, 0);
		glVertex3f( d,  d, 0);
		glVertex3f( d, -d, 0);
		glVertex3f(-d, -d, 0);
	}

	swapBuffers();
}

}