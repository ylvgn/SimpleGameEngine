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
			// TODO not impl
		};

		Option() noexcept
			: genMakefile(false)
			, genNinja(false)
			, hlsl(false)
			, glsl(false)
			, genNinjaConfigure(false)
		{}

		Vector<String>	include_dirs;
		Vector<Marco>	marcos; // TODO not impl
		String			file;
		String			out;
		String			profile;
		String			entry;

		bool genMakefile		: 1;
		bool genNinja			: 1;
		bool hlsl				: 1;
		bool glsl				: 1;
		bool genNinjaConfigure	: 1;

		void onFormat(fmt::format_context& ctx) const {
			TempString tmp ="======== Option ========"
				   "\nstatic const char* argvs[] = {"
				   "\n\t\"sge_shader_compiler\"";

			if (genMakefile) tmp += "\n\t, \"-genMakefile\"";
			if (genNinja) tmp += "\n\t, \"-genNinja\"";
			if (hlsl) tmp += "\n\t, \"-hlsl\"";
			if (glsl) tmp += "\n\t, \"-glsl\"";
			if (genNinjaConfigure) tmp += "\n\t, \"-genNinjaConfigure\"";

			if (!file.empty())		FmtTo(tmp, "\n\t, \"-file={}\"", file);
			if (!out.empty())		FmtTo(tmp, "\n\t, \"-out={}\"", out);
			if (!profile.empty())	FmtTo(tmp, "\n\t, \"-profile={}\"", profile);
			if (!entry.empty())		FmtTo(tmp, "\n\t, \"-entry={}\"", entry);

			for (auto& ic : include_dirs) FmtTo(tmp, "\n\t, \"-I={}\"", ic);

			tmp += "\n};";

			fmt::format_to(ctx.out(), "{}", tmp);
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
	};
	Option opt;

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

	void showHelp() {
		SGE_LOG("==== sge_shader_compiler Help: ===="
			"\n\tsge_shader_compiler -genMakefile -file=<filename> -I=<dirpath1> -I=<dirpath2> -out=<outdir>"
			"\n\tsge_shader_compiler -genNinja -file=<filename> -I=<dirpath1> -I=<dirpath2> -out=<outdir>"
			"\n\tsge_shader_compiler -hlsl -file=<filename> -out=<filename> -profile=vs_5_0|ps_5_0 -entry=<entry-point> -I=<dirpath1> -I=<dirpath2>"
			"\n\tsge_shader_compiler -glsl -file=<filename> -out=<filename> -profile=vs_330|ps_330 -entry=<entry-point> -I=<dirpath1> -I=<dirpath2>"
			"\n\tsge_shader_compiler -genNinjaConfigure -file=<outfilename> -out=<outdir> -I=<dirpath1> -I=<dirpath2>"
		);
	}

	void debugCompileManually() {
		setCurDirRelativeToExecutable("/../../../Test101");

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

		TempString RelativeTest101 = "/../../../Test101";
		String RelativeTest101Imported = FilePath::combine(RelativeTest101, ProjectSettings::instance()->importedPath());

#if 0 // debug genMakefile
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

#if 0 // debug genNinja
		workingDir.append(RelativeTest101);
		workingDir.append("/Assets/Shaders");
		Directory::setCurrent(workingDir);

		static const char* argvs[] = {
			file.c_str(),
			"-genNinja",
			"-file=test.shader",
			"-out=../../LocalTemp/Imported/Assets/Shaders/test.shader",
		};
#endif

#if 1 // debug genNinjaConfigure
		workingDir.append(RelativeTest101);
		workingDir.append("/Assets/Shaders");
		Directory::setCurrent(workingDir);

		static const char* argvs[] = {
			file.c_str(),
			"-genNinjaConfigure",
			"-file=build.ninja",
			"-out=../../LocalTemp/Imported/Assets/Shaders",
			"-I=.",
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

	void trimQuote(StrView& v) {
		if (v.size() > 2 && v.starts_with('"') && v.ends_with('"')) {
			v = v.sliceFromAndBack(1);
		}
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
			if (a == "-genNinja") {
				opt.genNinja = true;
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
			if (a == "-genNinjaConfigure") {
				opt.genNinjaConfigure = true;
				continue;
			}
			if (auto v = a.extractFromPrefix("-file=")) {
				trimQuote(v);
				FilePath::unixPathTo(opt.file, v);
				continue;
			}
			if (auto v = a.extractFromPrefix("-out=")) {
				trimQuote(v);
				FilePath::unixPathTo(opt.out, v);
				continue;
			}
			if (auto v = a.extractFromPrefix("-entry=")) {
				trimQuote(v);
				opt.entry = v;
				continue;
			}
			if (auto v = a.extractFromPrefix("-profile=")) {
				trimQuote(v);
				opt.profile = v;
				continue;
			}
			if (auto v = a.extractFromPrefix("-I=")) {
				trimQuote(v);
				opt.include_dirs.emplace_back(v);
				continue;
			}
			if (auto v = a.extractFromPrefix("-I")) { // -I without "=" is allow
				trimQuote(v);
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

//		SGE_LOG("{}", opt);

		if (opt.genMakefile) {
			_throwIfNotExists(opt.file, -1);
			_genMakefile();
		} else if (opt.genNinja) {
			_throwIfNotExists(opt.file, -1);
			_genNinja();
		} else if (opt.hlsl) {
			_throwIfNotExists(opt.file, -1);
			_compileHLSL();
		} else if (opt.glsl) {
			_throwIfNotExists(opt.file, -1);
			_compileGLSL();
		} else if (opt.genNinjaConfigure) {
			_genNinjaConfigure();
		} else {
			_exitCode = -1;
			showHelp();
		}
	}

private:
	void _throwIfNotExists(StrView path, int exitCode = 0) {
		if (!File::exists(path)) {
			_exitCode = exitCode;
			throw SGE_ERROR("path not found error: path={}", path);
		}
	}

	static constexpr const char* kShaderInfoJsonfile = "info.json";

	void _parseShader(ShaderInfo& out) {
		if (opt.out.empty()) {
			opt.out = Directory::current();
		}

		ShaderParser parser;

		MemMapFile mm;
		mm.open(opt.file);
		parser.readMem(out, mm, opt.file);

		TempString outFilename = Fmt("{}/{}", opt.out, kShaderInfoJsonfile);
		JsonUtil::writeFile(outFilename, out, true);
	}

	void _genMakefile() {
		if (opt.out.empty()) {
			opt.out = ".";
		}

		ShaderInfo shaderInfo;
		_parseShader(shaderInfo);

		String relativeShaderSrcFilename;
		FilePath::relativeTo(relativeShaderSrcFilename, opt.file, opt.out);
		StrView relativeShaderSrcDir = FilePath::dirname(relativeShaderSrcFilename);
		opt.include_dirs.emplace_back(relativeShaderSrcDir);

		String makefile;
		makefile.append(".PHONY: all\n");
		makefile.append("\nall:\n");
		makefile.append("\nCURRENT_MAKEFILE := $(lastword $(MAKEFILE_LIST))\n");
		makefile.append(Fmt("\nsge_src_shader = {}\n", relativeShaderSrcFilename));
		makefile.append(Fmt("\n$(sge_src_shader): {}\n", kShaderInfoJsonfile));

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

	void _genNinja() {
		if (opt.out.empty()) {
			opt.out = ".";
		}

		ShaderInfo shaderInfo;
		_parseShader(shaderInfo);

		String relativeShaderSrcFilename;
		FilePath::relativeTo(relativeShaderSrcFilename, opt.file, opt.out);
		StrView relativeShaderSrcDir = FilePath::dirname(relativeShaderSrcFilename);
		opt.include_dirs.emplace_back(relativeShaderSrcDir);

		String ninja;
		ninja.append(Fmt("# !!! Generated by {}\n", appName()));
		ninja.append("\nninja_required_version = 1.3\n");
		ninja.append("\nbuilddir = _ninja\n");
		ninja.append(Fmt("\nsge_src_shader = {}\n", relativeShaderSrcFilename));

#if SGE_OS_WINDOWS
		ninja.append("\nexe_suffix = .exe");
		ninja.append("\nmulti_command_prefix = cmd /c ");
#else
		ninja.append("\nexe_suffix = ");
		ninja.append("\nmulti_command_prefix = ");
#endif

		TempString sge_tools_bin;
		executableDirPathTo(sge_tools_bin);

		static constexpr StrLiteral sgeFileCmd			= "sge_file_cmd";
		static constexpr StrLiteral sgeShaderCompiler	= "sge_shader_compiler";

		{
			ninja.append("\n\n#------");
			ninja.append(Fmt("\nsge_tools_bin = {}", sge_tools_bin));
			ninja.append(Fmt("\nsgeFileCmd = ${{sge_tools_bin}}/{}${{exe_suffix}}", sgeFileCmd));
			ninja.append(Fmt("\nsgeShaderCompiler = ${{sge_tools_bin}}/{}${{exe_suffix}}", sgeShaderCompiler));
			ninja.append("\nninja = ${sge_tools_bin}/ninja${exe_suffix}"
							"\nglslc = ${sge_tools_bin}/glslc${exe_suffix}"
							"\nspirv_cross = ${sge_tools_bin}/spirv-cross${exe_suffix}"
							"\n\n#------"
							"\nrule sgeFileCmd"
							"\n  command = ${sgeFileCmd} ${sge_cmd_args}"
							"\n\nrule touch"
							"\n  command = ${sgeFileCmd} -touch ${out}"
							"\n\nrule mkdir"
							"\n  command = ${sgeFileCmd} -mkdir ${out}"
							"\n\nrule ninja"
							"\n  command = ${ninja} -C ${in} --quiet"
							"\n\nrule BUILD_DX11"
							"\n  command = ${sgeShaderCompiler} -hlsl -file=${file} -out=${out} -profile=${profile} -entry=${entry} ${include_dirs}"
							"\n\nrule BUILD_SPIRV"
							"\n  command = ${glslc} -fshader-stage=${fshader_stage} -fentry-point=${entry} -fhlsl-functionality1 -fhlsl-iomap -o ${out} -x hlsl ${file} ${include_dirs}"
							"\n\nrule BUILD_GLSL"
							"\n  command = ${sgeShaderCompiler} -glsl -file=${file} -out=${out} -profile=${profile} -entry=${entry} ${include_dirs}"
			);
		}

		{
			ninja.append("\n\n#------");
			ninja.append("\nrule echo"
				"\n  command = echo ${in}"
				"\n  description = [SGE_LOG]$: \"in=${in}\", \"out=${out}\""
				"\n\nrule start"
				"\n  command = ${multi_command_prefix}echo ========================== $"
				"\n  && echo sge_tools_bin=${sge_tools_bin} $"
				"\n  && echo sge_build_bin=${sge_build_bin} $"
				"\n  && echo sgeFileCmd=${sgeFileCmd} $"
				"\n  && echo sgeShaderCompiler=${sgeShaderCompiler} $"
				"\n  && echo ninja=${ninja} $"
				"\n  && echo glslc=${glslc} $"
				"\n  && echo spirv_cross=${spirv_cross} $"
				"\n  && echo ========================== $"
				"\n  && echo"
			);
			ninja.append("\n\n#------");
			ninja.append("\nbuild start: start");
		}

		int passIndex = 0;
		for (const auto& pass : shaderInfo.passes)
		{
			_genNinja_DX11(ninja, passIndex, pass);
			_genNinja_SPIRV(ninja, passIndex, pass);
			_genNinja_GLSL(ninja, passIndex, pass);
			++passIndex;
		}

		TempString outFilename = FilePath::combine(opt.out, "build.ninja");
		File::writeFileIfChanged(outFilename, ninja, false);
	}

	void _genNinja_DX11(String& ninja, int passIndex, const ShaderInfo::Pass& pass) {
		ninja.append("\n\n#------ BUILD_DX11");

		TempString fmt = "\nbuild dx11/pass{0}/${{profile}}.bin: BUILD_DX11"
			"\n  file         = ${{sge_src_shader}}"
			"\n  profile      = {1}"
			"\n  entry        = {2}"
			"\n  include_dirs ={3}"
			"\n"
		;

		TempString include_dirs;
		opt.include_dirsTo(include_dirs, "-I=");

		ninja.append(Fmt(fmt, passIndex, Profile::DX11_VS, pass.vsFunc, include_dirs));
		ninja.append(Fmt(fmt, passIndex, Profile::DX11_PS, pass.psFunc, include_dirs));
	}

	void _genNinja_SPIRV(String& ninja, int passIndex, const ShaderInfo::Pass& pass) {
		ninja.append("\n\n#------ BUILD_SPIRV");

		TempString fmt = "\nbuild spirv/pass{0}/${{profile}}.bin: BUILD_SPIRV"
			"\n  file          = ${{sge_src_shader}}"
			"\n  profile       = {1}"
			"\n  fshader_stage = {2}"
			"\n  entry         = {3}"
			"\n  include_dirs  ={4}"
			"\n"
		;

		TempString include_dirs;
		opt.include_dirsTo(include_dirs);

		// TODO fshader-stage
		ninja.append(Fmt(fmt, passIndex, Profile::GLSL_VS, "vertex", pass.vsFunc, include_dirs));
		ninja.append(Fmt(fmt, passIndex, Profile::GLSL_PS, "fragment", pass.psFunc, include_dirs));
	}

	void _genNinja_GLSL(String& ninja, int passIndex, const ShaderInfo::Pass& pass) {
		ninja.append("\n\n#------ BUILD_GLSL");

		TempString fmt = "\nbuild glsl/pass{0}/${{profile}}.glsl: BUILD_GLSL"
			"\n  file         = ${{sge_src_shader}}"
			"\n  profile      = {1}"
			"\n  entry        = {2}"
			"\n  include_dirs ={3}"
			"\n"
		;

		TempString include_dirs;
		opt.include_dirsTo(include_dirs, "-I=");

		ninja.append(Fmt(fmt, passIndex, Profile::GLSL_VS, pass.vsFunc, include_dirs));
		ninja.append(Fmt(fmt, passIndex, Profile::GLSL_PS, pass.psFunc, include_dirs));
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

	void _genNinjaConfigure() {
		if (opt.include_dirs.empty()) {
			SGE_LOG("please specific -I=<dirpath1> -I=<dirpath2>");
			_exitCode = -1;
			return;
		}

		if (opt.file.empty()) opt.file = "build.ninja";
		if (opt.out.empty()) opt.out = ".";

		String fileA;
		FilePath::realpathTo(fileA, opt.file);
		auto outdir = FilePath::dirname(fileA);
		Directory::create(outdir);

		String ninja;
		ninja.append(Fmt("# !!! Generated by {}\n", appName()));
		ninja.append("\nninja_required_version = 1.3\n");
		ninja.append("\nbuilddir = _ninja\n");

		String sge_tools_bin;
		executableDirPathTo(sge_tools_bin);

		String sgeShaderCompiler = "${sge_tools_bin}/sge_shader_compiler${exe_suffix}";

#if SGE_OS_WINDOWS
		ninja.append("\nexe_suffix = .exe");
		ninja.append("\nmulti_command_prefix = cmd /c ");
		FilePath::toWindowsPath(sge_tools_bin);
		FilePath::toWindowsPath(sgeShaderCompiler);
#else
		ninja.append("\nexe_suffix = ");
		ninja.append("\nmulti_command_prefix = ");
#endif

		{
			ninja.append("\n\n#------");
			ninja.append(Fmt("\nsge_tools_bin = {}", sge_tools_bin));
			ninja.append(Fmt("\nsgeShaderCompiler = {}", sgeShaderCompiler));
			ninja.append("\nninja = ${sge_tools_bin}/ninja${exe_suffix}"
						 "\n\n#------"
						 "\nrule sc"
						 "\n  command = ${multi_command_prefix}${sgeShaderCompiler} -genNinja -file=${file} -out=${out} $"
						 "\n  && ${ninja} -C ${out} --quiet"
			);
		}

		for (const auto& ic : opt.include_dirs) {
			_genNinjaConfigure(ninja, ic, outdir);
		}

		File::writeFileIfChanged(opt.file, ninja, false);
	}

	void _genNinjaConfigure(String& ninja, StrView include_dir, StrView outdir) {
		ninja.append(Fmt("\n\n#------ {}", include_dir));

		using Entry = Directory::Entry;

		Vector< Entry > out_files;
		Directory::getFileSystemEntries(out_files, include_dir, true, [&](Entry& entry) {
			if (entry.hidden || entry.isDir) return false;
			auto sv = entry.name.view();
			return sv.ends_with(".shader");
		});

		TempString fmt = "\nbuild {0}/{1}: sc"
			"\n  file = {2}"
			"\n"
		;

		String relativeBuildNinja;
		for (const auto& entry : out_files) {
			FilePath::relativeTo(relativeBuildNinja, entry.path, outdir);
			ninja.append(Fmt(fmt, opt.out, entry.name, relativeBuildNinja));
		}
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
					TempString profile = DX11Util::getDxStageProfile(ShaderStageMask::Vertex);
					c.compile(outFilename, ShaderStageMask::Vertex, profile, shaderFilename, pass.vsFunc, include_dirs);
				}
				{ // GLSL
					ShaderCompiler_GL c;
					TempString outFilename = Fmt("{}/glsl/pass{}/{}.glsl", outdir, passIndex, Profile::GLSL_VS);
					TempString profile = GLUtil::getGlStageProfile(ShaderStageMask::Vertex);
					c.compile(outFilename, ShaderStageMask::Vertex, profile, shaderFilename, pass.vsFunc, include_dirs);
				}
			}

			if (!pass.psFunc.empty()) {
				{ // DX11
					ShaderCompiler_DX11 c;
					TempString outFilename = Fmt("{}/dx11/pass{}/{}.bin", outdir, passIndex, Profile::DX11_PS);
					TempString profile = DX11Util::getDxStageProfile(ShaderStageMask::Pixel);
					c.compile(outFilename, ShaderStageMask::Pixel, profile, shaderFilename, pass.psFunc, include_dirs);
				}
				{ // GLSL
					ShaderCompiler_GL c;
					TempString outFilename = Fmt("{}/glsl/pass{}/{}.glsl", outdir, passIndex, Profile::GLSL_PS);
					TempString profile = GLUtil::getGlStageProfile(ShaderStageMask::Pixel);
					c.compile(outFilename, ShaderStageMask::Pixel, profile, shaderFilename, pass.psFunc, include_dirs);
				}
			}

			++passIndex;
		}
	}
}; // ShaderCompiler

SGE_FORMATTER(ShaderCompiler::Option)

} // namespace sge


SGE_MAIN(sge::ShaderCompiler)