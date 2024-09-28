#pragma once

namespace sge {

struct CommandLine_Param;

#define CmdType_ENUM_LIST(E) \
	E(None,) \
	E(Shell,) \
	E(Batch,) \
//----
SGE_ENUM_DECLARE(CmdType, u8)

#define CmdParamAssignment_ENUM_LIST(E) \
	E(Space,) \
	E(Equals,) \
//----
SGE_ENUM_DECLARE(CmdParamAssignment, u8)

class Cmd : public NonCopyable {
public:
	using Param				= CommandLine_Param;
	using Type				= CmdType;
	using ParamAssignment	= CmdParamAssignment;

	virtual ~Cmd() noexcept = default;

	void setWorkingDir(StrView s)				{ _workingDir.assign(s.begin(), s.end()); }
	void setExecFileName(StrView s)				{ _execFileName.assign(s.begin(), s.end()); }
	void setParamAssignment(ParamAssignment v)	{ _paramAssignment = v; }

	void exec(Param* params, size_t n);

	template<size_t N>
	void exec(const Param(&params)[N]) {
		if (!N) return;
		exec(params, N);
	}

	Type			type()				const { return _type; }
	ParamAssignment	paramAssignment()	const { return _paramAssignment; }

protected:
	Cmd(Type type) noexcept
		: _type(type)
	{}

	virtual void onExec(Param* params, size_t n) = 0;

	String			_workingDir;
	String			_execFileName;

	Type			_type				= Type::None;
	ParamAssignment	_paramAssignment	= ParamAssignment::Space;
};

}