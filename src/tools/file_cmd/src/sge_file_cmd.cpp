#include <sge_core/app/ConsoleApp.h>
#include <sge_core/log/Log.h>
#include <sge_core/file/File.h>
#include <sge_core/file/Directory.h>

namespace sge {

#define FileCmdOption_ENUM_LIST(E) \
	E(None,) \
	E(help,) \
	E(platform_os,) \
	E(touch,) \
	E(rm,) \
	E(mkdir,) \
	E(rmdir,) \
//----
SGE_ENUM_CLASS(FileCmdOption, u8)

class FileCmd : public ConsoleApp {
	using Option = FileCmdOption;
public:

	void showHelp() {
		SGE_LOG("\n==== sge_file_cmd Help: ===="
			"\n\tsge_file_cmd -platform_os"
			"\n\tsge_file_cmd -touch <filename>..."
			"\n\tsge_file_cmd -rm <filename>..."
			"\n\tsge_file_cmd -mkdir <dirname>..."
			"\n\tsge_file_cmd -rmdir <dirname>..."
		);
	}

	virtual void onRun() override {
#if 1
		_args = commandArguments();
#else
		auto exeFilePath = appName();;
		String exeDirname = FilePath::dirname();
		Directory::setCurrent(exeDirname);

		Vector<String> _argsStr;
		Vector<StrView> args;

		_argsStr.push_back(exeFilePath);
		_argsStr.push_back(String("-touch")); _argsStr.push_back(String("aaa.txt"));
		_argsStr.push_back(String("-mkdir")); _argsStr.push_back(String("a/b/c"));
		_argsStr.push_back(String("-rmdir")); _argsStr.push_back(String("a"));
		_argsStr.push_back(String("-platform_os"));

		args.reserve(_argsStr.size());
		for (auto& e : _argsStr) {
			args.push_back(e.view());
		}

		const auto& args_ = args;
		_args = args_.span();

#endif
//		SGE_DUMP_VAR(Directory::current());

		_argIndex = 0;
		_resetOp();

		_advance(1); // skip args[0]

		while (auto a = _next()) {
			if (a.starts_with('-')) {
				if (enumTryParse(_curOp, a.sliceFrom(1))) {
					a = _advance(1);
				}
			}

			if (!_handle(a)) {
				if (enumTryParse(_curOp, a)) {
					SGE_LOG("\n'{}' is not a sge_file_cmd. See 'sge_file_cmd --help'", a);
					SGE_LOG("The most similar command is\n\t-{}\n", _curOp);
					_resetOp();
				} else {
					SGE_LOG("Unknown argument {}", a);
				}
				_exitCode = -1;
				showHelp();
				return;
			}
		}
	}

private:
	bool _isEOF() const { return _args.empty() || _argIndex >= _args.size(); }

	void _resetOp() {
		_curOp = Option::None;
	}

	const StrView _advance(int i) {
		auto curArg = _peak();
		_argIndex += i;
		return curArg;
	}

	const StrView _next() {
		if (auto o = _peak()) {
			++_argIndex;
			return o;
		}
		return StrView();
	}

	const StrView _peak() {
		if (_isEOF()) return StrView();
		return _args[_argIndex];
	}

	bool _handle(const StrView& a) {
		using SRC = Option;
		switch (_curOp) {
			case SRC::help: {
				showHelp();
				_resetOp();
				return true;
			} break;
			case SRC::touch: {
				if (!File::exists(a)) {
					File::touch(a);
				} else {
					SGE_LOG("\n[Warning] create file failed, already exist file: '{}'", a);
				}
				return true;
			} break;
			case SRC::rm: {
				if (File::exists(a)) {
					File::remove(a);
				} else {
					SGE_LOG("\n[Warning] remove file failed, file not found: '{}'", a);
				}
				return true;
			} break;
			case SRC::mkdir: {
				if (!Directory::exists(a)) {
					Directory::create(a);
				} else {
					SGE_LOG("\n[Warning] create dir failed, already exist dir: '{}'", a);
				}
				return true;
			} break;
			case SRC::rmdir: {
				if (Directory::exists(a)) {
					Directory::remove(a);
				}
				else {
					SGE_LOG("\n[Warning] remove dir failed, dir not found: '{}'", a);
				}
				return true;
			} break;
			case SRC::platform_os: {
#if SGE_OS_WINDOWS
				SGE_LOG("{}", "Windows");
#elif SGE_OS_MACOSX
				SGE_LOG("{}", "MacOSX");
#elif SGE_OS_LINUX
				SGE_LOG("{}", "Linux");
#else
				SGE_LOG("{}", "Unknown");
#endif
				_resetOp();
				return true;
			} break;
		}

		_resetOp();
		return false;
	}

	Span<const StrView> _args;
	int					_argIndex = 0;
	Option				_curOp = Option::None;

}; // FileCmd

} // namespace sge


SGE_MAIN(sge::FileCmd)