#pragma once

#include "Cmd.h"

namespace sge {

class ShellCmd : public Cmd {
	using Base = Cmd;
public:
	ShellCmd() : Base(Base::Type::Shell) {}
protected:
	virtual void onExec(Param* params, size_t n) override;
};

}