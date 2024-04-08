#include "NeHeOGL_Lesson005.h"

namespace sge {

NeHeOGL_Lesson005::~NeHeOGL_Lesson005() {
	if (_gluQuad) {
		gluDeleteQuadric(_gluQuad);
		_gluQuad = nullptr;
	}
}

void NeHeOGL_Lesson005::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);
	
	_gluQuad = gluNewQuadric();

	_cubeVertexs[0].pos.set(-d, d,-d); _cubeVertexs[0].color.set(OGL::kbRed);
	_cubeVertexs[1].pos.set( d, d,-d); _cubeVertexs[1].color.set(OGL::kbRed);
	_cubeVertexs[2].pos.set( d, d, d); _cubeVertexs[2].color.set(OGL::kbRed);
	_cubeVertexs[3].pos.set(-d, d, d); _cubeVertexs[3].color.set(OGL::kbRed);

	_cubeVertexs[4].pos.set(-d,-d,-d); _cubeVertexs[4].color.set(OGL::kbBlue);
	_cubeVertexs[5].pos.set( d,-d,-d); _cubeVertexs[5].color.set(OGL::kbBlue);
	_cubeVertexs[6].pos.set( d,-d, d); _cubeVertexs[6].color.set(OGL::kbBlue);
	_cubeVertexs[7].pos.set(-d,-d, d); _cubeVertexs[7].color.set(OGL::kbBlue);

	_mesh.createCube(3, 2, 1);
//	_mesh.renderState.wireframe = true;
}

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
//	_example1();	// draw cone, cube
//	_example2();	// depth test
//	_example3();	// stencil test
//	_example4();	// scissor test
//	_example5();	// draw sphere
	_example6();	// vertex array
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

	_drawMyGrid();
	_drawMyCoordinate();

//	_cullFace();
//	_frontFace();
//	_depthTest();
//	_bufferWriteMask();
//	_depthTest();
	_zFight();

	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson005::_cullFace() {
//	GLint o;
//	glDisable(GL_CULL_FACE);
//	glGetIntegerv(GL_CULL_FACE, &o);
//	SGE_DUMP_VAR(o);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

//	glGetIntegerv(GL_CULL_FACE, &o);
//	SGE_DUMP_VAR(o);

	OGL::color4f(OGL::kRed);
	glTranslatef(0, 1.5f, 0);
	glBegin(GL_TRIANGLES);
		glVertex3f(-0.5f, 0.0f, 0);
		glVertex3f( 0.5f, 0.0f, 0);
		glVertex3f( 0.0f, 0.5f, 0);
	glEnd();
}

void NeHeOGL_Lesson005::_frontFace() {
//	GLint o;
//	glFrontFace(GL_CCW);
//	glGetIntegerv(GL_FRONT_FACE, &o);
//	SGE_DUMP_VAR(o == GL_CCW);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW); // The default value is GL_CCW 
	glCullFace(GL_BACK);

//	glGetIntegerv(GL_FRONT_FACE, &o);
//	SGE_DUMP_VAR(o == GL_CW);

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

	{ // make sure we don't update the stencil buffer while drawing the grid and coordinate
		glStencilMask(0);
		_drawMyGrid();
		_drawMyCoordinate();
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

	_drawMyGrid();
	_drawMyCoordinate();

	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson005::_example5() {
	float width  = _clientRect.w;
	float height = _clientRect.h;

	float aspect = width / height;
	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	glDisable(GL_SCISSOR_TEST);
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

	_drawMyGrid();
	_drawMyCoordinate();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glPushMatrix();
		glTranslatef(-2, 0, 0);
		_drawMySphere(1, 16, 16);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(2, 0, 0);
		_drawMygluSphere(1, 16, 16);
	glPopMatrix();

	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson005::_drawMySphere(float radius, int subAxis, int subHeight) {
	if (subAxis < 2 || subHeight < 2)
		return;

	Vec3f top    { 0,  radius, 0 };
	Vec3f bottom { 0, -radius, 0 };

	glEnable(GL_CULL_FACE);

	OGL::color4f(OGL::kWhite);

	{ // top
		OGL::ScopedBegin s(NeHe_BeginMode::Triangles);
		int y = 1;
		float ty0 = PI * y / subHeight;
		float r0, h0;
		Math::sincos(ty0, r0, h0);

		for (int a = 0; a < subAxis; ++a) {
			float th0 = PI2 * a / subAxis;
			float th1 = PI2 * (a + 1) / subAxis;

			Vec3f p0, p1;
			p0.y = p1.y = 0;
			Math::sincos(th0, p0.x, p0.z);
			Math::sincos(th1, p1.x, p1.z);

			Vec3f v0 = p0 * radius * r0;
			Vec3f v1 = p1 * radius * r0;
			v0.y = v1.y = h0;

			OGL::vertex3f(top);
			OGL::vertex3f(v1);
			OGL::vertex3f(v0);
		}
	}

	{ // middle
		OGL::ScopedBegin s(NeHe_BeginMode::Triangles);
		for (int y = 1; y < subHeight - 1; y++) {

			float ty0 = PI * y / subHeight;
			float ty1 = PI * (y+1) / subHeight;

			float r0 = Math::sin(ty0);
			float r1 = Math::sin(ty1);

			for (int a = 0; a < subAxis; ++a) {
				float th0 = PI2 * a / subAxis;
				float th1 = PI2 * (a+1) / subAxis;

				Vec3f p0, p1;
				p0.y = p1.y = 0;
				Math::sincos(th0, p0.x, p0.z);
				Math::sincos(th1, p1.x, p1.z);

				Vec3f v0 = p0 * radius * r0;
				Vec3f v1 = p1 * radius * r0;

				Vec3f v2 = p0 * radius * r1;
				Vec3f v3 = p1 * radius * r1;

				v0.y = v1.y = Math::cos(ty0);
				v2.y = v3.y = Math::cos(ty1);

				OGL::vertex3f(v0);
				OGL::vertex3f(v1);
				OGL::vertex3f(v3);

				OGL::vertex3f(v0);
				OGL::vertex3f(v3);
				OGL::vertex3f(v2);
			}
		}
	}

	{ // bottom
		OGL::ScopedBegin s(NeHe_BeginMode::Triangles);

		int y = subHeight - 2;
		float ty1 = PI * (y+1) / subHeight;
		float r1, h1;
		Math::sincos(ty1, r1, h1);

		for (int a = 0; a < subAxis; ++a) {
			float th0 = PI2 * a / subAxis;
			float th1 = PI2 * (a+1) / subAxis;

			Vec3f p0, p1;
			p0.y = p1.y = 0;
			Math::sincos(th0, p0.x, p0.z);
			Math::sincos(th1, p1.x, p1.z);

			Vec3f v0 = p0 * radius * r1;
			Vec3f v1 = p1 * radius * r1;
			v0.y = v1.y = h1;

			OGL::vertex3f(bottom);
			OGL::vertex3f(v0);
			OGL::vertex3f(v1);
		}
	}

	glDisable(GL_CULL_FACE);
}

void NeHeOGL_Lesson005::_drawMygluSphere(double radius, int slices, int stacks) {
	gluSphere(_gluQuad, radius, slices, stacks);
}

void NeHeOGL_Lesson005::_example6() {
	float width = _clientRect.w;
	float height = _clientRect.h;

	float aspect = width / height;
	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	glDisable(GL_SCISSOR_TEST);
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

	_drawMyGrid();
	_drawMyCoordinate();

//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	float s = 0.5f;
	float x = -3.f;
	float deltaX = 2.f;

#define E(FUNC_NAME) \
	glPushMatrix(); \
		glTranslatef(x, 0, 0); \
		glScalef(s, s, s); \
		FUNC_NAME(); \
		x += deltaX; \
	glPopMatrix(); \
// -----
	E(_drawMyCube1)
	E(_drawMyCube2)
	E(_drawMyCube3)
	E(_drawMyCube4)
	E(_drawMyCube5)
#undef E

	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson005::_drawMyCube1() {
	{ // glArrayElement with glBegin
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, kCube);

		OGL::ScopedBegin s(NeHe_BeginMode::Triangles);
		glArrayElement(0); glArrayElement(2); glArrayElement(1); // top
		glArrayElement(0); glArrayElement(3); glArrayElement(2);
		glArrayElement(3); glArrayElement(6); glArrayElement(2); // front
		glArrayElement(3); glArrayElement(7); glArrayElement(6);
		glArrayElement(2); glArrayElement(5); glArrayElement(1); // rigt
		glArrayElement(2); glArrayElement(6); glArrayElement(5);
		glArrayElement(7); glArrayElement(5); glArrayElement(6); // bottom
		glArrayElement(7); glArrayElement(4); glArrayElement(5);
		glArrayElement(4); glArrayElement(1); glArrayElement(5); // back
		glArrayElement(4); glArrayElement(0); glArrayElement(1);
		glArrayElement(4); glArrayElement(3); glArrayElement(0); // left
		glArrayElement(4); glArrayElement(7); glArrayElement(3);

		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void NeHeOGL_Lesson005::_drawMyCube2() {
	static constexpr float a = 1.0f;
	static constexpr float kCube_Quad[24][3] = {
		{ -a,-a, a }, {  a,-a, a }, {  a, a, a }, { -a, a, a },	// front
		{ -a,-a,-a }, { -a, a,-a }, {  a, a,-a }, {  a,-a,-a }, // back
		{ -a, a,-a }, { -a, a, a }, {  a, a, a }, {  a, a,-a }, // top
		{ -a,-a,-a }, {  a,-a,-a }, {  a,-a, a }, { -a,-a, a }, // bottom
		{  a,-a,-a }, {  a, a,-a }, {  a, a, a }, {  a,-a, a }, // right
		{ -a,-a,-a }, { -a,-a, a }, { -a, a, a }, { -a, a,-a }, // left
	};

	{ // glDrawArrays without glBegin
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, kCube_Quad);

		GLsizei vertexCount = static_cast<GLsizei>(sizeof(kCube_Quad) / sizeof(kCube_Quad[0]));
		glDrawArrays(GL_QUADS, 0, vertexCount);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void NeHeOGL_Lesson005::_drawMyCube3() {
	static constexpr Tuple3f kCubePos[8] = {
		{ -d, d,-d },
		{  d, d,-d },
		{  d, d, d },
		{ -d, d, d },
		{ -d,-d,-d },
		{  d,-d,-d },
		{  d,-d, d },
		{ -d,-d, d },
	};

	{ // glDrawElements without glBegin
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, kCubePos);

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(kCubeIndicesCount), kIndexType, kCubeIndices);

		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void NeHeOGL_Lesson005::_drawMyCube4() {
	{ // glColorPointer, glDrawElements without glBegin
		int stride = sizeof(_cubeVertexs[0]);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, stride, _cubeVertexs[0].pos.data);

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, stride, _cubeVertexs[0].color.data);

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(kCubeIndicesCount), kIndexType, kCubeIndices);

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void NeHeOGL_Lesson005::_drawMyCube5() {
	_mesh.draw();
}

void NeHeOGL_Lesson005::_drawMyGrid() {
	float oldColor[4];
	glGetFloatv(GL_CURRENT_COLOR, oldColor);
		float oldValue;
		glGetFloatv(GL_LINE_WIDTH, &oldValue);
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
		glLineWidth(oldValue);
	glColor4fv(oldColor);
}

void NeHeOGL_Lesson005::_drawMyCoordinate() {
	float oldColor[4];
	glGetFloatv(GL_CURRENT_COLOR, oldColor);
		float oldValue;
		glGetFloatv(GL_LINE_WIDTH, &oldValue);
		glLineWidth(2);
			glBegin(GL_LINES);
				OGL::color4f(OGL::kRed);	OGL::vertex3f(Vec3f::s_zero());	OGL::vertex3f(Vec3f::s_right());
				OGL::color4f(OGL::kGreen);	OGL::vertex3f(Vec3f::s_zero());	OGL::vertex3f(Vec3f::s_up());
				OGL::color4f(OGL::kBlue);	OGL::vertex3f(Vec3f::s_zero());	OGL::vertex3f(Vec3f::s_forward());
			glEnd();
		glLineWidth(oldValue);
	glColor4fv(oldColor);
}

}