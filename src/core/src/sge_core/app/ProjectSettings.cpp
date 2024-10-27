#include "ProjectSettings.h"
#include <sge_core/file/Directory.h>

namespace sge {

ProjectSettings* ProjectSettings::instance() {
	static ProjectSettings s;
	return &s;
}

void ProjectSettings::setProjectRoot(StrView path) {
	Directory::setCurrent(path);
	_projectRoot = Directory::current();
}

StrView ProjectSettings::importedPath() const {
	return "LocalTemp/Imported";
}

} // namespace sge