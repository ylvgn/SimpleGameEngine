#pragma once

#include <sge_core/base/Error.h>

namespace sge {

struct ProjectSettings : public NonCopyable {

	static ProjectSettings* instance();

	void		setProjectRoot(StrView path);
	StrView		projectRoot() const { return _projectRoot; }

	StrView		importedPath() const;

private:
	String _projectRoot;
};

} // namespace sge