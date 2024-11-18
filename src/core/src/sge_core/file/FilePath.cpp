#include "FilePath.h"
#include "Directory.h"
#include <sge_core/string/StringUtil.h>

namespace sge {

StrView FilePath::dirname(StrView path) {
	auto* end = path.end();
	auto* begin = path.begin();

	if (end == nullptr) return StrView();
	auto* p = end - 1;
	for (; p >= begin; p--) {
		if (*p == '/' || *p == '\\') {
			return StrView(begin, p - begin);
		}
	}

	return StrView();
}

StrView FilePath::basename(StrView path) {
	auto* end = path.end();
	auto* begin = path.begin();

	if (end == nullptr) return StrView();
	auto* p = end - 1;
	for ( ; p >= begin; p--) {
		if (*p == '/' || *p == '\\') {
			p++;
			return StrView(p, end - p);
		}
	}

	return StrView();
}

StrView FilePath::extension(StrView path) {
	auto* end = path.end();
	auto* begin = path.begin();

	if (end == nullptr) return StrView();
	auto* p = end - 1;
	for ( ; p >= begin; p--) {
		if (*p == '.') {
			p++;
			return StrView(p, end - p);
		}
	}

	return StrView();
}

bool FilePath::isRealpath(const StrView& path) {
	if (path.size() < 1) return false;
	if (path[0] == '/') return true;

	if (path.size() < 2) return false;
	if (std::isalpha(path[0]) && path[1] == ':') return true;

	return false;
}

void FilePath::realpathTo(String& out_path, StrView in_path) {
	out_path.clear();

	if (!in_path.size())
		return;

	bool needSlash = false;
	if (isRealpath(in_path)) {
		needSlash = (in_path[0] == '/'); // Unix path need '/' at beginning
	} else {
		out_path = Directory::current();
		needSlash = true;
	}

	StrView p = in_path;
	while (p.size()) {
		auto s = StringUtil::splitByChar(p, "\\/");
		if (s.first == ".") {
			//skip '.'
		}
		else if (!s.first.size()) {
			//skip '/'
		}
		else if (s.first == "..") {
			auto* parent = StringUtil::findCharFromEnd(out_path, "\\/", false);
			if (parent == nullptr) {
				out_path.clear(); // no more parent folder
				return;
			}
			out_path.resize(parent - out_path.data());
		}
		else {
			if (needSlash) {
				out_path += '/';
			}
			out_path += s.first;
			needSlash = true;
		}
		p = s.second;
	}
}

void FilePath::relativeTo(String& out_path, StrView in_path, StrView in_relativePath) {
	out_path.clear();

	String from = realpath(in_path);
	String to	= realpath(in_relativePath);

	auto fv = from.view();
	auto tv = to.view();

	// Unix real path start with /
	if (fv && fv[0] == '/') fv = fv.sliceFrom(1);
	if (tv && tv[0] == '/') tv = tv.sliceFrom(1);

	for (;;) {
		auto fp = StringUtil::splitByChar(fv, "\\/");
		auto tp = StringUtil::splitByChar(tv, "\\/");
		if (!fp.first || !tp.first) break;
		if (fp.first != tp.first) break;

		fv = fp.second;
		tv = tp.second;
	}

	for (;;) {
		auto tp = StringUtil::splitByChar(tv, "\\/");
		if (!tp.first) break;
		out_path += "../";
		tv = tp.second;
	}

	out_path += fv;
	toUnixPath(out_path);
}

void FilePath::toUnixPath(String& src_path) {
	src_path.replaceChars('\\', '/');
}

void FilePath::unixPathTo(String& out_path, StrView in_path) {
	out_path.assign(in_path.begin(), in_path.end());
	toUnixPath(out_path);
}

void FilePath::toWindowsPath(String& src_path) {
	src_path.replaceChars('/', '\\');
}

void FilePath::windowsPathTo(String& out_path, StrView in_path) {
	out_path.assign(in_path.begin(), in_path.end());
	toWindowsPath(out_path);
}

} // namespace sge