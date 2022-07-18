#include "ShaderCompiler_DX11.h"
#include "ShaderParser.h"

namespace sge {

class ShaderCompiler : public ConsoleApp {
public:

protected:
	virtual void onRun() {
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

				passIndex++;
			}
		}
	}

}; // ShaderCompiler

} // namespace

int main() {
	sge::ShaderCompiler app;
	app.run();

	return 0;
}