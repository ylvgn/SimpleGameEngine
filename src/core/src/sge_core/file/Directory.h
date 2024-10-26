#pragma once

namespace sge {

struct Directory {
	Directory() = delete;

	static void		setCurrent(StrView dir);
	static void		currentTo(String& out);
	static String	current() { String o; currentTo(o); return o; }

	static void		create(StrView path);
	static void		remove(StrView path);
	static bool		exists(StrView path);

private:
	static void		_create(StrView path);
	static void		_remove(StrView path);
}; // Directory

} // namespace sge