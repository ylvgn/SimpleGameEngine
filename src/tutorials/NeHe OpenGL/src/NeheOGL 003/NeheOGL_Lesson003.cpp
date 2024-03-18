#include "NeHeOGL_Lesson003.h"

namespace sge {

void NeHeOGL_Lesson003::onDraw() {
	//_example1();
	_example2();
}

void NeHeOGL_Lesson003::_example1() {
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
	glTranslatef(-0.5f, 0.f, -6.f);
	float d = 0.5f;
	glBegin(GL_TRIANGLES);
		_glR(); glVertex3f( 0.f, d, 0.f); // Set The Color To Red
		_glG(); glVertex3f(-d,  -d, 0.f); // Set The Color To Green
		_glB(); glVertex3f( d,  -d, 0.f); // Set The Color To Blue
	glEnd();

	glColor3f(0.5f, 0.5f, 1.0f);	// Set The Color To Blue One Time Only
	glTranslatef(1.f, 0.f, 0.f);
	glBegin(GL_QUADS);
		glVertex3f(-d,  d, 0.f);
		glVertex3f( d,  d, 0.f);
		glVertex3f( d, -d, 0.f);
		glVertex3f(-d, -d, 0.f);
	glEnd();

	swapBuffers();
}

void NeHeOGL_Lesson003::_example2() {
	glViewport(0, 0, static_cast<int>(_clientRect.w), static_cast<int>(_clientRect.h));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, _clientRect.w / _clientRect.h, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0.f, 0.f, -8.f);

	const float rotateAngleInDegree = 72.f;
	const float starAngleInDegree = 60.f;

	float hStar = g_rad(starAngleInDegree*0.5f);
	float hRotate = g_rad(rotateAngleInDegree * 0.5f);

	float e = 2.f;	
	float d1 = e * Math::sin(hStar);
	d1 = d1 / Math::tan(hRotate);
	float d2 = e * Math::cos(hStar);
	float d = d1 + d2;

	float pa[3] = {  0,  d, 0 };
	float pb[3] = { -1, d1, 0 };
	float pc[3] = {  1, d1, 0 };

	float v[15][3];
	float curAngle = 0.f;
	for (int i = 0; i < 15; i +=3) {
		float angleInRad = g_rad(curAngle);
		float c, s;
		Math::sincos(angleInRad, s, c);

		v[i+0][0] = pa[0] * c + pa[1] * -s;
		v[i+0][1] = pa[0] * s + pa[1] * c;
		v[i+0][2] = 0;
		
		v[i+1][0] = pb[0] * c + pb[1] * -s;
		v[i+1][1] = pb[0] * s + pb[1] * c;
		v[i+1][2] = 0;
		
		v[i+2][0] = pc[0] * c + pc[1] * -s;
		v[i+2][1] = pc[0] * s + pc[1] * c;
		v[i+2][2] = 0;

		curAngle += rotateAngleInDegree;
	};
	
	int mode = GL_POLYGON;
	switch (mode) {
		case GL_POLYGON:
			glBegin(mode);
				_glW(); glVertex3f(0,0,0); // origin(o)
#if 0
				Color4f c;
				for (int i = 0, st = 0, step = 1; i < 10; ++i) {
					c = step == 2 ? Color4f{ 1, 1, 1, 1 } : Color4f{ 1, 0, 0, 1 };
					_glColor(c); glVertex3fv(v[st % 15]);
					c = step == 1 ? Color4f{ 1, 1, 1, 1 } : Color4f{ 1, 0, 0, 1 };
					_glColor(c); glVertex3fv(v[(st + step) % 15]);

					st += step;
					step = 3 - step; // step is 1 or 2
				}
#else
				_glR(); glVertex3fv(v[0]);
				_glY(); glVertex3fv(v[1]);  // o,0,1
				_glY(); glVertex3fv(v[1]);
				_glR(); glVertex3fv(v[3]);  // o,1,3
				_glR(); glVertex3fv(v[3]);
				_glG(); glVertex3fv(v[4]);  // o,3,4
				_glG(); glVertex3fv(v[4]);
				_glR(); glVertex3fv(v[6]);  // o,4,6
				_glR(); glVertex3fv(v[6]);
				_glC(); glVertex3fv(v[7]);  // o,6,7
				_glC(); glVertex3fv(v[7]);
				_glR(); glVertex3fv(v[9]);  // o,7,9
				_glR(); glVertex3fv(v[9]);
				_glB(); glVertex3fv(v[10]); // o,9,10
				_glB(); glVertex3fv(v[10]);
				_glR(); glVertex3fv(v[12]); // o,10,12
				_glR(); glVertex3fv(v[12]);
				_glP(); glVertex3fv(v[13]); // o,12,13
				_glP(); glVertex3fv(v[13]);
				_glR(); glVertex3fv(v[0]);  // o,13,0
#endif
			glEnd();
			break;
		case GL_POINTS:
			// total 11 points
			glPointSize(12);
			glBegin(mode);
				_glR(); glVertex3fv(v[0]);
				_glG(); glVertex3fv(v[1]);
				//_glB(); glVertex3fv(v[2]); 

				_glR(); glVertex3fv(v[3]); 
				_glG(); glVertex3fv(v[4]);
				//_glB(); glVertex3fv(v[5]);

				_glR(); glVertex3fv(v[6]);
				_glG(); glVertex3fv(v[7]);
				//_glB(); glVertex3fv(v[8]);

				_glR(); glVertex3fv(v[9]); 
				_glG(); glVertex3fv(v[10]);
				//_glB(); glVertex3fv(v[11]);

				_glR(); glVertex3fv(v[12]);
				_glG(); glVertex3fv(v[13]);
				_glB(); glVertex3fv(v[14]);
			glEnd();
			break;
		case GL_TRIANGLES:
			glBegin(mode);
				_glR(); glVertex3fv(v[0]);
				_glG(); glVertex3fv(v[1]);
				_glB(); glVertex3fv(v[2]);

				_glR(); glVertex3fv(v[3]);
				_glG(); glVertex3fv(v[4]);
				_glB(); glVertex3fv(v[5]);

				_glR(); glVertex3fv(v[6]);
				_glG(); glVertex3fv(v[7]);
				_glB(); glVertex3fv(v[8]);

				_glR(); glVertex3fv(v[9]);
				_glG(); glVertex3fv(v[10]);
				_glB(); glVertex3fv(v[11]);

				_glR(); glVertex3fv(v[12]);
				_glG(); glVertex3fv(v[13]);
				_glB(); glVertex3fv(v[14]);
			glEnd();
			break;
		case GL_LINE_LOOP:
			// total 10 lines, and the last line will auto connected cuz line loop
			glBegin(mode);
				glVertex3fv(v[0]);
				glVertex3fv(v[1]);
				//glVertex3fv(v[5]);
				glVertex3fv(v[3]);
				glVertex3fv(v[4]);
				//glVertex3fv(v[8]);
				glVertex3fv(v[6]);
				glVertex3fv(v[7]);
				//glVertex3fv(v[11]);
				glVertex3fv(v[9]);
				glVertex3fv(v[10]);
				//glVertex3fv(v[14]);
				glVertex3fv(v[12]);
				glVertex3fv(v[13]);
				//glVertex3fv(v[2]);
			glEnd();
			break;
	}

	swapBuffers();
}


}