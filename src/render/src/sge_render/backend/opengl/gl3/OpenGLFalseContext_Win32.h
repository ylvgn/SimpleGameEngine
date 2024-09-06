#pragma once

#if SGE_OS_WINDOWS

namespace sge {

class OpenGLFalseContext_Win32 : public NonCopyable {
public:
	~OpenGLFalseContext_Win32() { destroy(); }
	
	void create();
	void destroy();

private:
	::HWND	_hwnd	= nullptr;
	::HDC	_dc		= nullptr;
	::HGLRC	_rc		= nullptr;
};

}

#endif