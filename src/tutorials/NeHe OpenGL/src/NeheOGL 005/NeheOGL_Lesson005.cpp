#include "NeheOGL_Lesson005.h"

namespace sge {

void NeheOGL_Lesson005::onDraw() {
	//_example1();
	_example2();
}

void NeheOGL_Lesson005::_example1() {
	float width = _clientRect.w;
	float height = _clientRect.h;

	if (height == 0) {
		height = 1;
	}

	float angleInDegreeFov = 45.f;
	float aspect = width / height;

	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	glClearDepth(1.0f);			// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);	// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);		// The Type Of Depth Testing To Do

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angleInDegreeFov, aspect, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float d = 1.f;
	{
//------------------------------------------
//		   1-----2
//		  /  0  /
//		 3-----4
		float v[5][3] = {
			{ 0,  d,  0}, // 0 Top
			{-d, -d, -d}, // 1
			{ d, -d, -d}, // 2
			{-d, -d,  d}, // 3
			{ d, -d,  d}, // 4
		};
		glLoadIdentity();
		glTranslatef(-1.5f, 0.f, -6.f);
		glRotatef(_rtri, 0.0f, 1.0f, 0.0f);
		OGL::glColor(OGL::kWhite);
		glBegin(GL_LINES);
			// bottom
			glVertex3fv(v[1]); glVertex3fv(v[2]);
			glVertex3fv(v[2]); glVertex3fv(v[4]);
			glVertex3fv(v[4]); glVertex3fv(v[3]);
			glVertex3fv(v[3]); glVertex3fv(v[1]);

			// top
			glVertex3fv(v[1]); glVertex3fv(v[0]);
			glVertex3fv(v[2]); glVertex3fv(v[0]);
			glVertex3fv(v[3]); glVertex3fv(v[0]);
			glVertex3fv(v[4]); glVertex3fv(v[0]);
		glEnd();
	}

	{
//------------------------------------------
//		  0--------1
//		 /|       /|
//		3--------2 |
//      | |      | |
//		| 4------|-5
//      |/       |/
//      7--------6
		float v[8][3] = {
			{-d,  d, -d}, // 0
			{ d,  d, -d}, // 1
			{ d,  d,  d}, // 2
			{-d,  d,  d}, // 3
			{-d, -d, -d}, // 4
			{ d, -d, -d}, // 5
			{ d, -d,  d}, // 6
			{-d, -d,  d}, // 7
		};
		glLoadIdentity();
		glTranslatef(1.f, 0.f, -7.0f);
		glRotatef(_rquad, 1.0f, 1.0f, 1.0f);
		OGL::glColor(OGL::kWhite);
		glBegin(GL_LINES);
			// top
			glVertex3fv(v[0]); glVertex3fv(v[1]);
			glVertex3fv(v[1]); glVertex3fv(v[2]);
			glVertex3fv(v[2]); glVertex3fv(v[3]);
			glVertex3fv(v[3]); glVertex3fv(v[0]);

			// bottom
			glVertex3fv(v[4]); glVertex3fv(v[5]);
			glVertex3fv(v[5]); glVertex3fv(v[6]);
			glVertex3fv(v[6]); glVertex3fv(v[7]);
			glVertex3fv(v[7]); glVertex3fv(v[4]);

			// vertical
			glVertex3fv(v[0]); glVertex3fv(v[4]);
			glVertex3fv(v[1]); glVertex3fv(v[5]);
			glVertex3fv(v[2]); glVertex3fv(v[6]);
			glVertex3fv(v[3]); glVertex3fv(v[7]);
		glEnd();

		_rtri  +=  1.f;
		_rquad -= -1.f;
	}

	swapBuffers();
	drawNeeded();
}

void NeheOGL_Lesson005::_example2() {
	float width = _clientRect.w;
	float height = _clientRect.h;

	if (height == 0) {
		height = 1;
	}

	float angleInDegreeFov = 45.f;
	float aspect = width / height;

	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	glClearDepth(1.0f);			// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);	// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);		// The Type Of Depth Testing To Do

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angleInDegreeFov, aspect, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float d = 1.f;
	{
//------------------------------------------
//		   1-----2
//		  /  0  /
//		 3-----4
		float v[5][3] = {
			{ 0,  d,  0}, // 0 Top
			{-d, -d, -d}, // 1
			{ d, -d, -d}, // 2
			{-d, -d,  d}, // 3
			{ d, -d,  d}, // 4
		};
		glLoadIdentity();
		glTranslatef(-1.5f, 0.f, -6.f);
		glRotatef(_rtri, 0.0f, 1.0f, 0.0f);
		glBegin(GL_TRIANGLES); // Note that all triangles are drawn in a counter clockwise order rotation
			// front
			OGL::glColor(OGL::kRed);	glVertex3fv(v[0]);
			OGL::glColor(OGL::kGreen);	glVertex3fv(v[3]);
			OGL::glColor(OGL::kBlue);	glVertex3fv(v[4]);

			// right
			OGL::glColor(OGL::kRed);	glVertex3fv(v[0]);
			OGL::glColor(OGL::kGreen);	glVertex3fv(v[4]);
			OGL::glColor(OGL::kBlue);	glVertex3fv(v[2]);

			// back
			OGL::glColor(OGL::kRed);	glVertex3fv(v[0]);
			OGL::glColor(OGL::kGreen);	glVertex3fv(v[2]);
			OGL::glColor(OGL::kBlue);	glVertex3fv(v[1]);

			// left
			OGL::glColor(OGL::kRed);	glVertex3fv(v[0]);
			OGL::glColor(OGL::kGreen);	glVertex3fv(v[1]);
			OGL::glColor(OGL::kBlue);	glVertex3fv(v[3]);
		glEnd();
	}

	{
//------------------------------------------
//		  0--------1
//		 /|       /|
//		3--------2 |
//      | |      | |
//		| 4------|-5
//      |/       |/
//      7--------6
		float v[8][3] = {
			{-d,  d, -d}, // 0
			{ d,  d, -d}, // 1
			{ d,  d,  d}, // 2
			{-d,  d,  d}, // 3
			{-d, -d, -d}, // 4
			{ d, -d, -d}, // 5
			{ d, -d,  d}, // 6
			{-d, -d,  d}, // 7
		};
		glLoadIdentity();
		glTranslatef(1.f, 0.f, -7.0f);
		glRotatef(_rquad, 1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
			// All of the quads are drawn in a counter clockwise order
				// Meaning the first point is the top right,
				// the second point is the top left,
				// third point is bottom left,
				// and finally bottom right
			// eyes(viewers) look at each face from cube's outside in front of your screen, TR->TL->BL->BR (counter clockwise order)
			// ignoring the order the quad is drawn in can cause weird results once you get into fancy things such as texture mapping
			OGL::glColor(OGL::kGreen);	glVertex3fv(v[1]); glVertex3fv(v[0]); glVertex3fv(v[3]); glVertex3fv(v[2]); // top
			OGL::glColor(OGL::kOrange);	glVertex3fv(v[6]); glVertex3fv(v[7]); glVertex3fv(v[4]); glVertex3fv(v[5]); // down, rotate 6 to 1 then you see this order

			OGL::glColor(OGL::kRed);	glVertex3fv(v[2]); glVertex3fv(v[3]); glVertex3fv(v[7]); glVertex3fv(v[6]); // front, 'rotate 6 to 1', so 2376 fits to 1032
			OGL::glColor(OGL::kYellow); glVertex3fv(v[5]); glVertex3fv(v[4]); glVertex3fv(v[0]); glVertex3fv(v[1]); // back

			OGL::glColor(OGL::kBlue);	glVertex3fv(v[0]); glVertex3fv(v[4]); glVertex3fv(v[7]); glVertex3fv(v[3]); // left
			OGL::glColor(OGL::kViolet); glVertex3fv(v[5]); glVertex3fv(v[1]); glVertex3fv(v[2]); glVertex3fv(v[6]); // right
		glEnd();

		_rtri  +=  1.f;
		_rquad -= -1.f;
	}

	swapBuffers();
	drawNeeded();
}

}