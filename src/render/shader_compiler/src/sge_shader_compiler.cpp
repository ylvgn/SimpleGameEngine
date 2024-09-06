#include "ShaderCompiler_DX11.h"
#include "ShaderCompiler_GL3.h"
#include "ShaderParser.h"

namespace sge {

class ShaderCompiler : public ConsoleApp {
public:

protected:
	virtual void onRun() override {
		{
			String file = getExecutableFilename();
			String path = FilePath::dirname(file);
			path.append("/../../../../../../examples/Test101"); // tmp
			Directory::setCurrent(path);

			auto* proj = ProjectSettings::instance();
			proj->setProjectRoot(path);
		}		

		compile("Assets/Shaders/test.shader");
		compile("Assets/Shaders/test.hlsl");
		compile("Assets/Shaders/terrain.shader");
		compile("Assets/Shaders/terrain_test.shader");
		compile("Assets/Shaders/imgui.shader");
		compile("Assets/Shaders/line.shader");
		compile("Assets/Shaders/test_constbuffer.shader");

		SGE_LOG("---- end ----");
	}

	void compile(StrView shaderFilename) {
		ShaderInfo info;

		String outputPath = Fmt("LocalTemp/Imported/{}", shaderFilename);
		Directory::create(outputPath);

		{ // shader parser
			ShaderParser parser;
			parser.readFile(info, shaderFilename);

			auto jsonFilename = Fmt("{}/info.json", outputPath);
			JsonUtil::writeFile(jsonFilename, info, false);
		}

		{ // DX11
			size_t passIndex = 0;
			for (auto& pass : info.passes) {
				auto passOutPath = Fmt("{}/dx11/pass{}", outputPath, passIndex);

				if (pass.vsFunc.size()) {
					ShaderCompiler_DX11 c;
					c.compile(passOutPath, ShaderStageMask::Vertex, shaderFilename, pass.vsFunc);
				}

				if (pass.psFunc.size()) {
					ShaderCompiler_DX11 c;
					c.compile(passOutPath, ShaderStageMask::Pixel, shaderFilename, pass.psFunc);
				}

				++passIndex;
			}
		}

		{ // GL3
			size_t passIndex = 0;
			for (auto& pass : info.passes) {
				auto passOutPath = Fmt("{}/gl3/pass{}", outputPath, passIndex);

				ShaderCompiler_GL3 c;
				if (pass.vsFunc.size()) {
					c.compile(passOutPath, ShaderStageMask::Vertex, shaderFilename, pass.vsFunc);
				}

				if (pass.psFunc.size()) {
					c.compile(passOutPath, ShaderStageMask::Pixel, shaderFilename, pass.psFunc);
				}

				++passIndex;
			}
		}
	}

};

}

int main() {
	sge::ShaderCompiler app;
	app.run();

	return 0;
}