#include "MyGLFWNativeUI.h"
#include "MyGLFWUIEvent.h"

namespace sge {
#if 0
#pragma mark ========= MyGLFWNativeUIWindow::CreateDesc ============
#endif
MyGLFWNativeUIWindow::CreateDesc::CreateDesc()
	: major(3)
	, minor(3)
	, profile(Profile::Core)
{
}

#if 0
#pragma mark ========= MyGLFWNativeUIWindow ============
#endif
void MyGLFWNativeUIWindow::destroy() {
	glfwTerminate();
}

void MyGLFWNativeUIWindow::update() {
	_handleNativeInputEvent();

	onRender();

	_glfwSwapBuffers();
	_glfwPollEvents();
}

void MyGLFWNativeUIWindow::onSetWindowPos(const Vec2f& pos) {
	glfwSetWindowPos(_glfwWin, int(pos.x), int(pos.y));
}

void MyGLFWNativeUIWindow::onSetWindowSize(const Vec2f& size) {
	glfwSetWindowSize(_glfwWin, int(size.x), int(size.y));
}

void MyGLFWNativeUIWindow::_glfwSetWindowTitle(StrView title) {
	TempString tmp(title);
	glfwSetWindowTitle(_glfwWin, tmp.c_str());
}

void MyGLFWNativeUIWindow::_handleNativeInputEvent() {
	_handleNativeUIKeyboardEvent();
}

void MyGLFWNativeUIWindow::_handleNativeUIKeyboardEvent() {
	UIKeyboardEvent ev(this->_glfwWin);
	onUINativeKeyboardEvent(ev);
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

	static const char* kWindowTitle = "MyGLFWNativeUIWindow";
	static const int kWindowTitleOffsetY = 50;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, desc.major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, desc.minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, int(desc.profile));

#if SGE_OS_MACOSX || SGE_OS_IOS
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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

	glfwSetFramebufferSizeCallback(_glfwWin, s_glfwSetFramebufferSizeCallback);
	glfwSetWindowPosCallback(_glfwWin, s_glfwSetWindowPosCallback);
}

#if 0
#pragma mark ========= MyGLFWNativeUIApp ============
#endif
void MyGLFWNativeUIApp::onCreate(CreateDesc& desc)
{
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
	Base::onRun();
	SGE_ASSERT(_mainWin != nullptr);

	while (!_mainWin->_glfwWindowShouldClose()) {
		_mainWin->update();
	}

	willQuit();
}

void MyGLFWNativeUIApp::onQuit() {
	Base::onQuit();
	if (_mainWin) {
		_mainWin->_glfwSetWindowShouldClose(true);
	}
}

} // namespace sge