#include <sge_core/app/ConsoleApp.h>

#include "ShaderParser.h"
#include "ShaderCompiler_DX11.h"
#include "ShaderCompiler_GL.h"

namespace sge {

class ShaderCompiler : public ConsoleApp {
public:

	using Profile = ShaderStageProfile;

	struct Option {
		Option()
			: genMakefile(false)
			, hlsl(false)
			, glsl(false)
		{}

		String	file;
		String	out;
		String	profile;
		String	entry;

		Vector<String>	include_dirs; // TODO not use now

		void log() {
			SGE_LOG("======== Option ========"
				"\nstatic const char* argvs[] = {{"
				"\n\tfile.c_str(),"
				"\n\t-file={0},"
				"\n\t-out={1},"
				"\n\t-profile={2},"
				"\n\t-entry={3},"
				"\n}}"
				, file, out, profile, entry
			);
		}

		bool genMakefile	: 1;
		bool hlsl			: 1;
		bool glsl			: 1;
	};

	struct ShaderStageProfileParser {

		void clear() {
			stage = ShaderStageMask::None;
			profile.clear();

			major = 0;
			minor = 0;
		}

		bool tryParse_DX11(StrView profileStr) {
			// e.g. profileStr = vs_5_0, ps_5_0, cs_5_0
			Vector<StrView> tokens;
			StringUtil::getTokens(profileStr, tokens, '_');

			if (tokens.size() < 3) return false;

			if (tokens[0] == "vs") stage = ShaderStageMask::Vertex;
			else if (tokens[0] == "ps") stage = ShaderStageMask::Pixel;
			else return false;

			if (!StringUtil::tryParse(tokens[1], major)) return false;
			if (!StringUtil::tryParse(tokens[2], minor)) return false;

			profile = profileStr;
			return true;
		}

		bool tryParse_GLSL(StrView profileStr) {
			// e.g. profileStr = vs_330, ps_330, cs_430
			Vector<StrView> tokens;
			StringUtil::getTokens(profileStr, tokens, '_');

			if (tokens.size() < 2) return false;
			if (tokens[0] == "vs") stage = ShaderStageMask::Vertex;
			else if (tokens[0] == "ps") stage = ShaderStageMask::Pixel;
			else return false;

			if (!StringUtil::tryParse(tokens[1], major)) return false;
			profile = tokens[1];

			// e.g. 430 -> major 4 minor 3
			minor = major / 10 % 10;
			major = major / 100;

			return true;
		}

		ShaderStageMask	stage = ShaderStageMask::None;
		String profile;

		int	major = 0;
		int	minor = 0;
	};

	Option opt;

	void showHelp() {
		SGE_LOG("==== sge_shader_compiler Help: ===="
			"\n\tsge_shader_compiler -genMakefile src=<filename> out=<filename>"
			"\n\tsge_shader_compiler -hlsl -file=<filename> -out=<filename> -profile=<profile_name> -entry=<entry_func_name>"
			"\n\tsge_shader_compiler -glsl -file=<filename> -out=<filename> -profile=<profile_name> -entry=<entry_func_name>"
		);
	}

	void debugCompileManually() {
		setCurDirRelativeToExecutable("/../../../../../../examples/Test101");

		SGE_DUMP_VAR(ProjectSettings::instance()->externalsToolsRoot());

		compile("Assets/Shaders/test.shader");
		compile("Assets/Shaders/terrain.shader");
		compile("Assets/Shaders/terrain_test.shader");
		compile("Assets/Shaders/imgui.shader");
		compile("Assets/Shaders/line.shader");
		compile("Assets/Shaders/test_constbuffer.shader");
		compile("Assets/Shaders/test_texture.shader");

		compile("Assets/Shaders/test.hlsl");

		SGE_LOG("\n---- end ----\n");
	}

	void debugBatchMode() {
		TempString file = appName();
		String workingDir = FilePath::dirname(file);

		TempString RelativeTest101 = "/../../../../../../examples/Test101";
		String RelativeTest101Imported = FilePath::combine(RelativeTest101, ProjectSettings::instance()->importedPath());

#if 1 // debug genMakefile
		workingDir.append(RelativeTest101);
		workingDir.append("/Assets/Shaders");
		Directory::setCurrent(workingDir);

		static const char* argvs[] = {
			file.c_str(),
			"-genMakefile",
			"-file=test.shader",
			"-out=../../LocalTemp/Imported/Assets/Shaders/test.shader",
		};
#endif

#if 0 // debug compile hlsl
// "sge_shader_compiler.exe" -hlsl -file="../../../../../Assets/Shaders/test.shader" -out="dx11/pass0/vs_5_0.bin" -profile=vs_5_0 -entry=vs_main -I=../../../../../Assets/Shaders
		workingDir.append(RelativeTest101Imported);
		workingDir.append("/Assets/Shaders/test.shader");
		Directory::setCurrent(workingDir);

		static const char* argvs[] = {
			file.c_str(),
			"-hlsl",
			"-file=../../../../../Assets/Shaders/test.shader",
			"-out=dx11/pass0/vs_5_0.bin",
			"-profile=vs_5_0",
			"-entry=vs_main",
			"-I=../../../../../Assets/Shaders",
		};
#endif

		setCommandArguments(argvs);
	}

	virtual void onRun() override {
#if 0 && _DEBUG // just for test
		debugBatchMode();
		SGE_DUMP_VAR(Directory::current());
#endif
		auto args = commandArguments();

#if _DEBUG
		if (args.size() <= 1) {
			debugCompileManually();
			return;
		}
#endif

		for (int i = 1; i < args.size(); ++i) {
			auto& a = args[i];
			
			if (a == "-genMakefile") {
				opt.genMakefile = true;
				continue;
			}
			if (a == "-hlsl") {
				opt.hlsl = true;
				continue;
			}
			if (a == "-glsl") {
				opt.glsl = true;
				continue;
			}
			if (auto v = a.extractFromPrefix("-file=")) {
				FilePath::unixPathTo(opt.file, v);
				continue;
			}
			if (auto v = a.extractFromPrefix("-out=")) {
				FilePath::unixPathTo(opt.out, v);
				continue;
			}
			if (auto v = a.extractFromPrefix("-entry=")) {
				opt.entry = v;
				continue;
			}
			if (auto v = a.extractFromPrefix("-profile=")) {
				opt.profile = v;
				continue;
			}
			if (auto v = a.extractFromPrefix("-I=")) {
				opt.include_dirs.emplace_back(v);
				continue;
			}

			SGE_LOG("Unknown argument {}", a);
			_exitCode = -1;
			showHelp();
			return;
		}

		if (opt.file.empty()) {
			SGE_LOG("please specific -file=<filename>");
			_exitCode = -1;
			showHelp();
			return;
		}

		if (!File::exists(opt.file)) {
			_exitCode = -1;
			SGE_LOG("file not found error: {}", opt.file);
			return;
		}

//		opt.log();

		if (opt.genMakefile) {
			_genMakefile();
		} else if (opt.hlsl) {
			_compileHLSL();
		} else if (opt.glsl) {
			_compileGLSL();
		} else {
			_exitCode = -1;
			showHelp();
		}
	}

private:
	void _genMakefile() {
		if (opt.out.empty()) {
			opt.out = Directory::current();
		}

		Directory::create(opt.out);

		ShaderInfo shaderInfo;
		{ // shader parser
			ShaderParser parser;

			MemMapFile mm;
			mm.open(opt.file);
			parser.readMem(shaderInfo, mm, opt.file);

			// does it need copy shader source file ??? so maybe no use include_dir
			//TempString shaderSrcFilenameCopy = Fmt("{}/srouce.shader", opt.out);
			//File::writeFileIfChanged(opt.out, mm, false);

			TempString shaderInfoFilename = Fmt("{}/info.json", opt.out);
			JsonUtil::writeFile(shaderInfoFilename, shaderInfo, false);
		}

		String relativeShaderSrcFilename;
		FilePath::relativeTo(relativeShaderSrcFilename, opt.file, opt.out);
		StrView relativeShaderSrcDir = FilePath::dirname(relativeShaderSrcFilename);

		String makefile;
		makefile.append(".PHONY: all\n");
		makefile.append("\nall:\n");
		makefile.append("\nCURRENT_MAKEFILE := $(lastword $(MAKEFILE_LIST))\n");
		makefile.append(Fmt("\nsge_src_shader = {}\n", relativeShaderSrcFilename));
		makefile.append("\n$(sge_src_shader): info.json\n");

		int passIndex = 0;
		for (const auto& pass : shaderInfo.passes)
		{
			_genMakefile_DX11	(makefile, passIndex, pass, relativeShaderSrcDir);
			_genMakefile_SPIRV	(makefile, passIndex, pass, relativeShaderSrcDir);
			_genMakefile_GLSL	(makefile, passIndex, pass, relativeShaderSrcDir);
			++passIndex;
		}

		TempString outFilename = FilePath::combine(opt.out, "GNUmakefile");
		File::writeFileIfChanged(outFilename, makefile, false);
	}

	void _genMakefile_DX11(String& makefile, int passIndex, const ShaderInfo::Pass& pass, StrView relativeShaderSrcDir) {
		using Util = DX11Util;

		makefile.append("\nifeq ($(BUILD_DX11), true)\n");
		
		TempString fmt = "\nall: dx11/pass{0}/{1}.bin"
			"\ndx11/pass{0}/{1}.bin: $(CURRENT_MAKEFILE) $(sgeShaderCompiler)"
			"\n-include dx11/pass{0}/{1}.bin.dep"
			"\ndx11/pass{0}/{1}.bin: $(sge_src_shader)"
			"\n\t\"$(sgeShaderCompiler)\" -hlsl -file=\"$<\" -out=\"$@\" -profile={1} -entry={2} -I={3}"
		;

		makefile.append(Fmt(fmt, passIndex, Profile::DX11_VS, pass.vsFunc, relativeShaderSrcDir));
		makefile.append("\n# ---");
		makefile.append(Fmt(fmt, passIndex, Profile::DX11_PS, pass.psFunc, relativeShaderSrcDir));

		makefile.append("\nendif # BUILD_DX11\n");
	}

	void _genMakefile_SPIRV(String& makefile, int passIndex, const ShaderInfo::Pass& pass, StrView relativeShaderSrcDir) {
		makefile.append("\nifeq ($(BUILD_SPIRV), true)\n");
		
		TempString fmt = "\nall: spirv/pass{0}/{1}.bin"
			"\nspirv/pass{0}/{1}.bin: $(CURRENT_MAKEFILE) $(glslc)"
			"\nspirv/pass{0}/{1}.bin: $(sge_src_shader)"
			"\n\t\"$(sgeFileCmd)\" -mkdir spirv/pass{0}"
			"\n\t\"$(glslc)\" -fhlsl-functionality1 -fhlsl-iomap -fshader-stage={2} -fentry-point={3} -I\"{4}\" -o \"$@\" -x hlsl \"$<\""
		;

		// TODO fshader-stage
		makefile.append(Fmt(fmt, passIndex, Profile::SPIRV_VS, "vertex", pass.vsFunc, relativeShaderSrcDir));
		makefile.append("\n# ---");
		makefile.append(Fmt(fmt, passIndex, Profile::SPIRV_PS, "fragment", pass.psFunc, relativeShaderSrcDir));

		makefile.append("\nendif # BUILD_SPIRV\n");
	}

	void _genMakefile_GLSL(String& makefile, int passIndex, const ShaderInfo::Pass& pass, StrView relativeShaderSrcDir) {
		using Util = GLUtil;

		makefile.append("\nifeq ($(BUILD_GLSL), true)\n");

		TempString fmt = "\nall: glsl/pass{0}/{1}.glsl"
			"\nglsl/pass{0}/{1}.glsl.spv: $(CURRENT_MAKEFILE) $(sgeShaderCompiler)"
			"\nglsl/pass{0}/{1}.glsl.spv: $(sge_src_shader)"
			"\n\t\"$(sgeFileCmd)\" -mkdir glsl/pass{0}"
			"\n\t\"$(glslc)\" -fhlsl-functionality1 -fhlsl-iomap -fshader-stage={2} -fentry-point={3} -I\"{4}\" -o \"$@\" -x hlsl \"$<\""
			"\nglsl/pass{0}/{1}.glsl: glsl/pass{0}/{1}.glsl.spv"
			"\nglsl/pass{0}/{1}.glsl: $(sge_src_shader)"
			"\n\t\"$(sgeShaderCompiler)\" -glsl -file=\"$<\" -out=\"$@\" -profile={1} -entry={3} -I={4}"
		;

		// TODO fshader-stage
		makefile.append(Fmt(fmt, passIndex, Profile::GLSL_VS, "vertex", pass.vsFunc, relativeShaderSrcDir));
		makefile.append("\n# ---");
		makefile.append(Fmt(fmt, passIndex, Profile::GLSL_PS, "fragment", pass.psFunc, relativeShaderSrcDir));

		makefile.append("\nendif # BUILD_GLSL\n");
	}

	void _compileHLSL() {
		ShaderStageProfileParser profileParser;
		if (!profileParser.tryParse_DX11(opt.profile)) {
			SGE_LOG("invalid HLSL profile error: {}", opt.profile);
			_exitCode = -1;
			return;
		}
		ShaderCompiler_DX11 c;
		c.compile(opt.out, profileParser.stage, profileParser.profile, opt.file, opt.entry);
	}

	void _compileGLSL() {
		ShaderStageProfileParser profileParser;
		if (!profileParser.tryParse_GLSL(opt.profile)) {
			SGE_LOG("invalid GLSL profile error: {}", opt.profile);
			_exitCode = -1;
			return;
		}
		ShaderCompiler_GL c;
		c.compile(opt.out, profileParser.stage, profileParser.profile, opt.file, opt.entry);
	}

	void compile(StrView shaderFilename) {
		ShaderInfo info;

		String outdir = Fmt("{}/{}", ProjectSettings::instance()->importedPath(), shaderFilename);
		Directory::create(outdir);

		{ // shader parser
			ShaderParser parser;
			parser.readFile(info, shaderFilename);

			auto jsonFilename = Fmt("{}/info.json", outdir);
			JsonUtil::writeFile(jsonFilename, info, false);
		}

		size_t passIndex = 0;
		for (auto& pass : info.passes) {
			if (!pass.vsFunc.empty()) {
				{ // DX11
					ShaderCompiler_DX11 c;
					TempString outFilename = Fmt("{}/dx11/pass{}/{}.bin", outdir, passIndex, Profile::DX11_VS);
					c.compile(outFilename, ShaderStageMask::Vertex, DX11Util::getDxStageProfile(ShaderStageMask::Vertex), shaderFilename, pass.vsFunc);
				}
				{ // GLSL
					ShaderCompiler_GL c;
					TempString outFilename = Fmt("{}/glsl/pass{}/{}.glsl", outdir, passIndex, Profile::GLSL_VS);
					c.compile(outFilename, ShaderStageMask::Vertex, GLUtil::getGlStageProfile(ShaderStageMask::Vertex), shaderFilename, pass.vsFunc);
				}
			}
			if (!pass.psFunc.empty()) {
				{ // DX11
					ShaderCompiler_DX11 c;
					TempString outFilename = Fmt("{}/dx11/pass{}/{}.bin", outdir, passIndex, Profile::DX11_PS);
					c.compile(outFilename, ShaderStageMask::Pixel, DX11Util::getDxStageProfile(ShaderStageMask::Pixel), shaderFilename, pass.psFunc);
				}
				{ // GLSL
					ShaderCompiler_GL c;
					TempString outFilename = Fmt("{}/glsl/pass{}/{}.glsl", outdir, passIndex, Profile::GLSL_PS);
					c.compile(outFilename, ShaderStageMask::Pixel, GLUtil::getGlStageProfile(ShaderStageMask::Pixel), shaderFilename, pass.psFunc);
				}
			}

			++passIndex;
		}
	}
}; // ShaderCompiler

} // namespace sge


SGE_MAIN(sge::ShaderCompiler)