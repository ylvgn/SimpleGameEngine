#include "NeheOGL_Lesson007.h"

namespace sge {

void NeheOGL_Lesson007::onCreate(CreateDesc& desc) {
	desc.ownDC = true;
	Base::onCreate(desc);

	const char* filename = "Crate.bmp";

	_imageToUpload.loadFile(filename);
	if (!_imageToUpload.width || !_imageToUpload.height)
		throw SGE_ERROR("load bmp filename = {}", filename);

	glGenTextures(kTexture2dCount, &_texture2ds[0]);

	{ // Create Nearest Filtered Texture (no smoothing)
		glBindTexture(GL_TEXTURE_2D, _texture2ds[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // The MAG_FILTER is the filter used when an image is drawn bigger than the original texture size
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // The MIN_FILTER is the filter used when an image is drawn smaller than the original texture size
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 _imageToUpload.width,
					 _imageToUpload.height,
					 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					 _imageToUpload.pixelData.data()
		);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	{ // Create Linear Filtered Texture
		glBindTexture(GL_TEXTURE_2D, _texture2ds[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 _imageToUpload.width,
					 _imageToUpload.height,
					 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					 _imageToUpload.pixelData.data()
		);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	{ // Create MipMapped Texture
		// Mipmapping!You may have noticed that when you make an image very tiny on the screen, alot of the fine details disappear.
		// Patterns that used to look nice start looking real bad.
		// When you tell OpenGL to build a mipmapped texture OpenGL tries to build different sized high quality textures.
		// When you draw a mipmapped texture to the screen OpenGL will select the BEST looking texture from the ones it built(texture with the most detail)
		// and draw it to the screen instead of resizing the original image (which causes detail loss).
		glBindTexture(GL_TEXTURE_2D, _texture2ds[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		gluBuild2DMipmaps(GL_TEXTURE_2D,
						  3,
						  _imageToUpload.width,
						  _imageToUpload.height,
						  GL_RGBA,
						  GL_UNSIGNED_BYTE,
						  _imageToUpload.pixelData.data()
		);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Setup Lighting
	glLightfv(GL_LIGHT1, GL_AMBIENT,  _lightAmbient.data);	// Setup The Ambient Light
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  _lightDiffuse.data);	// Setup The Diffuse Light
	glLightfv(GL_LIGHT1, GL_POSITION, _lightPosition.data);	// Position The Light
	glEnable(GL_LIGHT1);									// Enable GL_LIGHT1, when glEnable(GL_LIGHTING), the light will actually work!
}

void NeheOGL_Lesson007::onDraw() {
	_example1();
}

void NeheOGL_Lesson007::onUIMouseEvent(UIMouseEvent& ev) {
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
				float d = 0.15f;
				auto delta = ev.deltaPos * d;
				_lightPosition.x += delta.x;
				_lightPosition.y -= delta.y;
			} break;
		}
	}

	if (ev.isScroll()) {
		auto d = ev.scroll * 0.015f;
		_camerMovePosZ += d.y;
	}
}

void NeheOGL_Lesson007::onUIKeyboardEvent(UIKeyboardEvent& ev) {
	using KeyCode = UIKeyboardEvent::KeyCode;

	if (ev.isDown(KeyCode::LeftArrow)) {
		_camerOrbitSpeed.y -= 0.01f; // Decrease _camerOrbitSpeed yspeed
	}
	if (ev.isDown(KeyCode::RightArrow)) {
		_camerOrbitSpeed.y += 0.01f; // Increase _camerOrbitSpeed yspeed
	}

	if (ev.isDown(KeyCode::UpArrow)) {
		_camerOrbitSpeed.x -= 0.01f; // Decrease _camerOrbitSpeed xspeed
	}
	if (ev.isDown(KeyCode::DownArrow)) {
		_camerOrbitSpeed.x += 0.01f; // Increase _camerOrbitSpeed xspeed
	}

	if (ev.isDown(KeyCode::PageUp)) {
		_camerMovePosZ -= 0.02f;	// Crate Move Into The Screen
	}
	if (ev.isDown(KeyCode::PageDown)) {
		_camerMovePosZ += 0.02f;	// Crate Move Towards The Viewer
	}

	if (!_isPressedF && ev.isDown(KeyCode::F)) {
		_isPressedF = true;
		SGE_DUMP_VAR(_texSelectedIndex);
		_texSelectedIndex = (_texSelectedIndex + 1) % kTexture2dCount;
	}
	if (ev.isUp(KeyCode::F)) {
		_isPressedF = false;
	}

	// If _isPressedL was false, meaning the 'L' key hasn't been pressed yet, or it's been released, _isPressedL becomes true.
	// This forces the person to let go of the 'L' key before this code will run again.
	// If we didn't check to see if the key was being held down, the lighting would flicker off and on over and over,
	// because the program would think you were pressing the 'L' key over and over again each time it came to this section of code
	if (!_isPressedL && ev.isDown(KeyCode::L)) {
		_isPressedL = true;
		_isOnLight = !_isOnLight;
		SGE_DUMP_VAR(static_cast<int>(_isOnLight));
	}
	if (ev.isUp(KeyCode::L)) {
		_isPressedL = false;
	}
}

void NeheOGL_Lesson007::_example1() {
	float width  = _clientRect.w;
	float height = _clientRect.h;

	if (height == 0) {
		height = 1;
	}

	float aspect = width / height;
	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D); // Enable Texture Mapping
	glShadeModel(GL_SMOOTH); // Enable Smooth Shading

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective Calculations

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.f, aspect, 0.01f, 1000.0f);

	// setup camera
	glTranslatef(0, 0, _camerMovePosZ); // (away from and towards the viewer)
	glRotatef(_camerOrbitAngle.x, 1,0,0);
	glRotatef(_camerOrbitAngle.y, 0,1,0);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	// draw grid
	OGL::glColor(OGL::kWhite);
	glLineWidth(2);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		for (float x = -10; x <= 10; ++x) {
			glVertex3f(x, 0, -10);
			glVertex3f(x, 0, 10);
		}
		for (float z = -10; z <= 10; ++z) {
			glVertex3f(-10, 0, z);
			glVertex3f(10, 0, z);
		}
	glEnd();

	// setup lighting
	glLightfv(GL_LIGHT1, GL_POSITION, _lightPosition.data);	// Position The Light

	if (_isOnLight)
		glEnable(GL_LIGHTING);  // Enable Lighting
	else
		glDisable(GL_LIGHTING); // Disable Lighting

	glTranslatef(0, a, 0);
	glBindTexture(GL_TEXTURE_2D, _texture2ds[_texSelectedIndex]); // Select A Texture Based On filter
	glBegin(GL_QUADS);
		// A normal is a line pointing straight out of the middle of a polygon at a 90 degree angle. When you use lighting, you need to specify a normal
		// The normal tells OpenGL which direction the polygon is facing... which way is up
		// The normal should point outwards from the polygon
		// The light is close to the viewer, any time the normal is pointing towards the viewer it's also pointing towards the light.
		// When it does, the face will light up.
		// The more a normal points towards the light, the brighter that face is
		// If you move into the center of the cube you'll notice it's dark.
		// The normals are point out, not in, so there's no light inside the box, exactly as it should be

		// Front Face
		OGL::glNormal(Vec3f::s_forward());						// glNormal3f( 0.0f,  0.0f,  1.0f);
		glTexCoord2f(0.0f, 0.0f); OGL::glVertex(kCubePos[7]);	// glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); OGL::glVertex(kCubePos[6]);	// glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); OGL::glVertex(kCubePos[2]);	// glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); OGL::glVertex(kCubePos[3]);	// glVertex3f(-1.0f,  1.0f,  1.0f);
		// Back Face
		OGL::glNormal(Vec3f::s_back());							// glNormal3f( 0.0f,  0.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); OGL::glVertex(kCubePos[0]);	// glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); OGL::glVertex(kCubePos[1]);	// glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); OGL::glVertex(kCubePos[5]);	// glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); OGL::glVertex(kCubePos[4]);	// glVertex3f(-1.0f, -1.0f, -1.0f);
		// Top Face
		OGL::glNormal(Vec3f::s_up());							// glNormal3f( 0.0f,  1.0f,  0.0f);
		glTexCoord2f(0.0f, 0.0f); OGL::glVertex(kCubePos[3]);	// glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); OGL::glVertex(kCubePos[2]);	// glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); OGL::glVertex(kCubePos[1]);	// glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); OGL::glVertex(kCubePos[0]);	// glVertex3f(-1.0f,  1.0f, -1.0f);
		// Bottom Face
		OGL::glNormal(Vec3f::s_down());							// glNormal3f( 0.0f, -1.0f,  0.0f);
		glTexCoord2f(1.0f, 1.0f); OGL::glVertex(kCubePos[4]);	// glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); OGL::glVertex(kCubePos[5]);	// glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); OGL::glVertex(kCubePos[6]);	// glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); OGL::glVertex(kCubePos[7]);	// glVertex3f(-1.0f, -1.0f,  1.0f);
		// Right face
		OGL::glNormal(Vec3f::s_right());						// glNormal3f( 1.0f,  0.0f,  0.0f);
		glTexCoord2f(0.0f, 0.0f); OGL::glVertex(kCubePos[6]);	// glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); OGL::glVertex(kCubePos[5]);	// glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); OGL::glVertex(kCubePos[1]);	// glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); OGL::glVertex(kCubePos[2]);	// glVertex3f( 1.0f,  1.0f,  1.0f);
		// Left Face
		OGL::glNormal(Vec3f::s_left());							// glNormal3f(-1.0f,  0.0f,  0.0f);
		glTexCoord2f(0.0f, 0.0f); OGL::glVertex(kCubePos[4]);	// glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); OGL::glVertex(kCubePos[7]);	// glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); OGL::glVertex(kCubePos[3]);	// glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); OGL::glVertex(kCubePos[0]);	// glVertex3f(-1.0f,  1.0f, -1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	_camerOrbitAngle += _camerOrbitSpeed;

	swapBuffers();
	drawNeeded();
}


}