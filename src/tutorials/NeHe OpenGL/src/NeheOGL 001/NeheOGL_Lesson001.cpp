#include "NeheOGL_Lesson001.h"

namespace sge {

void NeheOGL_Lesson001::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);
	setWindowTitle("NeheOGL_Lesson001");
}

void NeheOGL_Lesson001::onClientRectChanged(const Rect2f& rc) {
	Base::onClientRectChanged(rc);
	_resizeGLScene();
}

void NeheOGL_Lesson001::onDraw() {
	SGE_LOG("onDraw\n");

	_resizeGLScene();
	_drawGLScene();

	swapBuffers();	// Swap Buffers (Double Buffering)
}

void NeheOGL_Lesson001::_resizeGLScene() {
	int width = static_cast<int>(_clientRect.w);
	int height = static_cast<int>(_clientRect.h);

	if (height == 0) {	// Prevent A Divide By Zero By
		height = 1;		// Making Height Equal One
	}

	// coordinate is same as GDI: origin left-top(0,0), right-bottom(width,height)
	glViewport(0, 0, width, height);

#if 0 // no need in NeheOGL_Lesson001
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(0.0f, static_cast<GLfloat>(width) / static_cast<GLfloat>(height), 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
#endif
}

void NeheOGL_Lesson001::_drawGLScene() {
	glClearColor(0.2f, 0.5f, 0.6f, 0);					// Set Clear Color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen Color And Depth Buffer

	float d = 0.5f;
	glBegin(GL_QUADS);
		glColor4f(1, 0, 0, 1); glVertex3f(-d, -d, 0);
		glColor4f(0, 1, 0, 1); glVertex3f( d, -d, 0);
		glColor4f(0, 0, 1, 1); glVertex3f( d,  d, 0);
		glColor4f(0, 0, 0, 1); glVertex3f(-d,  d, 0);
	glEnd();


	d = 0.25f;
	float v[4][3] = {
		-d, -d,  0, // 0
		 d, -d,  0, // 1
		 d,  d,  0, // 2
		-d,  d,  0  // 3
	};
	glBegin(GL_TRIANGLES);
		glColor4f(1, 0, 0, 1); glVertex3fv(v[0]);
		glColor4f(0, 1, 0, 1); glVertex3fv(v[1]);
		glColor4f(0, 0, 1, 1); glVertex3fv(v[2]);

		glColor4f(1, 0, 0, 1); glVertex3fv(v[0]);
		glColor4f(0, 1, 0, 1); glVertex3fv(v[2]);
		glColor4f(0, 0, 1, 1); glVertex3fv(v[3]);
	glEnd();
}

}