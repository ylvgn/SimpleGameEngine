#pragma once

namespace sge {

struct Directory {
	Directory() = delete;

	static void		setCurrent(StrView dir);
	static void		currentTo(String& out);
	static String	current() { String o; currentTo(o); return o; }

	static void		create(StrView dir);
	static void		remove(StrView dir);
	static bool		exists(StrView dir);

private:
	static void		_create(StrView dir);
	static void		_remove(StrView dir);
}; // Directory

} // namespace sge