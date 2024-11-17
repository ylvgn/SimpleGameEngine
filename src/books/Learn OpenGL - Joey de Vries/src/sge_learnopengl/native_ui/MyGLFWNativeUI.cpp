#include "MyGLFWNativeUI.h"
#include "MyGLFWUIEvent.h"

namespace sge {
#if 0
#pragma mark ========= MyGLFWNativeUIWindow ============
#endif
MyGLFWNativeUIWindow::~MyGLFWNativeUIWindow() {
	if (_glfwWin) {
		glfwDestroyWindow(_glfwWin);
		_glfwWin = nullptr;
	}
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

void MyGLFWNativeUIWindow::s_glfwSetErrorCallback(int error_code, const char* description) {
	SGE_LOG("glfw error({}): {}", error_code, description);
	SGE_ASSERT(false);
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

void MyGLFWNativeUIWindow::s_glfwSetWindowCloseCallback(GLFWwindow* window) {
	if (auto* win = s_getThis(window)) {
		win->onWindowCloseCallback();
	}
}

void MyGLFWNativeUIWindow::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);

	SGE_ASSERT(glfw::platform() != GLFW_PLATFORM_NULL);
	SGE_LOG("GLFW Version: {}", glfw::version());

	if (GLFW_FALSE == glfw::init())
		throw SGE_ERROR("glfw initialized failed");

	glfwSetErrorCallback(s_glfwSetErrorCallback);

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

	int in_x		= int(desc.rect.x);
	int in_y		= int(desc.rect.y);
	int in_width	= int(desc.rect.size.x);
	int in_height	= int(desc.rect.size.y);

	_glfwWin = glfwCreateWindow(in_width, in_height, kWindowTitle, nullptr, nullptr);
	if (!_glfwWin)
		throw SGE_ERROR("glfwCreateWindow");

	glfwSetFramebufferSizeCallback(_glfwWin, s_glfwSetFramebufferSizeCallback);
	glfwSetWindowPosCallback(_glfwWin, s_glfwSetWindowPosCallback);
	glfwSetWindowCloseCallback(_glfwWin, s_glfwSetWindowCloseCallback);

	glfwSetWindowUserPointer(_glfwWin, this);
	glfwMakeContextCurrent(_glfwWin);
	glfwSetWindowPos(_glfwWin, in_x, in_y + kWindowTitleOffsetY);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		throw SGE_ERROR("gladLoadGLLoader");
	}

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	SGE_LOG("OpenGL Version: {}.{}", major, minor);
}

#if 0
#pragma mark ========= MyGLFWNativeUIApp ============
#endif
void MyGLFWNativeUIApp::onCreate(CreateDesc& desc) {
	setCurDirRelativeToExecutable("/../../../../../Test108");
	Base::onCreate(desc);
}

void MyGLFWNativeUIApp::onRun() {
	SGE_ASSERT(_mainWin != nullptr);

	_deltaTime = glfw::time();

	while (!_mainWin->_glfwWindowShouldClose()) {
		_deltaTime = glfw::time() - _deltaTime;
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
		_mainWin->_glfwSetWindowShouldClose(_exitCode);
	}
}

} // namespace sge