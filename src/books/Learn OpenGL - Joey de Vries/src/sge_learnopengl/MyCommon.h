#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace sge { namespace glfw {

SGE_INLINE int		platform()										{ return glfwGetPlatform(); }
SGE_INLINE void		version(int& major, int& minor, int& revision)	{ glfwGetVersion(&major, &minor, &revision); }
SGE_INLINE String	version() {
	int glfwMajor, glfwMinor, glfwRev;
	glfw::version(glfwMajor, glfwMinor, glfwRev);
	String str = Fmt("{}.{}.{}", glfwMajor, glfwMinor, glfwRev);
	return str;
}

SGE_INLINE int		init()							{ return glfwInit(); }
SGE_INLINE void		terminate()						{ glfwTerminate(); }
SGE_INLINE void		swapBuffers(GLFWwindow* win)	{ glfwSwapBuffers(win); }
SGE_INLINE void		pollEvents()					{ glfwPollEvents(); }
SGE_INLINE double	time()							{ return glfwGetTime(); }

SGE_INLINE void		setWindowTitle(GLFWwindow* win, StrView title)		{ TempString tmp(title); glfwSetWindowTitle(win, tmp.c_str()); }
SGE_INLINE void		setWindowPos(GLFWwindow* win, int x, int y)			{ glfwSetWindowPos(win, x, y); }
SGE_INLINE void		setWindowPos(GLFWwindow* win, const Vec2f& pos)		{ glfwSetWindowPos(win, int(pos.x), int(pos.y)); }
SGE_INLINE void		setWindowSize(GLFWwindow* win, int x, int y)		{ glfwSetWindowSize(win, x, y); }
SGE_INLINE void		setWindowSize(GLFWwindow* win, const Vec2f& size)	{ glfwSetWindowSize(win, int(size.x), int(size.y)); }

}} // namespace sge/glfw
