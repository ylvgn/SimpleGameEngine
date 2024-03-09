#include "NeheOGL_Lesson005.h"

namespace sge {

void NeheOGL_Lesson005::onDraw() {
	auto uptime = static_cast<float>(_uptime.get());

	float width  = _clientRect.w;
	float height = _clientRect.h;

	if (height == 0) {
		height = 1;
	}

	float aspect = width / height;
	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearDepth(1.0f);			// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);	// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);		// The Type Of Depth Testing To Do

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.f, aspect, 0.01f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	_example(uptime, false);
	_example(uptime, true);

	swapBuffers();
	drawNeeded();
}

void NeheOGL_Lesson005::_example(float uptime, bool bWireFrame) {
	static constexpr float kRotateSpeed = 90.f;
	float angle = uptime * kRotateSpeed;

	if (bWireFrame) _wireFrame(angle);
	else _shaded(angle);
}

void NeheOGL_Lesson005::_wireFrame(float angle) {
	glLineWidth(2.f);

	{
		auto& v = kCone;
		glLoadIdentity();
		glTranslatef(-1.5f, 0.f, -6.f);
		glRotatef(angle, 0.0f, 1.0f, 0.0f);
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
		auto& v = kCube;
		glLoadIdentity();
		glTranslatef(1.f, 0.f, -7.0f);
		glRotatef(angle, 1.0f, 1.0f, 1.0f);
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
	}
}

void NeheOGL_Lesson005::_shaded(float angle) {
	{
		auto& v = kCone;
		glLoadIdentity();
		glTranslatef(-1.5f, 0.f, -6.f);
		glRotatef(angle, 0.0f, 1.0f, 0.0f);
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
		auto& v = kCube;
		glLoadIdentity();
		glTranslatef(1.f, 0.f, -7.0f);
		glRotatef(angle, 1.0f, 1.0f, 1.0f);
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
	}
}

}