#include "CommandLine.h"

#include "ShellCmd.h"

namespace sge {

void CommandLine::runShell(StrView execFileName, const Span<Param> params, StrView workingDir) {
	ShellCmd o;
	o.setExecFileName(execFileName);
	o.setWorkingDir(workingDir);
	o.exec(params.data(), params.size());
}

}