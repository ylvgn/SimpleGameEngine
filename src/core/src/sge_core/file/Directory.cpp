#include "Directory.h"
#include <sge_core/string/StringUtil.h>
#include <sge_core/base/Error.h>
#include "FilePath.h"

namespace sge {

void Directory::create(StrView path) {
	if (exists(path))
		return;

	auto parent = FilePath::dirname(path);
	if (!parent.empty()) {
		create(parent); // create recursively
	}

	_create(path);
}


void Directory::remove(StrView path) {
	if (!exists(path))
		return;

	_remove(path);
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
				SGE_LOG("The system cannot find the file specified: {}", dir); break;
			default: throw SGE_ERROR("::SetCurrentDirectory({}) error: {}", dir, errorCode); // TODO WSAGetLastError -> Win32Util::error()
		}
	}
	SGE_DUMP_VAR(Directory::current());
}

void Directory::currentTo(String& out) {
	out.clear();
	StringW_<MAX_PATH> pathW;
	pathW.resizeToLocalBufSize();
	auto requiredSize = ::GetCurrentDirectory(MAX_PATH, pathW.data());
	if (!requiredSize)
		throw SGE_ERROR("::GetCurrentDirectory error: {}", ::WSAGetLastError());
	pathW.resize(requiredSize);
	UtfUtil::convert(out, pathW);
	out.replaceChars('\\', '/');
}

void Directory::_create(StrView path) {
	TempStringW pathW;
	UtfUtil::convert(pathW, path);
	auto ret = ::CreateDirectory(pathW.c_str(), nullptr);
	if (!ret)
		throw SGE_ERROR("::CreateDirectory({}) error: {}", path, ::WSAGetLastError());
}

void Directory::_remove(StrView path) {
	TempStringW pathW = UtfUtil::toStringW(path);
	pathW += 0;

	// https://learn.microsoft.com/en-us/windows/win32/api/shellapi/ns-shellapi-shfileopstructa
	::SHFILEOPSTRUCTW op	= {};
	op.hwnd					= NULL;
	op.wFunc				= FO_DELETE;
	op.pFrom				= pathW.c_str(); // This string must be double-null terminated.
	op.pTo					= NULL;
	op.fFlags				= FOF_ALLOWUNDO | FOF_NO_UI;

	::SHFileOperationW(&op);
}

bool Directory::exists(StrView path) {
	TempStringW pathW;
	UtfUtil::convert(pathW, path);

	::DWORD dwAttrib = ::GetFileAttributes(pathW.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

#else

#if 0
#pragma mark ================= Unix ====================
#endif

Directory::_create(StrView path) {
	TempStringA pathA;
	UtfUtil::convert(pathA, path);
	auto ret = ::mkdir(pathA.c_str(), 0755);
	if (ret != 0)
		throw SGE_ERROR("::mkdir({}) error", pathA.c_str());
}

bool Directory::exists(StrView path) {
	TempStringA pathA;
	UtfUtil::convert(pathA, path);

	struct stat s;
	if( 0 != ::stat( pathA.c_str(), &s ) ) return false;
	return ( s.st_mode & S_IFDIR ) != 0;
}

#endif // Unix

} // namespace sge