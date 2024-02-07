#include "NeheOGL_Lesson002.h"

namespace sge {

void NeheOGL_Lesson002::onDraw() {
	glViewport(0, 0, static_cast<int>(_clientRect.w), static_cast<int>(_clientRect.h));

	glClearColor(0.0f, 0.2f, 0.2f, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// (0,0,0) is center (the viewport middle-center is origin)
		// x axis:
			// To the left of center would be a negative number;
			// To the right would be a positive number
		// y axis:
			// Moving towards the top of the screen would be a positive number
			// moving to the bottom of the screen would be a negative number
		// z axis: (no need in NeheOGL_Lesson002)
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

}