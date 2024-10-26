#include "CommandLine.h"

#include "ShellCmd.h"

namespace sge {

void CommandLine::runShell(StrView execFileName, const Span<Param> params, StrView workingDir) {
	ShellCmd o;
	o.setExecFileName(execFileName);
	o.setWorkingDir(workingDir);
	o.exec(params.data(), params.size());
}

const char* CommandLine_Param::assignment() const {
	using SRC = Assignment;
	switch (opAssignment) {
		case SRC::Space:		return " ";
		case SRC::Equals:		return "=";
		case SRC::ColonEquals:	return ":=";
		default: throw SGE_ERROR("unsupported CommandLine_ParamAssignment");
	}
}

} // namespace sge