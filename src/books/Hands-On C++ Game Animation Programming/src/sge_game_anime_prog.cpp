#include "common.h"

namespace sge {

// https://registry.khronos.org/OpenGL/api/GL/wglext.h
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);

typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc) override {
		Base::onCreate(desc);

		{ // create opengl render context

			const HDC dc = hdc();
			PIXELFORMATDESCRIPTOR pfd;
			pfd = {}; // memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
			pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 24;
			pfd.cDepthBits = 32;
			pfd.cStencilBits = 8;
			pfd.iLayerType = PFD_MAIN_PLANE;
			int pixelFormat = ChoosePixelFormat(dc, &pfd);
			SetPixelFormat(dc, pixelFormat, &pfd);

			// legacy render context
			HGLRC tempRC = wglCreateContext(dc);
			wglMakeCurrent(dc, tempRC);

			// legacy render context just for get function pointer of 'wglCreateContextAttribsARB'
			PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
			wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
			const int attribList[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 3,
				WGL_CONTEXT_FLAGS_ARB, 0,
				WGL_CONTEXT_PROFILE_MASK_ARB,
				WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0,
			};

			// modern render context
			HGLRC hglrc = wglCreateContextAttribsARB(dc, 0, attribList);

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(tempRC);
			wglMakeCurrent(dc, hglrc);

			// use 'glad' to load all opengl core function
			if (!gladLoadGL()) {
				throw SGE_ERROR("Could not initialize GLAD\n");
			}

			SGE_LOG("OpenGL Version: {}.{} loaded", GLVersion.major, GLVersion.minor);
		}

		{ // vsynch: https://www.khronos.org/opengl/wiki/Swap_Interval
			PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
			bool swapControlSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;

			_vsynch = 0;
			if (swapControlSupported) {
				PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
				PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

				if (wglSwapIntervalEXT(1)) {
					SGE_LOG("Enabled vsynch\n");
					_vsynch = wglGetSwapIntervalEXT();
				}
				else {
					SGE_LOG("Could not enable vsynch\n");
				}
			}
			else { // !swapControlSupported
				SGE_LOG("WGL_EXT_swap_control not supported\n");
			}

			glGenVertexArrays(1, &_vertexArrayObject);
			glBindVertexArray(_vertexArrayObject);
		}

		{ // test1
			_testShader = UPtr<Shader>(new Shader(
				"Assets/Shaders/static.vert",
				"Assets/Shaders/lit.frag"
			));
			
			_testTexture = UPtr<Texture>(new Texture("Assets/Textures/uvChecker.png"));

			_vertexPositions	= UPtr< Attribute<vec3> >(new Attribute<vec3>());
			_vertexNormals		= UPtr< Attribute<vec3> >(new Attribute<vec3>());
			_vertexTexCoords	= UPtr< Attribute<vec2> >(new Attribute<vec2>());
			_indexBuffer		= UPtr<IndexBuffer>(new IndexBuffer());

			Vector<vec3> positions{
				vec3(-1, -1, 0),
				vec3(-1,  1, 0),
				vec3( 1, -1, 0),
				vec3( 1,  1, 0),
			};
			_vertexPositions->set(positions);

			Vector<vec3> normals;
			normals.resize(4, vec3::s_forward());
			_vertexNormals->set(normals);

			Vector<vec2> uvs = {
				vec2(0,0),
				vec2(0,1),
				vec2(1,0),
				vec2(1,1),
			};
			_vertexTexCoords->set(uvs);

			Vector<u32> indices = {
				0,1,2,
				2,1,3
			};
			_indexBuffer->set(indices);
		}
	}

	virtual void onCloseButton() override {
		if (_vertexArrayObject != 0) {
			HDC dc = hdc();
			HGLRC hglrc = wglGetCurrentContext();

			// delete VAO
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &_vertexArrayObject);
			_vertexArrayObject = 0;

			// delete render context
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hglrc);
			ReleaseDC(_hwnd, dc);
		}

		NativeUIApp::current()->quit(0);
	}

	virtual void onDraw() override {
		{ // update frame
			DWORD thisTick = GetTickCount();
			float deltaTime = float(thisTick - _lastTick) * 0.001f;
			_lastTick = thisTick;
			
			{
				_testRotation += deltaTime * 45.0f;
				while (_testRotation > 360.0f) {
					_testRotation -= 360.0f;
				}
			}
		}

		{ // render frame
			Base::onDraw();
			HDC dc = hdc();

			// viewport
			float clientWidth = _clientRect.size.x;
			float clientHeight = _clientRect.size.y;
			glViewport(0, 0, static_cast<GLsizei>(clientWidth), static_cast<GLsizei>(clientHeight));

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glPointSize(5.0f);
			glBindVertexArray(_vertexArrayObject);

			glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			{
				float aspect = clientWidth / clientHeight;
				mat4 projection = mat4::s_perspective(60.0f, aspect, 0.01f, 1000.0f);
				mat4 view = mat4::s_lookAt(vec3(0, 0, -5), vec3(0, 0, 0), vec3(0, 1, 0));
				mat4 model = quat::s_mat4(quat::s_angleAxis(Math::radians(_testRotation), vec3(0, 0, 1)));
				
				_testShader->bind();
				{
					_vertexPositions->bind(_testShader->findAttributeByName("position"));
					_vertexNormals->bind(_testShader->findAttributeByName("normal"));
					_vertexTexCoords->bind(_testShader->findAttributeByName("texCoord"));

					{
						Uniform<mat4>::set(_testShader->findUniformByName("model"), model);
						Uniform<mat4>::set(_testShader->findUniformByName("view"), view);
						Uniform<mat4>::set(_testShader->findUniformByName("projection"), projection);

						Uniform<vec3>::set(_testShader->findUniformByName("light"), vec3(0, 0, 1));

						_testTexture->set(_testShader->findUniformByName("tex0"), 0);
						{
							g_Draw(*_indexBuffer.get(), DrawMode::Triangles);
						}
						_testTexture->unset(0);
					}

					_vertexPositions->unbind(_testShader->findAttributeByName("position"));
					_vertexNormals->unbind(_testShader->findAttributeByName("normal"));
					_vertexTexCoords->unbind(_testShader->findAttributeByName("texCoord"));

				}
				_testShader->unbind();

			}

			SwapBuffers(dc);
			if (_vsynch != 0) {
				glFinish();
			}
			drawNeeded();
		}
	}

private:
	int _vsynch = 0;
	DWORD _lastTick = 0;
	GLuint _vertexArrayObject = 0;

	float _testRotation = 0.0f;
	UPtr<Shader> _testShader;
	UPtr<Texture> _testTexture;

	UPtr<Attribute<vec3>>	_vertexPositions;
	UPtr<Attribute<vec3>>	_vertexNormals;
	UPtr<Attribute<vec2>>	_vertexTexCoords;
	UPtr<IndexBuffer>		_indexBuffer;
};

class GameAnimeProgApp : public NativeUIApp {
	using Base = NativeUIApp;
protected:
	virtual void onCreate(CreateDesc& desc) override {

		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../examples/Test102");

			Directory::setCurrent(workingDir);
			auto curDir = Directory::getCurrent();
			SGE_LOG("current dir={}", curDir);
		}

		Base::onCreate(desc);

		{ // create window
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 800, 600 };
			_mainWin.create(winDesc);
			_mainWin.setWindowTitle("SGE Game Anime Prog Window");
		}
	}

private:
	MainWin _mainWin;
};

} // namespace


int main(int argc, const char** argv) {
	sge::GameAnimeProgApp app;
	sge::GameAnimeProgApp::CreateDesc desc;
	app.run(desc);

	return 0;
}