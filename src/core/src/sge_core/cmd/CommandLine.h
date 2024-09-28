#pragma once

#include <sge_core/string/UtfUtil.h>
#include <sge_core/string/StringUtil.h>

namespace sge {

struct CommandLine_Param {
	using This = CommandLine_Param;

	This() noexcept = delete;

	This(StrView  name) noexcept : opName(name) {}
	This(StrViewW name) noexcept { UtfUtil::convert(opName, name); }

	This(StrView name, i8&  v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	This(StrView name, i16& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	This(StrView name, i32& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	This(StrView name, i64& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }

	This(StrView name, u8&  v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	This(StrView name, u16& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	This(StrView name, u32& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	This(StrView name, u64& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }

	This(StrView name, f32& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	This(StrView name, f64& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }

	This(StrView name, StrView  v) noexcept : opName(name) { opValue.assign(v.data(), v.size()); }
	This(StrView name, StrViewW v) noexcept : opName(name) { UtfUtil::convert(opValue, v); }

	void opNameW (StringW& out) const { UtfUtil::convert(out, opName);  }
	void opValueW(StringW& out) const { UtfUtil::convert(out, opValue); }

	void opNameW (TempStringW& out) const { UtfUtil::convert(out, opName);  }
	void opValueW(TempStringW& out) const { UtfUtil::convert(out, opValue); }

	bool empty() const { return opName.empty(); }

	const char* name()  const { return opName.c_str();  }
	const char* value() const { return opValue.c_str(); }

	String opName;
	String opValue;
};

struct CommandLine {
	using Param = CommandLine_Param;

	CommandLine() = delete;

	static void runShell(StrView execFileName, const Span<Param> params = Span<Param>(), StrView workingDir = StrView());
};

}