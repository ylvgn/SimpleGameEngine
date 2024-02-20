#include "NeheOGL_Lesson006.h"

namespace sge {

void NeheOGL_Lesson006::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);

	// Create A Texture
	glGenTextures(1, &_texture2d);
	_loadTexture2D("NeHe.bmp", _texture2d);
}

void NeheOGL_Lesson006::onDraw() {
	float uptime = static_cast<float>(_uptime.get() * 90.f);
	_example1(uptime);
}

void NeheOGL_Lesson006::_hBitMapToImage(MyImage& o, HBITMAP BitmapHandle) {
	auto& width = o.width;
	auto& height = o.height;
	auto& pixels = o.pixelData;

	BITMAP Bmp = { 0 };
	BITMAPINFO Info = { 0 };
	HDC DC = CreateCompatibleDC(NULL);
	HBITMAP OldBitmap = (HBITMAP)SelectObject(DC, BitmapHandle);
	GetObject(BitmapHandle, sizeof(Bmp), &Bmp);

	Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Info.bmiHeader.biWidth = width = Bmp.bmWidth;
	Info.bmiHeader.biHeight = height = Bmp.bmHeight;
	Info.bmiHeader.biPlanes = 1;
	Info.bmiHeader.biBitCount = Bmp.bmBitsPixel;
	Info.bmiHeader.biCompression = BI_RGB;
	Info.bmiHeader.biSizeImage = ((width * Bmp.bmBitsPixel + 31) / 32) * 4 * height;

	pixels.resize(Info.bmiHeader.biSizeImage);
	GetDIBits(DC, BitmapHandle, 0, height, pixels.data(), &Info, DIB_RGB_COLORS);
	SelectObject(DC, OldBitmap);

	height = std::abs(height);
	DeleteDC(DC);
}

void NeheOGL_Lesson006::_loadImage(MyImage& o, StrView filename) {
	o.clean();
	// Loads A Bitmap Image

	// a few VERY important things you need to know about the images you plan to use as textures
		// The image height and width MUST be a power of 2
		// The width and height must be at least 64 pixels,
		// and for compatability reasons, shouldn't be more than 256 pixels
	// If the image you want to use is not 64, 128 or 256 pixels on the width or height, resize it in an art program(like photoshop)
	
	if (filename.empty() || !File::exists(filename))
		return;

	// The AUX_RGBImageRec record will hold the bitmap width, height, and data
	TempStringW s = UtfUtil::toStringW(filename);
	_bmp = static_cast<HBITMAP>(LoadImage(GetModuleHandle(nullptr), s.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE));

	if (!_bmp)
		throw SGE_ERROR("LoadImage");

	_hBitMapToImage(o, _bmp);
}


void NeheOGL_Lesson006::_loadTexture2D(StrView filename, GLuint targetTexture) {
	MyImage image;
	_loadImage(image, filename);
	if (!image.width || !image.height)
		throw SGE_ERROR("_loadImage filename = {}", filename);

	glBindTexture(GL_TEXTURE_2D, targetTexture);// Typical Texture Generation Using Data From The Bitmap
	glTexImage2D(GL_TEXTURE_2D,					// tells OpenGL the texture will be a 2D texture
				 0,								// lod (level of detail)
				 GL_RGB,						// internalformat
				 image.width,					// image width
				 image.height,					// image height
				 0,								// the border. It's usually left at zero
				 GL_RGBA,						// color data format
				 GL_UNSIGNED_BYTE,				// means the data that makes up the image is made up of unsigned bytes
				 image.pixelData.data()		    // source data
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void NeheOGL_Lesson006::_example1(float uptime) {
	float angle = uptime;

	float width = _clientRect.w;
	float height = _clientRect.h;

	if (height == 0) {
		height = 1;
	}

	float angleInDegreeFov = 45.f;
	float aspect = width / height;

	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));
	glEnable(GL_TEXTURE_2D); // Enable Texture Mapping
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angleInDegreeFov, aspect, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glTranslatef(0.0f,0.0f,-5.0f);

	glRotatef(angle, 1.0f,1.0f,1.0f);

	glBindTexture(GL_TEXTURE_2D, _texture2d);

	glBegin(GL_QUADS);
		// Front Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		// Back Face
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		// Top Face
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		// Bottom Face
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		// Right face
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		// Left Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glEnd();

	swapBuffers();
	drawNeeded();
}

}