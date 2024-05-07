#include "NeHeOGL_Lesson008.h"

namespace sge {

void NeHeOGL_Lesson008::onInitedGL() {
	_rOpaque.createRect(1, 1);
	_rOpaque.renderState.cull = RenderState_Cull::None;

	_gTransparent.createRect(1, 1);
	_gTransparent.renderState.cull = RenderState_Cull::None;
	for (auto& v : _gTransparent.vertices) {
		v.color.set(255,255,255,128);
	}

	_bTransparent.createRect(1, 1);
	_bTransparent.renderState.cull = RenderState_Cull::None;
	for (auto& v : _bTransparent.vertices) {
		v.color.set(255, 255, 255, 128);
	}

	_gridMesh.createGrid(10);
	_gridMesh.renderState.wireframe = true;

	_coordinateMesh.createCoordinate();

	{
		NeHeOGL_Texture2D::CreateDesc desc;
		desc.imageToUpload.create(ColorType::RGBAb, 256, 256);
		{
			desc.imageToUpload.fill(OGL::kbRed);
			_rTex.create(desc);
		}
		{
			desc.imageToUpload.fill(OGL::kbGreen);
			_gTex.create(desc);
		}
		{
			desc.imageToUpload.fill(OGL::kbBlue);
			_bTex.create(desc);
		}
	}

	_opaqueRect.createRect(1, 1);
	_opaqueRect.renderState.cull = RenderState_Cull::None;

	_transparentRect.createRect(1, 1);
	_transparentRect.renderState.cull = RenderState_Cull::None;

	_cubeMesh.createCube(1, 1, 1);

	{
		NeHeOGL_Texture2D::CreateDesc desc;
		desc.imageToUpload.loadFile("ColorBleeding.png");
		desc.samplerState.filter = TextureFilter::Linear;
		_colorBleedingTex.create(desc);
	}

	{
		NeHeOGL_Texture2D::CreateDesc desc;
		desc.samplerState.filter = TextureFilter::Linear;
		desc.imageToUpload.setPreMultipliedAlpha(true);
		desc.imageToUpload.loadFile("ColorBleeding.png");
		_pmaTex.create(desc);
	}

	{
		NeHeOGL_Texture2D::CreateDesc desc;
		desc.samplerState.filter = TextureFilter::Linear;
		desc.imageToUpload.loadFile("Glass.bmp");
		_glassTex.create(desc);
	}

	OGLUtil::throwIfError();
}

void NeHeOGL_Lesson008::onUIMouseEvent(UIMouseEvent& ev) {
	using Button	= UIMouseEvent::Button;
	using Type		= UIMouseEvent::Type;
	using Modifier	= UIEventModifier;

	if (ev.isDragging()) {
		switch (ev.pressedButtons) {
			case Button::Left: {
				if (BitUtil::hasAny(ev.modifier, Modifier::Alt)) {
					float d = 0.15f;
					_camerOrbitAngle += ev.deltaPos.yx() * d;
				}
			} break;
			case Button::Right: {
				float d = 0.15f;
				_camerOrbitAngle += ev.deltaPos.yx() * d;
			} break;
			case Button::Middle: {
				float d = 0.01f;
				_camerMovePos.x += ev.deltaPos.x * d;
				_camerMovePos.y += ev.deltaPos.y * -d;
			} break;
		}
	}

	if (ev.isScroll()) {
		auto d = ev.scroll * 0.005f;
		_camerMovePos.z -= d.y;
	}
}

void NeHeOGL_Lesson008::onUIKeyboardEvent(UIKeyboardEvent& ev) {
	if (ev.isDown(KeyCode::LeftArrow)) {
		_rotXY.w -= 0.01f;
	}
	if (ev.isDown(KeyCode::RightArrow)) {
		_rotXY.w += 0.01f;
	}

	if (ev.isDown(KeyCode::UpArrow)) {
		_rotXY.z -= 0.01f;
	}
	if (ev.isDown(KeyCode::DownArrow)) {
		_rotXY.z += 0.01f;
	}

	if (ev.isDown(KeyCode::PageUp)) {
		_camerMovePos.z += 0.02f;
	}
	if (ev.isDown(KeyCode::PageDown)) {
		_camerMovePos.z -= 0.02f;
	}

	if (!_isPressedB && ev.isDown(KeyCode::B)) {
		_isBlend = !_isBlend;
		_isPressedB = true;
	}
	if (ev.isUp(KeyCode::B)) {
		_isPressedB = false;
	}
}

void NeHeOGL_Lesson008::_beginOpaquePass() {
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void NeHeOGL_Lesson008::_endOpaquePass() {

}

void NeHeOGL_Lesson008::_beginTransparentPass() {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void NeHeOGL_Lesson008::_endTransparentPass() {
	glDepthMask(GL_TRUE);
}

void NeHeOGL_Lesson008::_setViewport() {
	glViewport(0, 0, static_cast<int>(_clientRect.w), static_cast<int>(_clientRect.h));
}

void NeHeOGL_Lesson008::_setDefaultRenderState() {
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_TEXTURE_2D);

	glColor4f(1, 1, 1, 1);
	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClearDepth(1);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
}

void NeHeOGL_Lesson008::_initCamera() {
	float width  = _clientRect.w;
	float height = _clientRect.h;

	if (height == 0) height = 1;

	float aspect = width / height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, aspect, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// setup camera
	glTranslatef(0, 0, -_camerMovePos.z);
	glRotatef(_camerOrbitAngle.x, 1, 0, 0);
	glRotatef(_camerOrbitAngle.y, 0, 1, 0);
}

void NeHeOGL_Lesson008::_beginRender() {
	_setViewport();
	_setDefaultRenderState();
	_initCamera();

// ----
	OGL::drawGridAndCoordinate(_gridMesh, _coordinateMesh);
}

void NeHeOGL_Lesson008::_endRender() {
	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson008::onDraw() {
	_beginRender();

		glTranslatef(0, 0.5f, 0);

//		_example1(); // blending issue: transparent objectA block opaque objectB
//		_example2(); // blending issue: transparent objectA block transparent objectB
//		_example3(); // normal blending
//		_example4(); // blending issue: color bleeding
//		_example5(); // pre-multiplied alpha
		_example6(); // NeHe blending example

	_endRender();
}

void NeHeOGL_Lesson008::_example1() {
	// issue: transparent objectA(_gTransparent) block opaque objectB(_rOpaque)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// _gTransparent -> _rOpaque -> _bTransparent
	{
		OGL::Scoped_glPushMatrix scoped;
		glTranslatef(0.3f, 0.1f, -0.4f);
		_gTex.bind();
			_gTransparent.draw();
		_gTex.unbind();
	}
	{
		OGL::Scoped_glPushMatrix scoped;
		_rTex.bind();
			_rOpaque.draw();
		_rTex.unbind();
	}
	{
		OGL::Scoped_glPushMatrix scoped;
		glTranslatef(0.5f, -0.1f, 0.4f);
		_bTex.bind();
			_bTransparent.draw();
		_bTex.unbind();
	}
}

void NeHeOGL_Lesson008::_example2() {
	// issue: transparent objectA(_gTransparent) block transparent objectB(_bTransparent)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// _rOpaque -> _gTransparent -> _bTransparent
	{
		OGL::Scoped_glPushMatrix scoped;
		_rTex.bind();
			_rOpaque.draw();
		_rTex.unbind();
	}
	{
		OGL::Scoped_glPushMatrix scoped;
		glTranslatef(0.3f, 0.1f, -0.4f);
		_gTex.bind();
		_gTransparent.draw();
		_gTex.unbind();
	}
	{
		OGL::Scoped_glPushMatrix scoped;
		glTranslatef(0.5f, -0.1f, 0.4f);
		_bTex.bind();
		_bTransparent.draw();
		_bTex.unbind();
	}
}

void NeHeOGL_Lesson008::_example3() {
	// draw order: opaque pass -> transparent pass

	_beginOpaquePass();
	{
		OGL::Scoped_glPushMatrix scoped;
		_rTex.bind();
			_rOpaque.draw();
		_rTex.unbind();
	}
	_endOpaquePass();

	_beginTransparentPass();
	{
		{
			OGL::Scoped_glPushMatrix scoped;
			glTranslatef(0.3f, 0.1f, -0.4f);
			_gTex.bind();
				_gTransparent.drawInherit();
			_gTex.unbind();
		}

		{
			OGL::Scoped_glPushMatrix scoped;
			glTranslatef(0.5f, -0.1f, 0.4f);
			_bTex.bind();
				_bTransparent.drawInherit();
			_bTex.unbind();
		}
	}
	_endTransparentPass();
}

void NeHeOGL_Lesson008::_example4() {
	// issue: color bleeding
	_colorBleedingTex.bind();

	_beginOpaquePass();
	{
		OGL::Scoped_glPushMatrix scoped;
		glTranslatef(0.7f, 0, 0);
		_opaqueRect.draw();
	}
	_endOpaquePass();

	_beginTransparentPass();
	{
		OGL::Scoped_glPushMatrix scoped;
		glTranslatef(-0.7f, 0, 0);
		_transparentRect.drawInherit();
	}
	_endTransparentPass();

	_colorBleedingTex.unbind();
}

void NeHeOGL_Lesson008::_example5() {
_colorBleedingTex.bind();

	_beginOpaquePass();
	{
		OGL::Scoped_glPushMatrix scoped;
		glTranslatef(0, 1, 0);
		_opaqueRect.draw();
	}
	_endOpaquePass();

	_beginTransparentPass();
	{
		OGL::Scoped_glPushMatrix scoped;
		glTranslatef(-0.7f, 0, 0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Non-premultiplied alpha
		_transparentRect.drawInherit();
	}
_colorBleedingTex.unbind();

	{
		OGL::Scoped_glPushMatrix scoped;
		glTranslatef(0.7f, 0, 0);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // pre-multiplied alpha
		_pmaTex.bind();
			_transparentRect.drawInherit();
		_pmaTex.unbind();
	}
	_endTransparentPass();
}

void NeHeOGL_Lesson008::_example6() {
	glColor4f(1.0f, 1.0f, 1.0f, 0.5);	// Full Brightnes. 50% Alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);	// Set The Blending Function For Translucency

	if (_isBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDisable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

	glTranslatef(0, 0.7f, 0);

	glRotatef(_rotXY.x, 1.0f, 0.0f, 0.0f);
	glRotatef(_rotXY.y, 0.0f, 1.0f, 0.0f);

	_glassTex.bind();
		_cubeMesh.drawInherit();
	_glassTex.unbind();

	_rotXY.x += _rotXY.z;
	_rotXY.y += _rotXY.w;
}

}