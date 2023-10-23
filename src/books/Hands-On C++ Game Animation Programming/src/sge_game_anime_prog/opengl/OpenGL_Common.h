#pragma once

namespace sge {

struct OpenGLUtil {
	OpenGLUtil() = delete;
	
	template<class T> static constexpr GLenum getGLFormat();
	
	template<> static constexpr GLenum getGLFormat<void>() { return GL_NONE;  }

	template<> static constexpr GLenum getGLFormat<i8 >()  { return GL_BYTE;  }
	template<> static constexpr GLenum getGLFormat<i16>()  { return GL_SHORT; }
	template<> static constexpr GLenum getGLFormat<i32>()  { return GL_INT; }

	template<> static constexpr GLenum getGLFormat<u8 >()  { return GL_UNSIGNED_BYTE;  }
	template<> static constexpr GLenum getGLFormat<u16>()  { return GL_UNSIGNED_SHORT; }
	template<> static constexpr GLenum getGLFormat<u32>()  { return GL_UNSIGNED_INT; }

	template<> static constexpr GLenum getGLFormat<f32>()  { return GL_FLOAT; }
	template<> static constexpr GLenum getGLFormat<f64>()  { return GL_DOUBLE; }
};
	
}
