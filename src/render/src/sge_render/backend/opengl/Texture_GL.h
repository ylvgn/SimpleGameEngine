#pragma once

#include <sge_render/textures/Texture.h>
#include <sge_render/backend/opengl/Render_GL_Common.h>

#if SGE_RENDER_HAS_OPENGL

namespace sge {

class Texture2D_GL : public Texture2D {
	using Base = Texture2D;
	using Util = GLUtil;
public:
	Texture2D_GL(CreateDesc& desc);
	~Texture2D_GL();

	void bind()		{ glBindTexture(GL_TEXTURE_2D, _tex); }
	void unbind()	{ glBindTexture(GL_TEXTURE_2D, 0); }

private:

	struct Format {
		Format() noexcept = default;
		Format(ColorType colorType) { set(colorType); }
		void set(ColorType colorType);

		GLenum internalFormat{0};
		GLenum sourceFormat{0};
		GLenum elementType{0};
	};

	Format _format;
	GLuint _tex	= 0;

}; // Texture_GL

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL