#include "NeHeOGL_Lesson007.h"

namespace sge {

void NeHeOGL_Lesson007::onUIMouseEvent(UIMouseEvent& ev) {
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
		}
	}

	if (ev.isScroll()) {
		auto d = ev.scroll * 0.005f;
		_camerMovePosZ -= d.y;
	}
}

void NeHeOGL_Lesson007::onUIKeyboardEvent(UIKeyboardEvent& ev) {
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
		_camerMovePosZ += 0.02f;	// Crate Move Into The Screen
	}
	if (ev.isDown(KeyCode::PageDown)) {
		_camerMovePosZ -= 0.02f;	// Crate Move Towards The Viewer
	}

	if (!_isPressedF && ev.isDown(KeyCode::F)) {
		_isPressedF = true;
		_texSelectedIndex = (_texSelectedIndex + 1) % kTexture2dCount;
		SGE_LOG("current _texSelectedIndex = {}", _texSelectedIndex);
	}
	if (ev.isUp(KeyCode::F)) _isPressedF = false;

	if (!_isPressedT && ev.isDown(KeyCode::T)) {
		_isPressedT = true;
		_isFloorOnMipMap = !_isFloorOnMipMap;
		SGE_DUMP_VAR(static_cast<int>(_isFloorOnMipMap));
	}
	if (ev.isUp(KeyCode::T)) _isPressedT = false;

	if (!_isPressedM && ev.isDown(KeyCode::M)) {
		_isPressedM = true;
		_sphereMode = static_cast<SphereMode>(((enumInt(_sphereMode) + 1) % 3));
		SGE_DUMP_VAR(_sphereMode);
	}
	if (ev.isUp(KeyCode::M)) _isPressedM = false;

	if (!_isPressedO && ev.isDown(KeyCode::O)) {
		_isDrawOneSphere = !_isDrawOneSphere;
		SGE_DUMP_VAR(static_cast<int>(_isDrawOneSphere));
	}
	if (ev.isUp(KeyCode::O)) _isPressedO = false;

	if (!_isPressedN && ev.isDown(KeyCode::N)) {
		_isShowSphereNormals = !_isShowSphereNormals;
		SGE_DUMP_VAR(static_cast<int>(_isShowSphereNormals));
	}
	if (ev.isUp(KeyCode::S)) _isPressedN = false;

	if (!_isPressedW && ev.isDown(KeyCode::W)) {
		_isWireFrame = !_isWireFrame;
		SGE_DUMP_VAR(static_cast<int>(_isWireFrame));
	}
	if (ev.isUp(KeyCode::W)) _isPressedW = false;

	if (!_isPressedA && ev.isDown(KeyCode::A)) {
		_isOnAmbient = !_isOnAmbient;
		SGE_DUMP_VAR(static_cast<int>(_isOnAmbient));
	}
	if (ev.isUp(KeyCode::A)) _isPressedA = false;

	if (!_isPressedD && ev.isDown(KeyCode::D)) {
		_isOnDiffuse = !_isOnDiffuse;
		SGE_DUMP_VAR(static_cast<int>(_isOnDiffuse));
	}
	if (ev.isUp(KeyCode::D)) _isPressedD = false;

	if (!_isPressedS && ev.isDown(KeyCode::S)) {
		_isOnSpecular = !_isOnSpecular;
		SGE_DUMP_VAR(static_cast<int>(_isOnSpecular));
	}
	if (ev.isUp(KeyCode::S)) _isPressedS = false;
}

void NeHeOGL_Lesson007::onInitedGL() {
	_gridMesh.createGrid(100);
	_gridMesh.renderState.wireframe = true;

	_coordinateMesh.createCoordinate();

	{
		Texture2D::CreateDesc desc;
		desc.imageToUpload.loadFile("Crate.bmp");
	
	// Create Nearest Filtered Texture (no smoothing)
		desc.samplerState.filter = TextureFilter::Nearest;
		desc.samplerState.maxLOD = 0;
		desc.samplerState.minLOD = 0;
		_texture2dArray[0].create(desc);

	// Create Linear Filtered Texture
		desc.samplerState.filter = TextureFilter::Linear;
		desc.samplerState.minLOD = 0;
		desc.samplerState.maxLOD = 0;
		_texture2dArray[1].create(desc);

	// Create MipMapped Texture
		desc.samplerState.filter = TextureFilter::Linear;
		desc.samplerState.minLOD = 0;
		desc.samplerState.maxLOD = 3;
		// Mipmapping!You may have noticed that when you make an image very tiny on the screen, alot of the fine details disappear.
			// Patterns that used to look nice start looking real bad.
			// When you tell OpenGL to build a mipmapped texture OpenGL tries to build different sized high quality textures.
			// When you draw a mipmapped texture to the screen OpenGL will select the BEST looking texture from the ones it built(texture with the most detail)
			// and draw it to the screen instead of resizing the original image (which causes detail loss).
		_texture2dArray[2].create(desc);
	}

	{
		_floorMesh.createGrid(1000);

		Texture2D::CreateDesc desc;
		desc.samplerState.filter = TextureFilter::Linear;
		desc.imageToUpload.loadFile("grid.png");

		desc.samplerState.minLOD = 0;
		desc.samplerState.maxLOD = 4; // why can not set 5 ???
		_floorMainTex.create(desc);

		desc.samplerState.minLOD = 0;
		desc.samplerState.maxLOD = 0;
		_floorMainTexNoMipMap.create(desc);
	}

	{
		_sphereFlatMesh.loadObjFile("sphere_flat.obj");
		_sphereSmoothMesh.loadObjFile("sphere_smooth.obj");
		_sphereHighMesh.loadObjFile("sphere_high.obj");
	}
}

void NeHeOGL_Lesson007::onDraw() {
//	_example1(); // nehe lighting
//	_example2(); // mipmap
//	_example3(LightType::Directional);
//	_example3(LightType::Point);
	_example3(LightType::Spot);
}

void NeHeOGL_Lesson007::_example1() {
	float width  = _clientRect.w;
	float height = _clientRect.h;

	if (height == 0) {
		height = 1;
	}

	float aspect = width / height;
	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	glDisable(GL_LIGHTING);

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

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	// setup camera
	glTranslatef(0, 0, -_camerMovePosZ); // (away from and towards the viewer)
	glRotatef(_camerOrbitAngle.x, 1, 0, 0);
	glRotatef(_camerOrbitAngle.y, 0, 1, 0);

	OGL::drawGridAndCoordinate(_gridMesh, _coordinateMesh);

	// setup lighting
	_lightPos.set(0.0f, 0.0f, 2.0f, 1.0f);
	glLightfv(GL_LIGHT0, GL_POSITION, _lightPos.data);		// Position The Light
	glLightfv(GL_LIGHT0, GL_AMBIENT, _lightAmbient.data);	// Setup The Ambient Light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, _lightDiffuse.data);	// Setup The Diffuse Light
	glLightfv(GL_LIGHT0, GL_SPECULAR, _lightSpecular.data);	// Setup The Specular Light

	OGL::Scoped_glEnable light0(GL_LIGHT0); // Enable GL_LIGHT0, when glEnable(GL_LIGHTING), the light will actually work!
	OGL::Scoped_glEnable lighting(GL_LIGHTING);

	glTranslatef(0, 0.5f, 0);

	_texture2dArray[_texSelectedIndex].bind(); // Select A Texture Based On filter
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
		OGL::normal3f(Vec3f::s_forward());						// glNormal3f( 0.0f,  0.0f,  1.0f);
		glTexCoord2f(0.0f, 0.0f); OGL::vertex3f(kCubePos[7]);	// glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); OGL::vertex3f(kCubePos[6]);	// glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); OGL::vertex3f(kCubePos[2]);	// glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); OGL::vertex3f(kCubePos[3]);	// glVertex3f(-1.0f,  1.0f,  1.0f);
		// Back Face
		OGL::normal3f(Vec3f::s_back());							// glNormal3f( 0.0f,  0.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); OGL::vertex3f(kCubePos[0]);	// glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); OGL::vertex3f(kCubePos[1]);	// glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); OGL::vertex3f(kCubePos[5]);	// glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); OGL::vertex3f(kCubePos[4]);	// glVertex3f(-1.0f, -1.0f, -1.0f);
		// Top Face
		OGL::normal3f(Vec3f::s_up());							// glNormal3f( 0.0f,  1.0f,  0.0f);
		glTexCoord2f(0.0f, 0.0f); OGL::vertex3f(kCubePos[3]);	// glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); OGL::vertex3f(kCubePos[2]);	// glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); OGL::vertex3f(kCubePos[1]);	// glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); OGL::vertex3f(kCubePos[0]);	// glVertex3f(-1.0f,  1.0f, -1.0f);
		// Bottom Face
		OGL::normal3f(Vec3f::s_down());							// glNormal3f( 0.0f, -1.0f,  0.0f);
		glTexCoord2f(1.0f, 1.0f); OGL::vertex3f(kCubePos[4]);	// glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); OGL::vertex3f(kCubePos[5]);	// glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); OGL::vertex3f(kCubePos[6]);	// glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); OGL::vertex3f(kCubePos[7]);	// glVertex3f(-1.0f, -1.0f,  1.0f);
		// Right face
		OGL::normal3f(Vec3f::s_right());						// glNormal3f( 1.0f,  0.0f,  0.0f);
		glTexCoord2f(0.0f, 0.0f); OGL::vertex3f(kCubePos[6]);	// glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); OGL::vertex3f(kCubePos[5]);	// glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); OGL::vertex3f(kCubePos[1]);	// glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); OGL::vertex3f(kCubePos[2]);	// glVertex3f( 1.0f,  1.0f,  1.0f);
		// Left Face
		OGL::normal3f(Vec3f::s_left());							// glNormal3f(-1.0f,  0.0f,  0.0f);
		glTexCoord2f(0.0f, 0.0f); OGL::vertex3f(kCubePos[4]);	// glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); OGL::vertex3f(kCubePos[7]);	// glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); OGL::vertex3f(kCubePos[3]);	// glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); OGL::vertex3f(kCubePos[0]);	// glVertex3f(-1.0f,  1.0f, -1.0f);
	glEnd();
	_texture2dArray[_texSelectedIndex].unbind();

	_camerOrbitAngle += _camerOrbitSpeed;

	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson007::_example2() {
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

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	// setup camera
	glTranslatef(0, 0, -_camerMovePosZ);
	glRotatef(_camerOrbitAngle.x, 1, 0, 0);
	glRotatef(_camerOrbitAngle.y, 0, 1, 0);

	{
		OGL::Scoped_glPushMatrix scoped;

		if (_isFloorOnMipMap) _floorMainTex.bind();
		else _floorMainTexNoMipMap.bind();

		_floorMesh.draw();

		if (_isFloorOnMipMap) _floorMainTex.unbind();
		else _floorMainTexNoMipMap.unbind();
	}

	_camerOrbitAngle += _camerOrbitSpeed;

	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson007::_example3(LightType lightType) {
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

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.f, aspect, 0.01f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	// setup camera
	glTranslatef(0, 0, -_camerMovePosZ);
	glRotatef(_camerOrbitAngle.x, 1,0,0);
	glRotatef(_camerOrbitAngle.y, 0,1,0);

	OGL::drawGridAndCoordinate(_gridMesh, _coordinateMesh);

	_globalAmbient.set(0.2f, 0.2f, 0.2f, 1);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, _globalAmbient.data);

	switch (lightType) {
		case LightType::Directional:	_exampleDirectionalLight(uptime);	break;
		case LightType::Point:			_examplePointLight(uptime);			break;
		case LightType::Spot:			_exampleSpotLight(uptime);			break;
		default:						throw SGE_ERROR("unsupported LightType");
	}

	_drawLightPointGizmos();

	{
		OGL::Scoped_glPushMatrix scoped;
		OGL::translatef(Vec3f::s_up()); // offset
		_drawSpheres(_sphereMode);
		_drawSphereNormals(_sphereMode);
	}

	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson007::_exampleDirectionalLight(float uptime) {
#if 0
	_lightPos.set(0, 3, 0, 0); // xyz: light dir, w = 0
#else
	const float kRadius = 3.f;
	_lightPos.set(Math::cos(uptime) * kRadius, Math::sin(uptime) * kRadius, 0, 0);
#endif
	_lightAmbient.set(0, 0, 0, 1);
	_lightDiffuse.set(1, 1, 1, 1);
	_lightSpecular.set(1, 1, 1, 1);

	glLightfv(GL_LIGHT0, GL_POSITION,	_lightPos.data);
	glLightfv(GL_LIGHT0, GL_AMBIENT,	_lightAmbient.data);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,	_lightDiffuse.data);
	glLightfv(GL_LIGHT0, GL_SPECULAR,	_lightSpecular.data);
}

void NeHeOGL_Lesson007::_examplePointLight(float uptime) {
#if 0
	_lightPos.set(0, 3, 0, 1); // xyz: light pos, w = 1
#else
	const float kRadius = 3.f;
	_lightPos.set(0, Math::sin(uptime) * kRadius, 0, 1);
#endif
	_lightAmbient.set(0, 0, 0, 1);
	_lightDiffuse.set(1, 1, 1, 1);
	_lightSpecular.set(1, 1, 1, 1);

	glLightfv(GL_LIGHT0, GL_POSITION,	_lightPos.data);
	glLightfv(GL_LIGHT0, GL_AMBIENT,	_lightAmbient.data);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,	_lightDiffuse.data);
	glLightfv(GL_LIGHT0, GL_SPECULAR,	_lightSpecular.data);
}

void NeHeOGL_Lesson007::_exampleSpotLight(float uptime) {
#if 1
	OGL::Scoped_glPushMatrix scoped;
	glRotatef(uptime * 180, 0, 1, 0);
#endif
	_lightPos.set(0, 3, 0, 1); // xyz: light pos, w = 1
	_lightAmbient.set(0, 0, 0, 1);
	_lightDiffuse.set(1, 1, 1, 1);
	_lightSpecular.set(1, 1, 1, 1);

	glLightfv(GL_LIGHT0, GL_POSITION,	_lightPos.data);
	glLightfv(GL_LIGHT0, GL_AMBIENT,	_lightAmbient.data);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,	_lightDiffuse.data);
	glLightfv(GL_LIGHT0, GL_SPECULAR,	_lightSpecular.data);

	_spotDir.set(0,0,-1);
	_spotExponent		= 5.f;
	_spotCutoffAngle	= 45;

	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION,		_spotDir.data);
	glLightfv(GL_LIGHT0, GL_SPOT_EXPONENT,		&_spotExponent);
	glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF,		&_spotCutoffAngle);
}

void NeHeOGL_Lesson007::_drawSpheres(SphereMode mode) {
	using SRC = SphereMode;

	OGL::Scoped_glEnable lighting(GL_LIGHTING);
	OGL::Scoped_glEnable light0(GL_LIGHT0);

	switch (mode) {
		case SRC::Flat:		_drawSpheres(_sphereFlatMesh); break;
		case SRC::Smooth:	_drawSpheres(_sphereSmoothMesh); break;
		case SRC::High:		_drawSpheres(_sphereHighMesh); break;
		default:			throw SGE_ERROR("unsupported SphereMode");
	}
}

void NeHeOGL_Lesson007::_drawSpheres(Mesh& mesh) {
	_emission.set(0, 0, 0, 0);
	_shininess = 64;

	if (_isOnAmbient) _ambient.set(0, 0, 1, 1);
	else _ambient.set(0, 0, 0, 1);
		
	if (_isOnDiffuse) _diffuse.set(0, 1, 0, 1);
	else _diffuse.set(0, 0, 0, 1);

	if (_isOnSpecular) _specular.set(1, 0, 0, 1);
	else _specular.set(0, 0, 0, 1);

	// Result_ambient  = Ligting_ambient  * Matial_ambient
	// Result_diffuse  = Ligting_diffuse  * Matial_diffuse
	// Result_specular = Ligting_specular * Matial_specular
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,		_ambient.data);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,		_diffuse.data);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,	_specular.data);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,	&_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,	_emission.data);

	if (_isDrawOneSphere) {
		OGL::Scoped_glPushMatrix scoped;
		mesh.renderState.wireframe = _isWireFrame;
		mesh.draw();
		return;
	}

	static const int nx	= 12;
	static const int nz	= 12;
	float d = 3.f;

	{
		OGL::Scoped_glPushMatrix scoped;
		glTranslatef(-nx * 0.5f * d, 0, -nz * 0.5f * d);

		for (int z = 0; z < nz; ++z) {
			for (int x = 0; x < nx; ++x) {
				OGL::Scoped_glPushMatrix scoped2;
				glTranslatef(x * d, 0, z * d);
				mesh.renderState.wireframe = _isWireFrame;
				mesh.draw();
			}
		}

		for (int z = 0; z < nz; ++z) {
			for (int x = 0; x < nx; ++x) {
				OGL::Scoped_glPushMatrix scoped2;
				glTranslatef(x * d, 0, z * d);
			}
		}
	}
}

void NeHeOGL_Lesson007::_drawSphereNormals(SphereMode mode) {
	if (!_isShowSphereNormals)
		return;

	using SRC = SphereMode;
	switch (mode) {
		case SRC::Flat:		_drawSphereNormals(_sphereFlatMesh); break;
		case SRC::Smooth:	_drawSphereNormals(_sphereSmoothMesh); break;
		case SRC::High:		_drawSphereNormals(_sphereHighMesh); break;
		default:			throw SGE_ERROR("unsupported SphereMode");
	}
}

void NeHeOGL_Lesson007::_drawSphereNormals(const Mesh& mesh) {
	if (!_isShowSphereNormals)
		return;

	OGL::createDisplayNormals(_sphereNormals, mesh);

	{
		OGL::Scoped_glColor4b scopedColor(OGL::kbYellow);
		OGL::Scoped_glEnableClientState scopedClientState(GL_VERTEX_ARRAY);
		size_t stride = sizeof(_sphereNormals[0]);
		OGL::vertexPointer(&_sphereNormals[0], stride);
		glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(_sphereNormals.size()));
	}
}

void NeHeOGL_Lesson007::_drawLightPointGizmos() {
	glPointSize(12);
	OGL::Scoped_glBegin begin(NeHeOGL_BeginMode::Points);
	OGL::Scoped_glColor4f color(OGL::kYellow);
	glVertex3fv(_lightPos.data);
}

}