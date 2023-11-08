#include "AnimTexture.h"

namespace sge {

AnimTexture::AnimTexture() {
	// call any opengl function after "gladLoadGL" done
	glGenTextures(1, &_handle);
}

AnimTexture::~AnimTexture() {
	if (_data) {
		delete[] _data;
		_data = nullptr;
	}
	glDeleteTextures(1, &_handle);
}

void AnimTexture::setTexel(u32 x, u32 y, const vec3f& v) {
	u32 index = _getTexelIndex(x, y);

	_data[index+0] = v.x;
	_data[index+1] = v.y;
	_data[index+2] = v.z;
	_data[index+3] = 0;
}

void AnimTexture::setTexel(u32 x, u32 y, const quat4f& v) {
	u32 index = _getTexelIndex(x, y);

	_data[index+0] = v.x;
	_data[index+1] = v.y;
	_data[index+2] = v.z;
	_data[index+3] = v.w;
}

void AnimTexture::getTexel(vec4f& out, u32 x, u32 y) {
	u32 index = _getTexelIndex(x, y);

	out.set(_data[index+0],
			_data[index+1],
			_data[index+2],
			_data[index+3]);
}

void AnimTexture::save(StrView filename, bool createDir, bool logResult /*= true*/) const {
//	SGE_LOG("save {}:\n\ttexSize={}\n\tdataSize={}\n\tbyteSize={}", filename, _texSize, dataSize(), byteSize());
	File::writeFileIfChanged(filename, span(), createDir, logResult);
}

void AnimTexture::load(StrView filename) {
	MemMapFile mm;
	mm.open(filename);
	_load(mm, filename);
}

void AnimTexture::_load(ByteSpan data, StrView filename) {
	TempString s = filename;

	// supposed: _texSize * _texSize * 4 * sizeof(f32) == data.size()
	size_t texSize = static_cast<size_t>(
		Math::sqrt(static_cast<f32>(
			data.size() / sizeof(f32) / 4
		))
	);

	resize(texSize);

#if _DEBUG
	SGE_LOG("AnimTexture load {}:\n\ttexSize={}\n\tdataSize={}\n\tbyteSize={}", filename, _texSize, dataSize(), byteSize());
#endif

	std::memcpy(_data, data.data(), data.size());

	uploadToGpu();
}

void AnimTexture::uploadToGpu() {
	glBindTexture(GL_TEXTURE_2D, _handle);
	{
		auto size = static_cast<GLsizei>(_texSize);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA32F, size, size,
					 0,
					 GL_RGBA,
					 Util::getGLFormat<f32>(),
					 _data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void AnimTexture::set(u32 uniformIndex, u32 textureIndex) {
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glUniform1i(uniformIndex, textureIndex);
}

void AnimTexture::unset(u32 textureIndex) {
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
}

void AnimTexture::resize(size_t newTexSize) {
	if (_data) {
		delete[] _data;
		_data = nullptr;
	}

	_texSize	= newTexSize;
	size_t n	= dataSize();
	_data		= new f32[n];
}

}