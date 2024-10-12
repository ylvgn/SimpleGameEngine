#include "MyGLFWNativeUI.h"
#include "MyGLFWUIEvent.h"

namespace sge {
#if 0
#pragma mark ========= MyGLFWNativeUIWindow ============
#endif
MyGLFWNativeUIWindow::~MyGLFWNativeUIWindow() {
	glfw::terminate();
}

void MyGLFWNativeUIWindow::update(float dt) {
	if (_glfwWindowShouldClose()) {
		return;
	}

	_handleNativeInputEvent();

	onRender();

	glfw::swapBuffers(_glfwWin);
	glfw::pollEvents();
}

void MyGLFWNativeUIWindow::_handleNativeInputEvent() {
	_handleNativeUIKeyboardEvent();
}

void MyGLFWNativeUIWindow::_handleNativeUIKeyboardEvent() {
	UIKeyboardEvent ev(this->_glfwWin);
	onUIKeyboardEvent(ev);
}

void MyGLFWNativeUIWindow::s_glfwSetFramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	if (auto* win = s_getThis(window)) {
		win->onFramebufferSizeCallback(width, height);
	}
}

void MyGLFWNativeUIWindow::s_glfwSetWindowPosCallback(GLFWwindow* window, int xpos, int ypos) {
	if (auto* win = s_getThis(window)) {
		win->onWindowPosCallback(xpos, ypos);
	}
}

void MyGLFWNativeUIWindow::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);

	int platform = glfw::platform();
	SGE_ASSERT(platform != GLFW_PLATFORM_NULL);

	String glfwVersion = glfw::version();
	SGE_DUMP_VAR(glfwVersion);

	if (GLFW_FALSE == glfw::init())
		throw SGE_ERROR("glfw initialized failed");

	if (desc.major > 0) {
		// specific OpenGL version, not set will be default max OpenGL version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, desc.major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, desc.minor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, enumInt(desc.profile));
	}

#if SGE_OS_MACOSX || SGE_OS_IOS
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	static const char*	kWindowTitle = "MyGLFWNativeUIWindow";
	static const int	kWindowTitleOffsetY = 50;

	auto& rect = desc.rect;

	_glfwWin = glfwCreateWindow(int(rect.size.x), int(rect.size.y), kWindowTitle, nullptr, nullptr);
	if (!_glfwWin)
		throw SGE_ERROR("Failed to create GLFW window");

	glfwSetWindowUserPointer(_glfwWin, this);
	glfwMakeContextCurrent(_glfwWin);
	glfwSetWindowPos(_glfwWin, int(rect.x), int(rect.y) + kWindowTitleOffsetY);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw SGE_ERROR("Failed to initialize GLAD");
	}

	GLint openglMajor, openglMinor;
	glGetIntegerv(GL_MAJOR_VERSION, &openglMajor);
	glGetIntegerv(GL_MINOR_VERSION, &openglMinor);
	SGE_DUMP_VAR(openglMajor, openglMinor);

	glfwSetFramebufferSizeCallback(_glfwWin, s_glfwSetFramebufferSizeCallback);
	glfwSetWindowPosCallback(_glfwWin, s_glfwSetWindowPosCallback);
}

#if 0
#pragma mark ========= MyGLFWNativeUIApp ============
#endif
void MyGLFWNativeUIApp::onCreate(CreateDesc& desc) {
	{ // set working dir
		auto exeFilePath = getExecutableFilename();
		String workingDir = FilePath::dirname(exeFilePath);
		workingDir.append("/../../../../../../../../examples/Test108");

		Directory::setCurrent(workingDir);
		auto curDir = Directory::getCurrent();
		SGE_LOG("current dir={}", curDir);
	}

	Base::onCreate(desc);
}

void MyGLFWNativeUIApp::onRun() {
	SGE_ASSERT(_mainWin != nullptr);

	_deltaTime = static_cast<float>(glfw::time());

	while (!_mainWin->_glfwWindowShouldClose()) {
		_deltaTime = static_cast<float>(glfw::time()) - _deltaTime;
		update(_deltaTime);
	}

	willQuit();
}

void MyGLFWNativeUIApp::onUpdate(float dt) {
	_mainWin->update(dt);
}

void MyGLFWNativeUIApp::onQuit() {
	Base::onQuit();
	if (!_mainWin->_glfwWindowShouldClose()) {
		_mainWin->_glfwSetWindowShouldClose(true);
	}
}

} // namespace sge