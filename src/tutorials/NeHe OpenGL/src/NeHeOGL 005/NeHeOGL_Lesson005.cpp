#include "NeHeOGL_Lesson005.h"

namespace sge {

void NeHeOGL_Lesson005::onUIMouseEvent(UIMouseEvent& ev) {
	using Button = UIMouseEvent::Button;

	switch (ev.pressedButtons) {
		case Button::Right: {
			auto dt =  ev.deltaPos * 0.25f;
			_cameraX += dt.y;
			_cameraY += dt.x;
		} break;
	}
}

void NeHeOGL_Lesson005::onDraw() {
	//_example1();
	//_example2();
	//_example3();	// stencil test
	_example4();	// scissor test
}

void NeHeOGL_Lesson005::_example1() {
	auto uptime = static_cast<float>(_uptime.get());

	static constexpr float kRotateSpeed = 90.f;
	float angleInDegrees = uptime * kRotateSpeed;

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

	_wireFrame(angleInDegrees);
	_shaded(angleInDegrees);

	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson005::_wireFrame(float angleInDegrees) {
	glLineWidth(2.f);

	{
		auto& v = kCone;
		glLoadIdentity();
		glTranslatef(-1.5f, 0.f, -6.f);
		glRotatef(angleInDegrees, 0.0f, 1.0f, 0.0f);
		OGL::color4f(OGL::kWhite);
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
		glRotatef(angleInDegrees, 1.0f, 1.0f, 1.0f);
		OGL::color4f(OGL::kWhite);
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

void NeHeOGL_Lesson005::_shaded(float angleInDegrees) {
	{
		auto& v = kCone;
		glLoadIdentity();
		glTranslatef(-1.5f, 0.f, -6.f);
		glRotatef(angleInDegrees, 0.0f, 1.0f, 0.0f);
		glBegin(GL_TRIANGLES); // Note that all triangles are drawn in a counter clockwise order rotation
			// front
			OGL::color4f(OGL::kRed);	glVertex3fv(v[0]);
			OGL::color4f(OGL::kGreen);	glVertex3fv(v[3]);
			OGL::color4f(OGL::kBlue);	glVertex3fv(v[4]);

			// right
			OGL::color4f(OGL::kRed);	glVertex3fv(v[0]);
			OGL::color4f(OGL::kGreen);	glVertex3fv(v[4]);
			OGL::color4f(OGL::kBlue);	glVertex3fv(v[2]);

			// back
			OGL::color4f(OGL::kRed);	glVertex3fv(v[0]);
			OGL::color4f(OGL::kGreen);	glVertex3fv(v[2]);
			OGL::color4f(OGL::kBlue);	glVertex3fv(v[1]);

			// left
			OGL::color4f(OGL::kRed);	glVertex3fv(v[0]);
			OGL::color4f(OGL::kGreen);	glVertex3fv(v[1]);
			OGL::color4f(OGL::kBlue);	glVertex3fv(v[3]);
		glEnd();
	}

	{
		auto& v = kCube;
		glLoadIdentity();
		glTranslatef(1.f, 0.f, -7.0f);
		glRotatef(angleInDegrees, 1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
			// All of the quads are drawn in a counter clockwise order
				// Meaning the first point is the top right,
				// the second point is the top left,
				// third point is bottom left,
				// and finally bottom right
			// eyes(viewers) look at each face from cube's outside in front of your screen, TR->TL->BL->BR (counter clockwise order)
			// ignoring the order the quad is drawn in can cause weird results once you get into fancy things such as texture mapping
			OGL::color4f(OGL::kGreen);	glVertex3fv(v[1]); glVertex3fv(v[0]); glVertex3fv(v[3]); glVertex3fv(v[2]); // top
			OGL::color4f(OGL::kOrange);	glVertex3fv(v[6]); glVertex3fv(v[7]); glVertex3fv(v[4]); glVertex3fv(v[5]); // down, rotate 6 to 1 then you see this order

			OGL::color4f(OGL::kRed);	glVertex3fv(v[2]); glVertex3fv(v[3]); glVertex3fv(v[7]); glVertex3fv(v[6]); // front, 'rotate 6 to 1', so 2376 fits to 1032
			OGL::color4f(OGL::kYellow); glVertex3fv(v[5]); glVertex3fv(v[4]); glVertex3fv(v[0]); glVertex3fv(v[1]); // back

			OGL::color4f(OGL::kBlue);	glVertex3fv(v[0]); glVertex3fv(v[4]); glVertex3fv(v[7]); glVertex3fv(v[3]); // left
			OGL::color4f(OGL::kViolet); glVertex3fv(v[5]); glVertex3fv(v[1]); glVertex3fv(v[2]); glVertex3fv(v[6]); // right
		glEnd();
	}
}

void NeHeOGL_Lesson005::_example2() {
	float width = _clientRect.w;
	float height = _clientRect.h;

	float aspect = width / height;
	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90.f, aspect, 0.01f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	glTranslatef(0, 0, -3.0f);
	glRotatef(_cameraX, 1,0,0);
	glRotatef(_cameraY, 0,1,0);

	// draw grid
	glLineWidth(1);
	OGL::color4f(OGL::kWhite);
	glBegin(GL_LINES);
		for (float x = -10; x <= 10; x++) {
			glVertex3f(x, 0, -10);
			glVertex3f(x, 0, 10);
		}
		for (float z = -10; z <= 10; z++) {
			glVertex3f(-10, 0, z);
			glVertex3f(10, 0, z);
		}
	glEnd();

	// draw coordinate
	glLineWidth(2);
	glBegin(GL_LINES);
		OGL::color4f(OGL::kRed);	OGL::vertex3f(Vec3f::s_zero()); OGL::vertex3f(Vec3f::s_right());
		OGL::color4f(OGL::kGreen);	OGL::vertex3f(Vec3f::s_zero()); OGL::vertex3f(Vec3f::s_up());
		OGL::color4f(OGL::kBlue);	OGL::vertex3f(Vec3f::s_zero()); OGL::vertex3f(Vec3f::s_forward());
	glEnd();

	//_cullFace();
	//_frontFace();
	//_depthTest();
	//_bufferWriteMask();
	//_depthTest();
	_zFight();

	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson005::_cullFace() {
	//GLint o;
	//glDisable(GL_CULL_FACE);
	//glGetIntegerv(GL_CULL_FACE, &o);
	//SGE_DUMP_VAR(o);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//glGetIntegerv(GL_CULL_FACE, &o);
	//SGE_DUMP_VAR(o);

	OGL::color4f(OGL::kRed);
	glTranslatef(0, 1.5f, 0);
	glBegin(GL_TRIANGLES);
		glVertex3f(-0.5f, 0.0f, 0);
		glVertex3f( 0.5f, 0.0f, 0);
		glVertex3f( 0.0f, 0.5f, 0);
	glEnd();
}

void NeHeOGL_Lesson005::_frontFace() {
	//GLint o;
	//glFrontFace(GL_CCW);
	//glGetIntegerv(GL_FRONT_FACE, &o);
	//SGE_DUMP_VAR(o == GL_CCW);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW); // The default value is GL_CCW 
	glCullFace(GL_BACK);

	//glGetIntegerv(GL_FRONT_FACE, &o);
	//SGE_DUMP_VAR(o == GL_CW);

	OGL::color4f(OGL::kRed);
	glTranslatef(0, 1.5f, 0);
	glBegin(GL_TRIANGLES);
		glVertex3f(-0.5f, 0.0f, 0);
		glVertex3f(0.5f, 0.0f, 0);
		glVertex3f(0.0f, 0.5f, 0);
	glEnd();
}

void NeHeOGL_Lesson005::_depthTest() {
	glEnable(GL_DEPTH_TEST);	// comment this line to check depth test
	glDepthFunc(GL_LESS);		// GL_LESS is default value
	glDepthRange(0, 1);			// set depth range

	// set all pixels depth value is 1, cuz 1 is zFar max range and use GL_LESS default to update depth value
	// so when some vertexs posZ < current depth buffer value, it will update depth buffer
	glClearDepth(1.0f);

	glPushMatrix();
		OGL::color4f(OGL::kBlue);
		glTranslatef(0, 1, 0);
		glBegin(GL_TRIANGLES);
			glVertex3f(-0.5f, 0.0f, 0);
			glVertex3f(0.5f, 0.0f, 0);
			glVertex3f(0.0f, 0.5f, 0);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		const float degreesInAngle = 60.f;
		OGL::color4f(OGL::kRed);
		glTranslatef(0, 1.1f, -0.2f);
		glRotatef(degreesInAngle, 1, 0, 0);
		glBegin(GL_TRIANGLES);
			glVertex3f(-0.5f, 0.0f, 0);
			glVertex3f( 0.5f, 0.0f, 0);
			glVertex3f( 0.0f, 0.5f, 0);
		glEnd();
	glPopMatrix();
}

void NeHeOGL_Lesson005::_bufferWriteMask() {
	{ // depth buffer
		// if we want enable depth test but we dont want to update depth buffer value
		glEnable(GL_DEPTH_TEST);
		glDepthMask(0); // If flag is zero, depth-buffer writing is disabled
	}

	{ // color buffer (frame buffer)
		glColorMask(1, 0, 1, 0); // means only red channel can update color buffer
	}

	glPushMatrix();
		OGL::color4f(OGL::kBlue);
		glTranslatef(0, 1, 0);
		glBegin(GL_TRIANGLES);
			glVertex3f(-0.5f, 0.0f, 0);
			glVertex3f(0.5f, 0.0f, 0);
			glVertex3f(0.0f, 0.5f, 0);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		const float angleInDegrees = 60.0f;
		OGL::color4f(OGL::kRed);
		glTranslatef(0, 1.1f, -0.2f);
		glRotatef(angleInDegrees, 1, 0, 0);
		glBegin(GL_TRIANGLES);
			glVertex3f(-0.5f, 0.0f, 0);
			glVertex3f( 0.5f, 0.0f, 0);
			glVertex3f( 0.0f, 0.5f, 0);
		glEnd();
	glPopMatrix();

	// reset default next begin frame
	glDepthMask(1);
	glColorMask(1, 1, 1, 1);
}

void NeHeOGL_Lesson005::_zFight() {

#define SGE_SOLUTION 2
	// 0: the problem
	// 1: solution 1 add z
	// 2: solution 2 glPolygonOffset

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LESS);

	Vec3f translate1{ 0, 1, 0 };

	glPushMatrix();
		OGL::color4f(OGL::kBlue);
		OGL::translatef(translate1);
		glBegin(GL_TRIANGLES);
			glVertex3f(-0.5f, 0.0f, 0);
			glVertex3f( 0.5f, 0.0f, 0);
			glVertex3f( 0.0f, 0.5f, 0);
		glEnd();
	glPopMatrix();

	static Vec3f translateOffset { -0.2f, 0, 0 };
	Vec3f translate2 = translate1 + translateOffset;
	
	glPushMatrix();
		OGL::color4f(OGL::kGreen);
#if (0 == SGE_SOLUTION)
		OGL::glTranslate(translate2);
#elif (1 == SGE_SOLUTION)
		glTranslatef(translate2.x, translate2.y, translate2.z + 0.001f); // add z value as small as possible to incase the depth buffer value precision
		// but when the two trignale near the far plane of the frustum, it may comes up the problem again.
#elif (2 == SGE_SOLUTION)
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(-1.f, 0.001f); // The value of the offset is factor * z + r * units
		// or glPolygonOffset(1.f, 0.001f);
		OGL::translatef(translate2);
#endif
		glBegin(GL_TRIANGLES);
			glVertex3f(-0.5f, 0.0f, 0);
			glVertex3f( 0.5f, 0.0f, 0);
			glVertex3f( 0.0f, 0.5f, 0);
		glEnd();
#if (2 == SGE_SOLUTION)
		glDisable(GL_POLYGON_OFFSET_FILL);
#endif
	glPopMatrix();

#undef SGE_SOLUTION
}

void NeHeOGL_Lesson005::_example3() {
	// FYI: https://learnopengl.com/Advanced-OpenGL/Stencil-testing

	float width  = _clientRect.w;
	float height = _clientRect.h;

	float aspect = width / height;
	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear color buffer, depth buffer, stencil buffer

	// configure opengl state
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);

	const GLuint stencilMask = 0xFF; // 0xFF cuz stencil buffer is 8 bit buffer
	glEnable(GL_STENCIL_TEST);	// Create OpenGL Window nees to set cStencilBits
		// GLint o = 123456;
		// glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &o);
		// SGE_DUMP_VAR(o);
	glClearStencil(0); // set stencil buffer value 0
	glStencilFunc(GL_ALWAYS, 0, stencilMask); // at the beginning, stencial buffer is fill with 0

	// But glStencilFunc only describes whether OpenGL should pass or discard fragments based on the stencil buffer's content,
	// not how we can actually update the buffer.
	// That is where glStencilOp comes in.
		// By default the glStencilOp function is set to (GL_KEEP, GL_KEEP, GL_KEEP)
		// so whatever the outcome of any of the tests, the stencil buffer keeps its values
		// The default behavior does not update the stencil buffer
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90.f, aspect, 0.01f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	glTranslatef(0, 0, -3.0f);
	glRotatef(_cameraX, 1,0,0);
	glRotatef(_cameraY, 0,1,0);

	{
		glStencilMask(0); // make sure we don't update the stencil buffer while drawing the grid and coordinate

		// draw grid
		glLineWidth(1);
		OGL::color4f(OGL::kWhite);
		glBegin(GL_LINES);
			for (float x = -10; x <= 10; x++) {
				glVertex3f(x, 0, -10);
				glVertex3f(x, 0, 10);
			}
			for (float z = -10; z <= 10; z++) {
				glVertex3f(-10, 0, z);
				glVertex3f(10, 0, z);
			}
		glEnd();

		// draw coordinate
		glLineWidth(2);
		glBegin(GL_LINES);
			OGL::color4f(OGL::kRed);	OGL::vertex3f(Vec3f::s_zero());		OGL::vertex3f(Vec3f::s_right());
			OGL::color4f(OGL::kGreen);	OGL::vertex3f(Vec3f::s_zero());		OGL::vertex3f(Vec3f::s_up());
			OGL::color4f(OGL::kBlue);	OGL::vertex3f(Vec3f::s_zero());		OGL::vertex3f(Vec3f::s_forward());
		glEnd();
	}

	// 1st. draw objects as normal, writing to the stencil buffer, and now the cube's per pixel update stencil buffer value to 1
	GLint myStencilRef = 1;
	glStencilFunc(GL_ALWAYS, myStencilRef, stencilMask);
	glStencilMask(stencilMask);
	glPushMatrix();
		glTranslatef(0, d, 0);
		float s = 0.5f;
		glScalef(s, s, s);
		OGL::color4f(OGL::kRed);
		glBegin(GL_QUADS);
			glVertex3fv(kCube[7]); glVertex3fv(kCube[6]); glVertex3fv(kCube[2]); glVertex3fv(kCube[3]); // Front Face
			glVertex3fv(kCube[0]); glVertex3fv(kCube[1]); glVertex3fv(kCube[5]); glVertex3fv(kCube[4]); // Back Face
			glVertex3fv(kCube[3]); glVertex3fv(kCube[2]); glVertex3fv(kCube[1]); glVertex3fv(kCube[0]); // Top Face
			glVertex3fv(kCube[4]); glVertex3fv(kCube[5]); glVertex3fv(kCube[6]); glVertex3fv(kCube[7]); // Bottom Face
			glVertex3fv(kCube[6]); glVertex3fv(kCube[5]); glVertex3fv(kCube[1]); glVertex3fv(kCube[2]); // Right face
			glVertex3fv(kCube[4]); glVertex3fv(kCube[7]); glVertex3fv(kCube[3]); glVertex3fv(kCube[0]); // Left Face
		glEnd();
	glPopMatrix();

	// 2nd. now draw slightly scaled versions of the objects, this time disabling stencil writing
	glStencilFunc(GL_NOTEQUAL, myStencilRef, stencilMask); // Passes if (ref & mask) != (stencil & mask).
	glStencilMask(0); // we dont want to update the stencil buff
	glDisable(GL_DEPTH_TEST);
		glPushMatrix();
			// when we draw a slightly scaled cube, it will check the cube's per pixel stencil buff wheather != myStencilRef
			// at 1st. the last cube's per pixel is assigned as myStencilRef
			// and now, current cube's outline per pixel is still 0 stencil buffer value
			// so the outline will pass the stencil test
			glTranslatef(0, d, 0);
			s += 0.01f;
			glScalef(s, s, s);
			OGL::color4f(OGL::kWhite); // and I colored passed pixel with white color.
			glBegin(GL_QUADS);
				glVertex3fv(kCube[7]); glVertex3fv(kCube[6]); glVertex3fv(kCube[2]); glVertex3fv(kCube[3]); // Front Face
				glVertex3fv(kCube[0]); glVertex3fv(kCube[1]); glVertex3fv(kCube[5]); glVertex3fv(kCube[4]); // Back Face
				glVertex3fv(kCube[3]); glVertex3fv(kCube[2]); glVertex3fv(kCube[1]); glVertex3fv(kCube[0]); // Top Face
				glVertex3fv(kCube[4]); glVertex3fv(kCube[5]); glVertex3fv(kCube[6]); glVertex3fv(kCube[7]); // Bottom Face
				glVertex3fv(kCube[6]); glVertex3fv(kCube[5]); glVertex3fv(kCube[1]); glVertex3fv(kCube[2]); // Right face
				glVertex3fv(kCube[4]); glVertex3fv(kCube[7]); glVertex3fv(kCube[3]); glVertex3fv(kCube[0]); // Left Face
			glEnd();
		glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glStencilMask(stencilMask);
	glStencilFunc(GL_ALWAYS, 0, stencilMask);

	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson005::_example4() {
	float width  = _clientRect.w;
	float height = _clientRect.h;

	float aspect = width / height;
	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	glDisable(GL_SCISSOR_TEST);
	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_SCISSOR_TEST);
	glScissor(300, 100, 400, 400); // 0,0 origin is left-bottom, and orientation right +x, up +y

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90.f, aspect, 0.01f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	glTranslatef(0, 0, -3.0f);
	glRotatef(_cameraX, 1,0,0);
	glRotatef(_cameraY, 0,1,0);

	// draw grid
	glLineWidth(1);
	OGL::color4f(OGL::kWhite);
	glBegin(GL_LINES);
		for (float x = -10; x <= 10; x++) {
			glVertex3f(x, 0, -10);
			glVertex3f(x, 0, 10);
		}
		for (float z = -10; z <= 10; z++) {
			glVertex3f(-10, 0, z);
			glVertex3f(10, 0, z);
		}
	glEnd();

	swapBuffers();
	drawNeeded();
}

}