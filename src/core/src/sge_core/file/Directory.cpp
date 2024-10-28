#include "Directory.h"
#include <sge_core/string/StringUtil.h>
#include <sge_core/base/Error.h>
#include "FilePath.h"

namespace sge {

void Directory::create(StrView dir) {
	if (exists(dir))
		return;

	auto parent = FilePath::dirname(dir);
	if (!parent.empty()) {
		create(parent); // create recursively
	}

	_create(dir);
}

void Directory::remove(StrView dir) {
	if (!exists(dir))
		return;

	_remove(dir);
}

#if SGE_OS_WINDOWS

#if 0
#pragma mark ================= Windows ====================
#endif

void Directory::setCurrent(StrView dir) {
	TempStringW pathW = UtfUtil::toStringW(dir);
	int ret = ::SetCurrentDirectory(pathW.c_str());
	if (!ret) {
		auto errorCode = ::WSAGetLastError();
		switch (errorCode) {
			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				SGE_LOG("[Warning] The system cannot find the file specified: {}", dir); break;
			default: throw SGE_ERROR("::SetCurrentDirectory({}) error: {}", dir, errorCode); // TODO WSAGetLastError -> Win32Util::error()
		}
	}
//	SGE_DUMP_VAR(Directory::current());
}

void Directory::currentTo(String& out) {
	out.clear();
	StringW_<MAX_PATH> dirW;
	dirW.resizeToLocalBufSize();
	auto requiredSize = ::GetCurrentDirectory(MAX_PATH, dirW.data());
	if (!requiredSize)
		throw SGE_ERROR("::GetCurrentDirectory error: {}", ::WSAGetLastError());
	dirW.resize(requiredSize);
	UtfUtil::convert(out, dirW);
	out.replaceChars('\\', '/');
}

void Directory::_create(StrView dir) {
	TempStringW dirW;
	UtfUtil::convert(dirW, dir);
	auto ret = ::CreateDirectory(dirW.c_str(), nullptr);
	if (!ret) {
		auto errorCode = ::WSAGetLastError();
		switch (errorCode) {
			case ERROR_ALREADY_EXISTS: SGE_LOG("[Warning] Cannot create a file when that file already exists: {}", dir); break; // TODO incase error from makefile, but why???
			default: throw SGE_ERROR("::CreateDirectory({}) error: {}", dir, errorCode);
		}
	}
}

void Directory::_remove(StrView dir) {
	TempStringW dirW = UtfUtil::toStringW(dir);
	dirW += 0;

	// https://learn.microsoft.com/en-us/windows/win32/api/shellapi/ns-shellapi-shfileopstructa
	::SHFILEOPSTRUCTW op	= {};
	op.hwnd					= NULL;
	op.wFunc				= FO_DELETE;
	op.pFrom				= dirW.c_str(); // This string must be double-null terminated.
	op.pTo					= NULL;
	op.fFlags				= FOF_ALLOWUNDO | FOF_NO_UI;

	::SHFileOperationW(&op);
}

bool Directory::exists(StrView dir) {
	TempStringW dirW;
	UtfUtil::convert(dirW, dir);

	::DWORD dwAttrib = ::GetFileAttributes(dirW.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

#else

#if 0
#pragma mark ================= Unix ====================
#endif

Directory::_create(StrView dir) {
	TempStringA dirA;
	UtfUtil::convert(dirA, dir);
	auto ret = ::mkdir(dirA.c_str(), 0755);
	if (ret != 0)
		throw SGE_ERROR("::mkdir({}) error", dirA.c_str());
}

bool Directory::exists(StrView dir) {
	TempStringA dirA;
	UtfUtil::convert(dirA, dir);

	struct stat s;
	if( 0 != ::stat( dirA.c_str(), &s ) ) return false;
	return ( s.st_mode & S_IFDIR ) != 0;
}

#endif // Unix

} // namespace sge