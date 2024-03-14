#include "NeheOGL_Lesson006.h"

namespace sge {

void NeheOGL_Lesson006::onCreate(CreateDesc& desc) {
	float d = 1.0f;
//------------------------------------------
//		  0--------1
//		 /|       /|
//		3--------2 |
//      | |      | |
//		| 4------|-5
//      |/       |/
//      7--------6
	static const int kCubeVertexCount = 8;
	static Vector<Tuple3f, kCubeVertexCount> kCubePos {
		{-d, d,-d}, // 0
		{ d, d,-d}, // 1
		{ d, d, d}, // 2
		{-d, d, d}, // 3
		{-d,-d,-d}, // 4
		{ d,-d,-d}, // 5
		{ d,-d, d}, // 6
		{-d,-d, d}, // 7
	};
	_mesh.clean();
	_mesh.pos.reserve(kCubeVertexCount);
	_mesh.pos.appendRange(kCubePos);

	static const int kCubeIndiceCount = 24;
	static Vector<u32, kCubeIndiceCount> kCubeIndices {
		2,3,7,6, // Front
		5,4,0,1, // Back
		1,0,3,2, // Top
		6,7,4,5, // Bottom
		1,2,6,5, // Right
		3,0,4,7, // Left
	};

	_mesh.indices.reserve(kCubeIndiceCount);
	_mesh.indices.appendRange(kCubeIndices);

	_mesh.uv = {
		{1,0}, {0,0}, {0,1}, {1,1}, // Front
		{0,1}, {1,1}, {1,0}, {0,0}, // Back
		{0,0}, {1,0}, {1,1}, {0,1}, // Top
		{0,0}, {1,0}, {1,1}, {0,1}, // Bottom
		{0,0}, {1,0}, {1,1}, {0,1}, // Right
		{0,0}, {1,0}, {1,1}, {0,1}, // Left
	};

	Base::onCreate(desc);

	{ // create a texture
		glGenTextures(1, &_texture2d);
		_loadTexture2D("NeHe.bmp", _imageToUpload, _texture2d);
	}

	{ // create more than one texture
		glGenTextures(2, _texture2ds);
		_loadTexture2D("NeHe.bmp", _imagesToUpload[0], _texture2ds[0]);
		_loadTexture2D("uvChecker_BMP.bmp", _imagesToUpload[1], _texture2ds[1]);
	}
}

void NeheOGL_Lesson006::onDraw() {
	float uptime = static_cast<float>(_uptime.get() * 90.f);
	//_example1(uptime);
	//_example2(uptime);
	_example3(uptime);
}

#if SGE_OS_WINDOWS
void NeheOGL_Lesson006::_loadByHBITMAP(MyImage& o, StrView filename) {
	// Loads A Bitmap Image

	// a few VERY important things you need to know about the images you plan to use as textures
		// The image height and width MUST be a power of 2
		// The width and height must be at least 64 pixels,
		// and for compatability reasons, shouldn't be more than 256 pixels
	// If the image you want to use is not 64, 128 or 256 pixels on the width or height, resize it in an art program(like photoshop)
	if (filename.empty() || !File::exists(filename))
		throw SGE_ERROR("_loadByHBITMAP invalid filename = {}", filename);

	TempStringW s = UtfUtil::toStringW(filename);
	HBITMAP hBmp = static_cast<HBITMAP>(LoadImage(GetModuleHandle(nullptr), s.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE));

	if (!hBmp)
		throw SGE_ERROR("LoadImage");

	auto& width  = o.width;
	auto& height = o.height;
	auto& pixels = o.pixelData;

	HDC hdc = ::CreateCompatibleDC(NULL);
		BITMAP bmpMap;
		g_bzero(bmpMap);
		HBITMAP oldBitmap = SelectBitmap(hdc, hBmp);
		GetObject(hBmp, sizeof(bmpMap), &bmpMap);

		BITMAPINFO info;
		g_bzero(info);
		info.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
		info.bmiHeader.biWidth		= width = bmpMap.bmWidth;
		info.bmiHeader.biHeight		= height = bmpMap.bmHeight;
		info.bmiHeader.biPlanes		= 1;
		info.bmiHeader.biBitCount	= bmpMap.bmBitsPixel; // bpp
		info.bmiHeader.biCompression= BI_RGB;
		info.bmiHeader.biSizeImage	= ((width * bmpMap.bmBitsPixel + 31) / 32) * 4 * height;

		pixels.resize(info.bmiHeader.biSizeImage);

		// API GetDIBits retrieves the bits of the specified compatible bitmap
		// and copies them into a buffer as a DIB using the specified format
		if (!::GetDIBits(hdc, hBmp, 0, height, pixels.data(), &info, DIB_RGB_COLORS)) {
			throw SGE_ERROR("GetDIBits");
		}
		::SelectObject(hdc, oldBitmap);
		height = Math::abs(height);
		if (hBmp) {
			DeleteObject(hBmp);
			hBmp = nullptr;
		}

		/* RGBQUADs came from the BMP format
		typedef struct tagRGBQUAD {
			BYTE    rgbBlue;
			BYTE    rgbGreen;
			BYTE    rgbRed;
			BYTE    rgbReserved;
		} RGBQUAD;
		*/
		// COLORREF c = RGB(1,0,1);

		// bgra -> rgba
		for (int i = 0; i < pixels.size(); i += 4) {
			swap(pixels[i], pixels[i+2]);
		}
	::DeleteDC(hdc);
}
#endif

void NeheOGL_Lesson006::_loadTexture2D(StrView filename, MyImage& img, GLuint targetTexture) {
#if 0 && SGE_OS_WINDOWS
	_loadByHBITMAP(img, filename);
#else
	img.loadFile(filename);
#endif

	if (!img.width || !img.height)
		throw SGE_ERROR("_loadImage filename = {}", filename);

	glBindTexture(GL_TEXTURE_2D, targetTexture);	// Typical Texture Generation Using Data From The Bitmap
		// Generate The Texture
		glTexImage2D(GL_TEXTURE_2D,			// tells OpenGL the texture will be a 2D texture
					 0,						// lod (level of detail)
					 GL_RGBA,				// internalformat
					 img.width,				// image width
					 img.height,			// image height
					 0,						// the border. It's usually left at zero
					 GL_RGBA,				// color data format
					 GL_UNSIGNED_BYTE,		// means the data that makes up the image is made up of unsigned bytes
					 img.pixelData.data()	// source data
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear Filtering, when it's smaller (GL_TEXTURE_MIN_FILTER) on the screen than the actual texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear Filtering, use when the image is larger (GL_TEXTURE_MAG_FILTER) or stretched on the screen than the original texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void NeheOGL_Lesson006::onUIMouseEvent(UIMouseEvent& ev) {
	using Button	= UIMouseEvent::Button;
	using Type		= UIMouseEvent::Type;
	using Modifier	= UIEventModifier;

	if (ev.isDragging()) {
		switch (ev.pressedButtons) {
			case Button::Left: {
				if (BitUtil::hasAny(ev.modifier, Modifier::Alt)) {
					float d = 0.15f;
					_addCameraOrbitAngle(ev.deltaPos.yx() * d);
				}
			} break;
			case Button::Right: {
				float d = 0.15f;
				_addCameraOrbitAngle(ev.deltaPos.yx() * d);
			} break;
			case Button::Middle: {
				float d = 0.01f;
				_camerMovePos.x += ev.deltaPos.x * d;
				_camerMovePos.y += ev.deltaPos.y * -d;
			} break;
		}
	}

	if (ev.isScroll()) {
		auto d = ev.scroll * 0.015f;
		_camerMovePos.z += d.y;
	}
}

void NeheOGL_Lesson006::_addCameraOrbitAngle(const Vec2f& deltaAngle) {
	_camerOrbitAngle += deltaAngle;
}

void NeheOGL_Lesson006::_example1(float uptime) {
	float angle = uptime;

	float width = _clientRect.w;
	float height = _clientRect.h;

	if (height == 0) {
		height = 1;
	}

	float aspect = width / height;

	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));
	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D); // Enable Texture Mapping
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.f, aspect, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	glTranslatef(0.0f,0.0f,-5.0f); // Move Into The Screen 5 Units
	glRotatef(angle, 1.0f,1.0f,1.0f);

	glBindTexture(GL_TEXTURE_2D, _texture2d); // Select Our Texture, call it before glBegin/glEnd scope

	float d = 1.0f;
	glBegin(GL_QUADS);
		// Front Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-d,-d, d);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( d,-d, d);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( d, d, d);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-d, d, d);
		// Back Face
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-d,-d,-d);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-d, d,-d);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( d, d,-d);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( d,-d,-d);
		// Top Face
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-d, d,-d);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-d, d, d);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( d, d, d);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( d, d,-d);
		// Bottom Face
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-d,-d,-d);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( d,-d,-d);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( d,-d, d);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-d,-d, d);
		// Right face
		glTexCoord2f(1.0f, 0.0f); glVertex3f( d,-d,-d);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( d, d,-d);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( d, d, d);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( d,-d, d);
		// Left Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-d,-d,-d);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-d,-d, d);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-d, d, d);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-d, d,-d);
	glEnd();

	swapBuffers();
	drawNeeded();
}

void NeheOGL_Lesson006::_example2(float uptime) {
	float angle = uptime;
	auto tex = _texture2ds[1];

	glViewport(0, 0, static_cast<int>(_clientRect.w), static_cast<int>(_clientRect.h));
	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.f, _clientRect.w / _clientRect.h, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	glTranslatef(0,0,-5);
	glRotatef(angle, 1,1,1);

	glBindTexture(GL_TEXTURE_2D, tex);
	_mesh.bind();

	swapBuffers();
	drawNeeded();
}

void NeheOGL_Lesson006::_example3(float uptime) {
	float angle = uptime;

	auto tex0 = _texture2ds[0];
	auto tex1 = _texture2ds[1];

	glViewport(0, 0, static_cast<int>(_clientRect.w), static_cast<int>(_clientRect.h));
	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.f, _clientRect.w / _clientRect.h, 0.1f, 1000.0f);

	// setup camera
	glTranslatef(_camerMovePos.x, _camerMovePos.y, _camerMovePos.z);
	glRotatef(_camerOrbitAngle.x, 1,0,0);
	glRotatef(_camerOrbitAngle.y, 0,1,0);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	glTranslatef(0, 0, -5);
	glRotatef(angle, 1,1,1);

	int i = 0;
	float s = 1.f / 16.f;
	static Vec2f uvs[] = {
		{0,0},
		{1,0},
		{1,1},
		{0,1},
	};
	glBindTexture(GL_TEXTURE_2D, tex0);
	glBegin(GL_QUADS); // Front
		OGL::glTexCoord(_mesh.uv[i]); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord(_mesh.uv[i]); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord(_mesh.uv[i]); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord(_mesh.uv[i]); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
	glEnd();
	//glBindTexture(GL_TEXTURE_2D, 0); // unselect texture, or just select other opengl texture handle

	glBindTexture(GL_TEXTURE_2D, tex1);
	glBegin(GL_QUADS); // Back
		OGL::glTexCoord(_mesh.uv[i]); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord(_mesh.uv[i]); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord(_mesh.uv[i]); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord(_mesh.uv[i]); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex1);
	static Vec2f sOffsetRightDown = { 1,1 };
	glBegin(GL_QUADS); // Top
		OGL::glTexCoord((uvs[0]+sOffsetRightDown)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord((uvs[1]+sOffsetRightDown)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord((uvs[2]+sOffsetRightDown)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord((uvs[3]+sOffsetRightDown)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex1);
	glBegin(GL_QUADS); // Bottom
		OGL::glTexCoord((uvs[0]+sOffsetRightDown*2)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord((uvs[1]+sOffsetRightDown*2)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord((uvs[2]+sOffsetRightDown*2)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord((uvs[3]+sOffsetRightDown*2)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex1);
	glBegin(GL_QUADS); // Right
		OGL::glTexCoord((uvs[0]+sOffsetRightDown*3)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord((uvs[1]+sOffsetRightDown*3)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord((uvs[2]+sOffsetRightDown*3)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord((uvs[3]+sOffsetRightDown*3)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex1);
	glBegin(GL_QUADS); // Left
		OGL::glTexCoord((uvs[0]+sOffsetRightDown*4)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord((uvs[1]+sOffsetRightDown*4)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord((uvs[2]+sOffsetRightDown*4)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
		OGL::glTexCoord((uvs[3]+sOffsetRightDown*4)*s); OGL::glVertex(_mesh.pos[_mesh.indices[i++]]);
	glEnd();

	swapBuffers();
	drawNeeded();
}

}