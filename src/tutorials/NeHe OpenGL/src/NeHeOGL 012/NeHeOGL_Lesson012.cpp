#include "NeHeOGL_Lesson012.h"

namespace sge {

Color4b NeHeOGL_Lesson012::kTopColors[kColorCount];

void NeHeOGL_Lesson012::onInitedGL() {
	Base::onInitedGL();

	_camera.setPos(-0.32152906f, 14.181764f, 32.151882f);
	_camera.setAim(0, 0, 0);

	{
		NeHeOGL_Texture2D::CreateDesc desc;
		desc.imageToUpload.loadFile("Cube.bmp");

		// I've decided to use mip-mapping to make the texture look real smooth. I hate seeing pixels :)
		desc.samplerState.filter = TextureFilter::Linear;
		desc.samplerState.minLOD = 0;
		desc.samplerState.maxLOD = 3;

		_tex.create(desc);
	}

	for (int i = 0; i < kColorCount; ++i) {
		auto& c = kTopColors[i];
		auto newRGB = kBoxColors[i].rgb() / 2;
		c.set(newRGB.r, newRGB.g, newRGB.b, 255);
	}

	_buildLists();
}

void NeHeOGL_Lesson012::_buildLists() {
	{
		_boxMesh.pos.resize(5 * 4);
		_boxMesh.uv[0].resize(_boxMesh.pos.size());
		_boxMesh.normal.resize(_boxMesh.pos.size());

		auto* pos = _boxMesh.pos.begin();
		auto* uv  = _boxMesh.uv[0].begin();
		auto* nl  = _boxMesh.normal.begin();

		// Bottom Face
		nl->set(0, -1, 0); uv->set(1.f, 1.f); pos->set(-1.0f, -1.0f, -1.0f);nl++; uv++; pos++;	// Top Right Of The Texture and Quad
		nl->set(0, -1, 0); uv->set(0.f, 1.f); pos->set(1.0f, -1.0f, -1.0f);	nl++; uv++; pos++;	// Top Left Of The Texture and Quad
		nl->set(0, -1, 0); uv->set(0.f, 0.f); pos->set(1.0f, -1.0f, 1.0f);	nl++; uv++; pos++;	// Bottom Left Of The Texture and Quad
		nl->set(0, -1, 0); uv->set(1.f, 0.f); pos->set(-1.0f, -1.0f, 1.0f);	nl++; uv++; pos++;	// Bottom Right Of The Texture and Quad
		// Front Face
		nl->set(0,0,1); uv->set(0.f, 0.f); pos->set(-1.0f, -1.0f, 1.0f);	nl++; uv++; pos++;	// Bottom Left Of The Texture and Quad
		nl->set(0,0,1); uv->set(1.f, 0.f); pos->set(1.0f, -1.0f, 1.0f);		nl++; uv++; pos++;	// Bottom Right Of The Texture and Quad
		nl->set(0,0,1); uv->set(1.f, 1.f); pos->set(1.0f, 1.0f, 1.0f);		nl++; uv++; pos++;	// Top Right Of The Texture and Quad
		nl->set(0,0,1); uv->set(0.f, 1.f); pos->set(-1.0f, 1.0f, 1.0f);		nl++; uv++; pos++;	// Top Left Of The Texture and Quad
		// Back Face
		nl->set(0,0,-1); uv->set(1.f, 0.f); pos->set(-1.0f, -1.0f, -1.0f);	nl++; uv++; pos++;	// Bottom Right Of The Texture and Quad
		nl->set(0,0,-1); uv->set(1.f, 1.f); pos->set(-1.0f, 1.0f, -1.0f);	nl++; uv++; pos++;	// Top Right Of The Texture and Quad
		nl->set(0,0,-1); uv->set(0.f, 1.f); pos->set(1.0f, 1.0f, -1.0f);	nl++; uv++; pos++;	// Top Left Of The Texture and Quad
		nl->set(0,0,-1); uv->set(0.f, 0.f); pos->set(1.0f, -1.0f, -1.0f);	nl++; uv++; pos++;	// Bottom Left Of The Texture and Quad
		// Right face
		nl->set(1,0,0); uv->set(1.f, 0.f); pos->set(1.0f, -1.0f, -1.0f);	nl++; uv++; pos++;	// Bottom Right Of The Texture and Quad
		nl->set(1,0,0); uv->set(1.f, 1.f); pos->set(1.0f, 1.0f, -1.0f);		nl++; uv++; pos++;	// Top Right Of The Texture and Quad
		nl->set(1,0,0); uv->set(0.f, 1.f); pos->set(1.0f, 1.0f, 1.0f);		nl++; uv++; pos++;	// Top Left Of The Texture and Quad
		nl->set(1,0,0); uv->set(0.f, 0.f); pos->set(1.0f, -1.0f, 1.0f);		nl++; uv++; pos++;	// Bottom Left Of The Texture and Quad
		// Left Face
		nl->set(-1,0,0); uv->set(0.f, 0.f); pos->set(-1.0f, -1.0f, -1.0f);	nl++; uv++; pos++;	// Bottom Left Of The Texture and Quad
		nl->set(-1,0,0); uv->set(1.f, 0.f); pos->set(-1.0f, -1.0f, 1.0f);	nl++; uv++; pos++;	// Bottom Right Of The Texture and Quad
		nl->set(-1,0,0); uv->set(1.f, 1.f); pos->set(-1.0f, 1.0f, 1.0f);	nl++; uv++; pos++;	// Top Right Of The Texture and Quad
		nl->set(-1,0,0); uv->set(0.f, 1.f); pos->set(-1.0f, 1.0f, -1.0f);	nl++; uv++; pos++;	// Top Left Of The Texture and Quad

		SGE_ASSERT(pos == _boxMesh.pos.end());
		SGE_ASSERT(uv  == _boxMesh.uv[0].end());
		SGE_ASSERT(nl  == _boxMesh.normal.end());
	}

	{
		_topMesh.pos.resize(1 * 4);
		_topMesh.uv[0].resize(_topMesh.pos.size());
		_topMesh.normal.resize(_topMesh.pos.size());

		auto* pos = _topMesh.pos.begin();
		auto* uv = _topMesh.uv[0].begin();
		auto* nl = _topMesh.normal.begin();

		// Top Face
		nl->set(0,1,0); uv->set(0.0f, 1.0f); pos->set(-1.0f, 1.0f, -1.0f);	nl++; uv++; pos++;	// Top Left Of The Texture and Quad
		nl->set(0,1,0); uv->set(0.0f, 0.0f); pos->set(-1.0f, 1.0f, 1.0f);	nl++; uv++; pos++;	// Bottom Left Of The Texture and Quad
		nl->set(0,1,0); uv->set(1.0f, 0.0f); pos->set(1.0f, 1.0f, 1.0f);	nl++; uv++; pos++;	// Bottom Right Of The Texture and Quad
		nl->set(0,1,0); uv->set(1.0f, 1.0f); pos->set(1.0f, 1.0f, -1.0f);	nl++; uv++; pos++;	// Top Right Of The Texture and Quad

		SGE_ASSERT(pos == _topMesh.pos.end());
		SGE_ASSERT(uv  == _topMesh.uv[0].end());
		SGE_ASSERT(nl  == _topMesh.normal.end());
	}

	_nBoxDisplayList = glGenLists(2);
	_nTopDisplayList = _nBoxDisplayList + 1;

	{
		// You can put just about any command you want between glNewList() and glEndList().
		// You can set colors, you can change textures, etc.
		// The only type of code you CAN'T add is code that would change the display list on the fly.
		// Once the display list is built, you CAN'T change it. because the list is only CREATED once
		// If you want to change the color of the display list, you have to change it BEFORE you draw the display list to the screen
		// Anything between glNewList() and glEndList is part of the Display List,
		// anything before glNewList() or after glEndList() is not part of the current display list
		OGL::Scoped_glNewList scoped(_nBoxDisplayList, GL_COMPILE);
		_bindMeshByQuads(_boxMesh);
	}

	{
		OGL::Scoped_glNewList scoped(_nTopDisplayList, GL_COMPILE);
		_bindMeshByQuads(_topMesh);
	}
}

void NeHeOGL_Lesson012::_bindMeshByQuads(EditMesh& mesh) {
	OGL::Scoped_glBegin scoped(NeHeOGL_BeginMode::Quads);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
		OGL::vertexPointer(mesh.pos.data(), 0);
		OGL::texCoordPointer(mesh.uv[0].data(), 0);
		OGL::normalPointer(mesh.normal.data(), 0);
		
		size_t vertexCount = mesh.pos.size();
		size_t indicesCount = vertexCount / 4;

		for (int i = 0, j = 0; i < indicesCount; i++, j+=4) {
			glArrayElement(  j);
			glArrayElement(1+j);
			glArrayElement(2+j);
			glArrayElement(3+j);
		}
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	OGLUtil::throwIfError();
}

void NeHeOGL_Lesson012::onDraw() {
	setFrameBufferSize(_clientRect.size);

	OGL::drawGridAndOriginAxis();

	glTranslatef(0, 6, 0);
	glEnable(GL_DEPTH_TEST);
	_example1();
	glDisable(GL_DEPTH_TEST);

	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson012::_example1() {
	_tex.bind();
	for (int y = 1; y < 6; ++y)
		for (int x = 0; x < y; ++x) {
			OGL::Scoped_glPushMatrix m;
			glTranslatef(1.4f + (float(x) * 2.8f) - (float(y) * 1.4f), ((6.0f - float(y)) * 2.4f) - 7.0f, -5.0f);
			glRotatef(45.0f, 1,0,0);
			glRotatef(45.0f, 0,1,0);

			OGL::color4b(kBoxColors[y-1]); glCallList(_nBoxDisplayList);
			OGL::color4b(kTopColors[y-1]); glCallList(_nTopDisplayList);
		}
	_tex.unbind();
}

}