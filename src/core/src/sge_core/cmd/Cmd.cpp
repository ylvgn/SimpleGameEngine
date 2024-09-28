#include "Cmd.h"
#include <sge_core/file/Directory.h>

namespace sge {

void Cmd::exec(Param* params, size_t n) {
	SGE_ASSERT(!_execFileName.empty());

	auto curDir = Directory::getCurrent();
	bool hasChangeDir = !_workingDir.empty();

	if (hasChangeDir) Directory::setCurrent(_workingDir);
	onExec(params, n);
	if (hasChangeDir) Directory::setCurrent(curDir);
}

}