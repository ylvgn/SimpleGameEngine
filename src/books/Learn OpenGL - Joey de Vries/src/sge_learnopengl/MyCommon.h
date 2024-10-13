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
SGE_INLINE double	timeInDouble()					{ return glfwGetTime(); }
SGE_INLINE float	time()							{ return float(glfwGetTime()); }

SGE_INLINE void		setWindowShouldClose(GLFWwindow* win, int exitCode)	{ glfwSetWindowShouldClose(win, exitCode); }
SGE_INLINE void		setWindowTitle(GLFWwindow* win, StrView title)		{ TempString tmp(title); glfwSetWindowTitle(win, tmp.c_str()); }
SGE_INLINE void		setWindowPos(GLFWwindow* win, int x, int y)			{ glfwSetWindowPos(win, x, y); }
SGE_INLINE void		setWindowPos(GLFWwindow* win, const Vec2i& pos)		{ glfwSetWindowPos(win, pos.x, pos.y); }
SGE_INLINE void		setWindowPos(GLFWwindow* win, const Vec2f& pos)		{ glfwSetWindowPos(win, int(pos.x), int(pos.y)); }
SGE_INLINE void		setWindowSize(GLFWwindow* win, int x, int y)		{ glfwSetWindowSize(win, x, y); }
SGE_INLINE void		setWindowSize(GLFWwindow* win, const Vec2i& size)	{ glfwSetWindowSize(win, size.x, size.y); }
SGE_INLINE void		setWindowSize(GLFWwindow* win, const Vec2f& size)	{ glfwSetWindowSize(win, int(size.x), int(size.y)); }

SGE_INLINE bool		isWindowShouldClose(GLFWwindow* win)							{ return glfwWindowShouldClose(win); }
SGE_INLINE void		framebufferSize(GLFWwindow* win, int& outWidth, int& outHeight)	{ glfwGetFramebufferSize(win, &outWidth, &outHeight); }
SGE_INLINE void		framebufferSize(GLFWwindow* win, Vec2i& outSize)				{ glfwGetFramebufferSize(win, &outSize.x, &outSize.y); }
SGE_INLINE void		framebufferSize(GLFWwindow* win, Vec2f& outSize)				{ int w, h; glfwGetFramebufferSize(win, &w, &h); outSize.set(f32(w), f32(h)); }
SGE_INLINE void		windowPos(GLFWwindow* win, int& outX, int& outY)				{ glfwGetWindowPos(win, &outX, &outY); }
SGE_INLINE void		windowPos(GLFWwindow* win, Vec2i& outPos)						{ glfwGetWindowPos(win, &outPos.x, &outPos.y); }
SGE_INLINE void		windowPos(GLFWwindow* win, Vec2f& outPos)						{ int x, y; glfwGetWindowPos(win, &x, &y); outPos.set(f32(x), f32(y)); }

}} // namespace sge/glfw
