#pragma once

#include "MyGLFWNativeUI.h"

namespace sge {
#if 0
#pragma mark ========= MyGLFWUIKeyboardEvent ============
#endif
bool MyGLFWUIKeyboardEvent::isUp(KeyCode k) const {
	return _win->getKeyIsRelease(k);
}

bool MyGLFWUIKeyboardEvent::isDown(KeyCode k) const {
	return _win->getKeyIsDown(k);
}

bool MyGLFWUIKeyboardEvent::_glfwIsDown(int k) const {
	return _win->_getKeyIsDown(k);
}

bool MyGLFWUIKeyboardEvent::_glfwIsUp(int k) const {
	return _win->_getKeyIsRelease(k);
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

	glfwSwapBuffers(_glfwWin);
	glfwPollEvents();
}

bool MyGLFWNativeUIWindow::getKeyIsPress(KeyCode v) const {
	return _getKeyIsPress(_toGLFWKeyCode(v));
}

bool MyGLFWNativeUIWindow::getKeyIsRelease(KeyCode v) const {
	return _getKeyIsRelease(_toGLFWKeyCode(v));
}

bool MyGLFWNativeUIWindow::getKeyIsRepeat(KeyCode v) const {
	return _getKeyIsRepeat(_toGLFWKeyCode(v));
}

void MyGLFWNativeUIWindow::onSetWindowPos(const Vec2f& pos) {
	glfwSetWindowPos(_glfwWin, int(pos.x), int(pos.y));
}

void MyGLFWNativeUIWindow::onSetWindowSize(const Vec2f& size) {
	glfwSetWindowSize(_glfwWin, int(size.x), int(size.y));
}

bool MyGLFWNativeUIWindow::_getKeyIsPress(int v) const {
	return _getKeyCodeState(v, GLFW_PRESS);
}

bool MyGLFWNativeUIWindow::_getKeyIsRelease(int v) const {
	return _getKeyCodeState(v, GLFW_RELEASE);
}

bool MyGLFWNativeUIWindow::_getKeyIsRepeat(int v) const {
	return _getKeyCodeState(v, GLFW_REPEAT);
}

void MyGLFWNativeUIWindow::_glfwSetWindowTitle(StrView title) {
	TempString tmp(title);
	glfwSetWindowTitle(_glfwWin, tmp.c_str());
}

void MyGLFWNativeUIWindow::_handleNativeInputEvent() {
	_handleNativeUIKeyboardEvent();
}

void MyGLFWNativeUIWindow::_handleNativeUIKeyboardEvent() {
	UIKeyboardEvent ev(this);
	onUINativeKeyboardEvent(ev);
}

int MyGLFWNativeUIWindow::_toGLFWKeyCode(KeyCode v) const {
#define SGE__CASE(V, ...) case SRC_T::V: return enumInt(DST_T::V);
#define SGE_SWITCH(S, D) \
		using SRC_T = S; \
		using DST_T = D; \
		switch (v) { \
			S##_ENUM_LIST(SGE__CASE) \
			default: SGE_ASSERT(false); return 0; \
		} \
//----
	SGE_SWITCH(UIKeyboardEventKeyCode, GLFWKeyCode)
#undef SGE__CASE
#undef SGE_SWITCH
}

bool MyGLFWNativeUIWindow::_getKeyCodeState(int glfwKeyCode, int state) const {
	return glfwGetKey(_glfwWin, glfwKeyCode) == state;
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
MyGLFWNativeUIApp* MyGLFWNativeUIApp::_current = nullptr;

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

}