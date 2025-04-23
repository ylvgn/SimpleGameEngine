#pragma once

#include <sge_render/textures/Texture.h>
#include <sge_render/backend/opengl/Render_GL_Common.h>

#if SGE_RENDER_HAS_OPENGL

namespace sge {

#if 0
#pragma mark ========= Texture2D_GL__Format ============
#endif
struct Texture2D_GL__Format {

	Texture2D_GL__Format() = default;
	Texture2D_GL__Format(ColorType colorType) { set(colorType); }

	void set(ColorType colorType);

	GLenum internalFormat{0};
	GLenum sourceFormat{0};
	GLenum elementType{0};
};


#if 0
#pragma mark ========= Texture2D_GL ============
#endif
class Texture2D_GL : public Texture2D {
	using Base   = Texture2D;
	using Util   = GLUtil;
	using Format = Texture2D_GL__Format;
public:
	Texture2D_GL(CreateDesc& desc);
	~Texture2D_GL();

	void bind()		{ glBindTexture(GL_TEXTURE_2D, _tex); }
	void unbind()	{ glBindTexture(GL_TEXTURE_2D, 0); }
private:
	Format _format;
	GLuint _tex	= 0;
}; // Texture2D_GL


#if 0
#pragma mark ========= TextureCube_GL ============
#endif
class TextureCube_GL : public TextureCube {
	using Base	 = TextureCube;
	using Util	 = GLUtil;
	using Format = Texture2D_GL__Format;
public:
	TextureCube_GL(CreateDesc& desc);
	~TextureCube_GL();

	void bind()		{ glBindTexture(GL_TEXTURE_CUBE_MAP, _tex); }
	void unbind()	{ glBindTexture(GL_TEXTURE_CUBE_MAP, 0); }
private:
	Format _format;
	GLuint _tex	= 0;
}; // TextureCube_GL


} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL