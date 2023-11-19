#pragma once

#include <sge_game_anime_prog/opengl/OpenGL_Common.h>
#include <sge_game_anime_prog/math/quat.h>

namespace sge {
/*
	write animation data to A 32-bit floating-point texture
*/

class AnimTexture : public RefCountBase {
public:
	using Util = OpenGLUtil;

	AnimTexture();
	~AnimTexture();

	void setTexel(u32 x, u32 y, const vec3f&  v);
	void setTexel(u32 x, u32 y, const quat4f& v);

	void getTexel(vec4f& out, u32 x, u32 y);

	void save(StrView filename, bool createDir, bool logResult = true) const;
	void load(StrView filename);

	// similar to 'Texture::load'
	void uploadToGpu();

	// similar to `Texture::set` and `Texture::unset`
	void set(u32 uniformIndex, u32 textureIndex);
	void unset(u32 textureIndex);

	void resize(size_t newTexSize);

	u32		handle()	const { return _handle; }
	size_t	texSize()	const { return _texSize; }

	f32*	data()		const { return _data; }
	size_t	dataSize()	const { return _texSize * _texSize * 4; }
	size_t	byteSize()	const { return dataSize() * sizeof(f32); }

	ByteSpan span() const {
		auto* p = reinterpret_cast<u8*>(_data);
		return ByteSpan(p, p + byteSize());
	}

private:
	u32 _getTexelIndex(u32 x, u32 y) const {
		return (y * static_cast<u32>(_texSize) * 4) + (x * 4);
	}

	void _load(ByteSpan data, StrView filename);

	f32*	_data     = nullptr;
	size_t	_texSize  = 0;

	u32		_handle;  // OpenGL texture handle
};

}