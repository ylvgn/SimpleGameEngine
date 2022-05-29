#pragma once

#include <sge_render.h>
#include <sge_core/app/ConsoleApp.h>

namespace sge {

class ShaderCompiler : public ConsoleApp {
public:
	void compile(StrView shaderFilename);
protected:
	virtual void onRun();
}; // ShaderCompiler

} // namespace