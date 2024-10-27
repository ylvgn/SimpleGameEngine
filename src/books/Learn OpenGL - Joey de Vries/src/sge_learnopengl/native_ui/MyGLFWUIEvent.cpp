#include "MyGLFWUIEvent.h"

namespace sge {
#if 0
#pragma mark ========= MyGLFWUIKeyboardEvent ============
#endif
MyGLFWUIKeyboardEvent::MyGLFWUIKeyboardEvent(GLFWwindow* win)
	: _win(win)
{
	SGE_ASSERT(win != nullptr);
}

int MyGLFWUIKeyboardEvent::_GLFW_KEY(KeyCode v) const {
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

bool MyGLFWUIKeyboardEvent::isUp(KeyCode v) const {
	return _glfwIsUp(_GLFW_KEY(v));
}

bool MyGLFWUIKeyboardEvent::isDown(KeyCode v) const {
	return _glfwIsDown(_GLFW_KEY(v));
}

bool MyGLFWUIKeyboardEvent::_glfwIsUp(int v) const {
	return _isState(v, GLFW_RELEASE);
}

bool MyGLFWUIKeyboardEvent::_glfwIsDown(int v) const {
	return _isState(v, GLFW_PRESS) || _isState(v, GLFW_REPEAT);
}

bool MyGLFWUIKeyboardEvent::_isState(int v, int expectedState) const {
	return glfwGetKey(_win, v) == expectedState;
}

} // namespace sge