#pragma once

#if SGE_RENDER_HAS_GL

// GLEW --------
#define GLEW_STATIC 1
#include <GL/glew.h>

#if SGE_OS_WINDOWS
#include <GL/wglew.h>
#endif
// GLEW --------

#include <GL/gl.h>
#include <GL/glu.h>

#if SGE_RENDER_HAS_GL3
	#pragma comment(lib, "Opengl32.lib")
	#pragma comment(lib, "GLu32.lib")
#endif

#include <sge_render/Render_Common.h>
#include <sge_render/RenderDataType.h>
#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/shader/Shader.h>
#include <sge_render/textures/Texture.h>

namespace sge {
#if 0
#pragma mark ========= GLObject ============
#endif
class GLObject : public NonCopyable {
public:
	virtual ~GLObject() noexcept { SGE_ASSERT(_gl == 0); }

	bool isValid() { return _gl != 0; }

protected:
	GLuint _gl = 0;
};

#if 0
#pragma mark ========= GLVertexBuffer ============
#endif
class GLVertexBuffer : public GLObject {
public:
	~GLVertexBuffer() { destroy(); }

	template<class T> void create(Span<T> data);
	void destroy();

	void bind	() const { glBindBuffer(GL_ARRAY_BUFFER, _gl); }
	void unbind	() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
};

#if 0
#pragma mark ========= GLIndexBuffer ============
#endif
class GLIndexBuffer : public GLObject {
public:
	~GLIndexBuffer() { destroy(); }

	void create(const Span<const u16> data);
	void create(const Span<const u32> data);

	void destroy();

	void bind	() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _gl); }
	void unbind	() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
};

}

#endif // SGE_RENDER_HAS_GL