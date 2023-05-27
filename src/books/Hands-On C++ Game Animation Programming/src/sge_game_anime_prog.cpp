#include "common.h"
#include "DebugDraw.h"

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

#if 1
		{ // test mat4
			mat4 m1{2, 3, 2, 5,
				    1, 4, 6, 2,
				    3, 3, 5, 2,
				    4, 6, 2, 7};
			m1.transpose();

			mat4 m2{2, 3, 1, 2,
				    2, 3, 5, 3,
				    2, 3, 3, 1,
				    1, 1, 4, 3};
			m2.transpose();

			// test mat4*mat4
			mat4 m1mulm2{19, 26, 43, 30,
				         24, 35, 47, 26,
				         24, 35, 41, 26,
				         31, 43, 68, 49};
			m1mulm2.transpose();
			SGE_ASSERT(m1mulm2 == (m1 * m2));

			// test mat4*vec4
			vec4 m1mulv4{ 19,24,24,31 };
			SGE_ASSERT(m1mulv4 == (m1 * vec4(2,2,2,1)));

			//test inverse
			mat4 m1Inv = m1.inverse();
			SGE_ASSERT(mat4::s_identity().equals(m1 * m1Inv, 0.000001f));
		}
#endif

		Base::onCreate(desc);

		{ // create opengl render context
			const HDC dc = hdc();
			PIXELFORMATDESCRIPTOR pfd;
			pfd					= {};
			pfd.nSize			= sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion		= 1;
			pfd.dwFlags			= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
			pfd.iPixelType		= PFD_TYPE_RGBA;
			pfd.cColorBits		= 24;
			pfd.cDepthBits		= 32;
			pfd.cStencilBits	= 8;
			pfd.iLayerType		= PFD_MAIN_PLANE;
			int pixelFormat		= ChoosePixelFormat(dc, &pfd);
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
				} else {
					SGE_LOG("Could not enable vsynch\n");
				}
			}
			else { // !swapControlSupported
				SGE_LOG("WGL_EXT_swap_control not supported\n");
			}

			glGenVertexArrays(1, &_vertexArrayObject);
			glBindVertexArray(_vertexArrayObject);
		}

		_lastTick = GetTickCount();

		{ // test lit texture
			_testShader = UPtr<Shader> (new Shader(
				"Assets/Shaders/static.vert",
				"Assets/Shaders/lit.frag"
			));
			
			_testTexture = UPtr<Texture>(new Texture("Assets/Textures/uvChecker.png"));

			_vertexPositions	= UPtr< Attribute<vec3> >(new Attribute<vec3>());
			_vertexNormals		= UPtr< Attribute<vec3> >(new Attribute<vec3>());
			_vertexTexCoords	= UPtr< Attribute<vec2> >(new Attribute<vec2>());
			_indexBuffer		= UPtr<IndexBuffer>(new IndexBuffer());

			_debugPoints = UPtr<DebugDraw>(new DebugDraw());
			_debugLines = UPtr<DebugDraw>(new DebugDraw());
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

			_debugPoints->push_back(positions);

			SGE_ASSERT(indices.size() == 6);
			for (int i = 0; i < indices.size(); i+=3) {
				_debugLines->push_back(positions[indices[i]]);
				_debugLines->push_back(positions[indices[i+1]]);

				_debugLines->push_back(positions[indices[i+1]]);
				_debugLines->push_back(positions[indices[i+2]]);

				_debugLines->push_back(positions[indices[i+2]]);
				_debugLines->push_back(positions[indices[i]]);
			}

			_debugPoints->uploadToGpu();
			_debugLines->uploadToGpu();
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

	void update(float dt) {
		_testRotation += dt * 45.0f;
		while (_testRotation > 360.0f) {
			_testRotation -= 360.0f;
		}
	}

	virtual void onDraw() override {
		if (_vertexArrayObject == 0) return;
		if (_testShader == nullptr) return;

		{ // update frame
			DWORD thisTick = GetTickCount();
			float dt = float(thisTick - _lastTick) * 0.001f;
			_lastTick = thisTick;
			update(dt);
		}

		{ // render frame
			Base::onDraw();
			HDC dc = hdc();

			// set viewport
			float clientWidth = _clientRect.size.x;
			float clientHeight = _clientRect.size.y;
			glViewport(0, 0, static_cast<GLsizei>(clientWidth), static_cast<GLsizei>(clientHeight));

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glPointSize(5.0f);

			glBindVertexArray(_vertexArrayObject);
			glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			float aspect = clientWidth / clientHeight;
			mat4 projection = mat4::s_perspective(60.0f, aspect, 0.01f, 1000.0f);
			mat4 view = mat4::s_lookAt(vec3(0, 0, -5), vec3::s_zero(), vec3::s_up());

			{ // test lit texture
				mat4 model = quat::s_mat4(quat::s_angleAxis(Math::radians(_testRotation), vec3(0,0,1))); // or mat4::s_identity();
				mat4 mvp = projection * view * model;

				_testShader->bind();
				{
					{ // bind attributes
						_vertexPositions->bind(_testShader->findAttributeByName("position"));
						_vertexNormals->bind(_testShader->findAttributeByName("normal"));
						_vertexTexCoords->bind(_testShader->findAttributeByName("texCoord"));
					}

					{ // bind uniforms
						Uniform<mat4>::set(_testShader->findUniformByName("model"), model);
						Uniform<mat4>::set(_testShader->findUniformByName("view"), view);
						Uniform<mat4>::set(_testShader->findUniformByName("projection"), projection);
						Uniform<vec3>::set(_testShader->findUniformByName("light"), vec3(0, 0, 1));
						_testTexture->set(_testShader->findUniformByName("tex0"), 0);
					}

					g_Draw(*_indexBuffer.get(), DrawMode::Triangles);

					_debugLines->draw(DebugDrawMode::Lines, mvp);
					_debugPoints->draw(DebugDrawMode::Points, mvp, Color4f(0, 0, 1, 1));

					{ // unbind/deactive
						_testTexture->unset(0);

						_vertexPositions->unbind(_testShader->findAttributeByName("position"));
						_vertexNormals->unbind(_testShader->findAttributeByName("normal"));
						_vertexTexCoords->unbind(_testShader->findAttributeByName("texCoord"));
					}

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
	GLuint _vertexArrayObject = 0;

	DWORD _lastTick = 0;
	float _testRotation = 0.0f;

	UPtr<Shader>  _testShader;
	UPtr<Texture> _testTexture;

	UPtr<Attribute<vec3>>	_vertexPositions;
	UPtr<Attribute<vec3>>	_vertexNormals;
	UPtr<Attribute<vec2>>	_vertexTexCoords;
	UPtr<IndexBuffer>		_indexBuffer;

	UPtr<DebugDraw>			_debugPoints;
	UPtr<DebugDraw>			_debugLines;
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