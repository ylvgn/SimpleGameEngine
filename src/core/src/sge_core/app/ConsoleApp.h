#pragma once

#include "AppBase.h"

namespace sge {

class ConsoleApp : public AppBase {
public:
	int	exitCode()	const { return _exitCode; }

protected:
	int	_exitCode = 0;
}; // ConsoleApp

} // namespace sge