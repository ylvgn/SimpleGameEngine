#include "ShaderCompiler.h"
#include "ShaderParser.h"

#include "ShaderCompiler_DX11.h"

#include <sge_core/file/Directory.h>
#include <sge_render/shader/ShaderInfo.h>
#include <sge_core/log/Log.h>

namespace sge {

	void ShaderCompiler::onRun()
	{
		String file = getExecutableFilename();
		String path = FilePath::dirname(file);
		path.append("/../../../../../../examples/Test101");
		Directory::setCurrent(path);

		auto dir = Directory::getCurrent();
		SGE_LOG("current dir = {}", dir);

		{ // just for test
			StrView shaderFilename = "Assets/Shaders/test.shader";
			compile(shaderFilename);
		}
	}

	void ShaderCompiler::compile(StrView shaderFilename)
	{
		ShaderInfo info;
		String outputPath = Fmt("LocalTemp/Imported/{}", shaderFilename);
		Directory::create(outputPath);

		{ // shader parser
			ShaderParser parser;
			parser.readFile(info, shaderFilename);
			SGE_LOG("{}", info);
		}

		{ // DX11
			size_t passIndex = 0;
			for (auto& pass : info.passes) {
				auto passOutPath = Fmt("{}/dx11/pass{}", outputPath, passIndex);

				if (pass.vsFunc.size()) {
					ShaderCompiler_DX11 c;
					c.compile(passOutPath + "_vs", ShaderStage::Vertex, shaderFilename, pass.vsFunc);
				}

				if (pass.psFunc.size()) {
					ShaderCompiler_DX11 c;
					c.compile(passOutPath + "_ps", ShaderStage::Pixel, shaderFilename, pass.psFunc);
				}
				passIndex++;
			}
		}
	}

} // namespace