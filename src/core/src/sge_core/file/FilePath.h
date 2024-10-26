#pragma once

#include <sge_core/string/UtfUtil.h>

namespace sge {

struct FilePath {
	FilePath() = delete;

	static StrView	dirname(StrView path);
	static StrView	basename(StrView path);
	static StrView	extension(StrView path);

	static bool		isRealpath(const StrView& path);

	static void		realpathTo(String& out_path, StrView in_path);
	static String	realpath(StrView path) { String o; realpathTo(o, path); return o; }

	static void		relativeTo(String& out_path, StrView in_path, StrView in_relativePath);
	static String	relative(StrView path, StrView in_relativePath) { String o; relativeTo(o, path, in_relativePath); return o; }

	static void		toUnixPath(String& src_path);
	static void		unixPathTo(String& out_path, StrView in_path);
	static String	unixPath(StrView path) { String o; unixPathTo(o, path); return o; }

	static void		toWindowsPath(String& src_path);
	static void		windowsPathTo(String& out_path, StrView in_path);
	static String	windowsPath(StrView path) { String o; windowsPathTo(o, path); return o; }

	template<class... Args>
	static void		combineTo(String& src_path, Args&&... paths);

	template<class... Args>
	static String	combine(Args&&... paths) { String o; combineTo(o, SGE_FORWARD(paths)...); return o; }
}; // FilePath


template<class... Args> SGE_INLINE
void FilePath::combineTo(String& src_path, Args&&... paths) {
	auto sv = src_path.view();
	bool needSlash = !sv.empty() && !sv.ends_with('/') && !sv.ends_with('\\');
	
	StrView views[] { paths... };
	size_t n = sizeof...(paths);

	if (needSlash) src_path.append("/");
	for (size_t i = 0; i < n; ++i) {
		const auto& v = views[i];
		src_path.append(v.data(), v.size());
		if (i + 1 < n) src_path.append("/");
	}
}

} // namespace sge