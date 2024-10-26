#pragma once

namespace sge {

struct CommandLine_Param;

#define CmdType_ENUM_LIST(E) \
	E(None,) \
	E(Shell,) \
	E(Batch,) \
//----
SGE_ENUM_DECLARE(CmdType, u8)

class Cmd : public NonCopyable {
public:
	using Param				= CommandLine_Param;
	using Type				= CmdType;

	virtual ~Cmd() = default;

	void setWorkingDir(StrView s)		{ _workingDir.assign(s.begin(), s.end()); }
	void setExecFileName(StrView s)		{ _execFileName.assign(s.begin(), s.end()); }

	void exec(Param* params, size_t n);

	template<size_t N>
	void exec(const Param(&params)[N]) {
		if (!N) return;
		exec(params, N);
	}

	Type type()	const { return _type; }

protected:
	Cmd(Type type) noexcept
		: _type(type)
	{}

	virtual void onExec(Param* params, size_t n) = 0;

	String			_workingDir;
	String			_execFileName;
	Type			_type = Type::None;
}; // Cmd

} // namespace sge