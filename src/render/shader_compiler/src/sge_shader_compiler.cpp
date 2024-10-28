#include <sge_core/app/ConsoleApp.h>

#include "ShaderParser.h"
#include "ShaderCompiler_DX11.h"
#include "ShaderCompiler_GL.h"

namespace sge {

class ShaderCompiler : public ConsoleApp {
public:

	using Profile = ShaderStageProfile;

	struct Option {
		struct Marco {

		};

		Option()
			: genMakefile(false)
			, hlsl(false)
			, glsl(false)
		{}

		Vector<String>	include_dirs;
		Vector<Marco>	marcos; // TODO not impl
		String			file;
		String			out;
		String			profile;
		String			entry;

		void log() {
			if (hlsl) {
				SGE_LOG("======== Option ========"
					"\nstatic const char* argvs[] = {{"
					"\n\tfile.c_str(),"
					"\n\t-hlsl"
					"\n\t-file={0},"
					"\n\t-out={1},"
					"\n\t-profile={2},"
					"\n\t-entry={3},"
					"\n\t-I=.,"
					"\n\t-I=..,"
					"\n}}"
					, file, out, profile, entry
				);
			}
			else if (glsl) {
				SGE_LOG("======== Option ========"
					"\nstatic const char* argvs[] = {{"
					"\n\tfile.c_str(),"
					"\n\t-glsl"
					"\n\t-file={0},"
					"\n\t-out={1},"
					"\n\t-profile={2},"
					"\n\t-entry={3},"
					"\n\t-I=.,"
					"\n\t-I=..,"
					"\n}}"
					, file, out, profile, entry
				);
			} else if (genMakefile) {
				SGE_LOG("======== Option ========"
					"\nstatic const char* argvs[] = {{"
					"\n\tfile.c_str(),"
					"\n\t-genMakefile"
					"\n\t-file={0},"
					"\n\t-out={1},"
					"\n\t-I=.,"
					"\n\t-I=..,"
					"\n}}"
					, file, out
				);
			}
			else {
				SGE_ASSERT(false);
			}
		}

		void addIncDirPath(StrView v) {
			if (!v) return;
			if (v.size() > 2 && v.starts_with('"') && v.ends_with('"')) {
				include_dirs.emplace_back(v.sliceFromAndBack(1));
			} else {
				include_dirs.emplace_back(v);
			}
		}

		void include_dirsTo(TempString& o, StrView syntax = "-I") {
			o.clear();
			for (auto& inc : include_dirs) {
				auto v = inc.view();
				if (!v) continue;
				if (v[0] == '\'') {
					throw SGE_ERROR("invalid include dir: {}", inc);
				}
				o.append(" ");
				if (v[0] == '"' && v.back() == '"') {
					o.append(syntax);
					o.append(v);
				}
				else {
					o.append(syntax);
					o += "\"";
					o.append(v);
					o += "\"";
				}
			}
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
			SGE_ASSERT(major >= 100);
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
			"\n\tsge_shader_compiler -genMakefile -file=<filename> -I=<dirpath1> -I=<dirpath2> out=<filename>"
			"\n\tsge_shader_compiler -hlsl -file=<filename> -out=<filename> -profile=vs_5_0|ps_5_0 -entry=<entry-point>"
			"\n\tsge_shader_compiler -glsl -file=<filename> -out=<filename> -profile=vs_330|ps_330 -entry=<entry-point>"
		);
	}

	void debugCompileManually() {
		setCurDirRelativeToExecutable("/../../../../../../examples/Test101");

		SGE_DUMP_VAR(ProjectSettings::instance()->externalsToolsRoot());

		Vector<String> include_dirs;
		include_dirs.emplace_back(".");
		include_dirs.emplace_back("..");
		include_dirs.emplace_back("Assets");
#if 1
		_compile("Assets/Shaders/test.shader", include_dirs);
		_compile("Assets/Shaders/terrain.shader", include_dirs);
		_compile("Assets/Shaders/terrain_test.shader", include_dirs);
		_compile("Assets/Shaders/imgui.shader", include_dirs);
		_compile("Assets/Shaders/line.shader", include_dirs);
		_compile("Assets/Shaders/test_constbuffer.shader", include_dirs);
		_compile("Assets/Shaders/test_texture.shader", include_dirs);

		_compile("Assets/Shaders/test.hlsl", include_dirs);

#else // just for single test
		_compile("Assets/Shaders/test.shader", include_dirs);
#endif
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
			"-I=.",
//			"-I..",
		};
#endif

#if 0 // debug compile hlsl: tips you can genMakefile first to check the argvs
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
//			"-I../../../../../Assets"
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
				opt.addIncDirPath(v);
				continue;
			}
			if (auto v = a.extractFromPrefix("-I")) { // -I without "=" is allow
				opt.addIncDirPath(v);
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

		opt.include_dirs.emplace_back(relativeShaderSrcDir);

		int passIndex = 0;
		for (const auto& pass : shaderInfo.passes)
		{
			_genMakefile_DX11	(makefile, passIndex, pass);
			_genMakefile_SPIRV	(makefile, passIndex, pass);
			_genMakefile_GLSL	(makefile, passIndex, pass);
			++passIndex;
		}

		TempString outFilename = FilePath::combine(opt.out, "GNUmakefile");
		File::writeFileIfChanged(outFilename, makefile, false);
	}

	void _genMakefile_DX11(String& makefile, int passIndex, const ShaderInfo::Pass& pass) {
		using Util = DX11Util;

		makefile.append("\nifeq ($(BUILD_DX11), true)\n");
		
		TempString fmt = "\nall: dx11/pass{0}/{1}.bin"
			"\ndx11/pass{0}/{1}.bin: $(CURRENT_MAKEFILE) $(sgeShaderCompiler)"
			"\n-include dx11/pass{0}/{1}.bin.dep"
			"\ndx11/pass{0}/{1}.bin: $(sge_src_shader)"
			"\n\t\"$(sgeShaderCompiler)\" -hlsl -file=\"$<\" -out=\"$@\" -profile={1} -entry={2} {3}"
		;

		TempString include_dirs;
		opt.include_dirsTo(include_dirs, "-I=");

		makefile.append(Fmt(fmt, passIndex, Profile::DX11_VS, pass.vsFunc, include_dirs));
		makefile.append("\n# ---");
		makefile.append(Fmt(fmt, passIndex, Profile::DX11_PS, pass.psFunc, include_dirs));

		makefile.append("\nendif # BUILD_DX11\n");
	}

	void _genMakefile_SPIRV(String& makefile, int passIndex, const ShaderInfo::Pass& pass) {
		makefile.append("\nifeq ($(BUILD_SPIRV), true)\n");
		
		TempString fmt = "\nall: spirv/pass{0}/{1}.bin"
			"\nspirv/pass{0}/{1}.bin: $(CURRENT_MAKEFILE) $(glslc)"
			"\nspirv/pass{0}/{1}.bin: $(sge_src_shader)"
			"\n\t\"$(sgeFileCmd)\" -mkdir spirv/pass{0}"
			"\n\t\"$(glslc)\" -fhlsl-functionality1 -fhlsl-iomap -fshader-stage={2} -fentry-point={3} {4} -o \"$@\" -x hlsl \"$<\""
		;

		TempString include_dirs;
		opt.include_dirsTo(include_dirs);
		
		// TODO fshader-stage
		makefile.append(Fmt(fmt, passIndex, Profile::SPIRV_VS, "vertex", pass.vsFunc, include_dirs));
		makefile.append("\n# ---");
		makefile.append(Fmt(fmt, passIndex, Profile::SPIRV_PS, "fragment", pass.psFunc, include_dirs));

		makefile.append("\nendif # BUILD_SPIRV\n");
	}

	void _genMakefile_GLSL(String& makefile, int passIndex, const ShaderInfo::Pass& pass) {
		using Util = GLUtil;

		makefile.append("\nifeq ($(BUILD_GLSL), true)\n");

		TempString fmt = "\nall: glsl/pass{0}/{1}.glsl"
			"\nglsl/pass{0}/{1}.glsl.spv: $(CURRENT_MAKEFILE) $(sgeShaderCompiler)"
			"\nglsl/pass{0}/{1}.glsl.spv: $(sge_src_shader)"
			"\n\t\"$(sgeFileCmd)\" -mkdir glsl/pass{0}"
			"\n\t\"$(glslc)\" -fhlsl-functionality1 -fhlsl-iomap -fshader-stage={2} -fentry-point={3} {4} -o \"$@\" -x hlsl \"$<\""
			"\nglsl/pass{0}/{1}.glsl: glsl/pass{0}/{1}.glsl.spv"
			"\nglsl/pass{0}/{1}.glsl: $(sge_src_shader)"
			"\n\t\"$(sgeShaderCompiler)\" -glsl -file=\"$<\" -out=\"$@\" -profile={1} -entry={3} {4}"
		;

		TempString include_dirs;
		opt.include_dirsTo(include_dirs);

		// TODO fshader-stage
		makefile.append(Fmt(fmt, passIndex, Profile::GLSL_VS, "vertex", pass.vsFunc, include_dirs));
		makefile.append("\n# ---");
		makefile.append(Fmt(fmt, passIndex, Profile::GLSL_PS, "fragment", pass.psFunc, include_dirs));

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
		c.compile(opt.out, profileParser.stage, profileParser.profile, opt.file, opt.entry, opt.include_dirs);
	}

	void _compileGLSL() {
		ShaderStageProfileParser profileParser;
		if (!profileParser.tryParse_GLSL(opt.profile)) {
			SGE_LOG("invalid GLSL profile error: {}", opt.profile);
			_exitCode = -1;
			return;
		}
		ShaderCompiler_GL c;
		c.compile(opt.out, profileParser.stage, profileParser.profile, opt.file, opt.entry, opt.include_dirs);
	}

	void _compile(StrView shaderFilename, Vector<String>& include_dirs) {
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
					c.compile(outFilename, ShaderStageMask::Vertex, DX11Util::getDxStageProfile(ShaderStageMask::Vertex), shaderFilename, pass.vsFunc, include_dirs);
				}
				{ // GLSL
					ShaderCompiler_GL c;
					TempString outFilename = Fmt("{}/glsl/pass{}/{}.glsl", outdir, passIndex, Profile::GLSL_VS);
					c.compile(outFilename, ShaderStageMask::Vertex, GLUtil::getGlStageProfile(ShaderStageMask::Vertex), shaderFilename, pass.vsFunc, include_dirs);
				}
			}
			if (!pass.psFunc.empty()) {
				{ // DX11
					ShaderCompiler_DX11 c;
					TempString outFilename = Fmt("{}/dx11/pass{}/{}.bin", outdir, passIndex, Profile::DX11_PS);
					c.compile(outFilename, ShaderStageMask::Pixel, DX11Util::getDxStageProfile(ShaderStageMask::Pixel), shaderFilename, pass.psFunc, include_dirs);
				}
				{ // GLSL
					ShaderCompiler_GL c;
					TempString outFilename = Fmt("{}/glsl/pass{}/{}.glsl", outdir, passIndex, Profile::GLSL_PS);
					c.compile(outFilename, ShaderStageMask::Pixel, GLUtil::getGlStageProfile(ShaderStageMask::Pixel), shaderFilename, pass.psFunc, include_dirs);
				}
			}

			++passIndex;
		}
	}
}; // ShaderCompiler

} // namespace sge


SGE_MAIN(sge::ShaderCompiler)