#include "Cmd.h"
#include <sge_core/file/Directory.h>

namespace sge {

void Cmd::exec(const Param* params, size_t n) const {
	SGE_ASSERT(!_execFileName.empty());

	auto oldDir = Directory::current();
	bool hasChangeDir = !_workingDir.empty();

	if (hasChangeDir) Directory::setCurrent(_workingDir);
	onExec(params, n);
	if (hasChangeDir) Directory::setCurrent(oldDir);
}

}