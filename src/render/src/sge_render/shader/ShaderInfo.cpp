#include "ShaderInfo.h"
#include <sge_core/log/Log.h>

namespace sge {

	void ShaderInfo::clear() {
		props.clear();
		passes.clear();
	}

	void ShaderInfo::onFormat(fmt::format_context& ctx) const {
		TempString tmp;
		tmp += "------------------- props ------------------\n";
		for (auto& prop : props) {
			if (prop.displayName.size()) {
				FmtTo(tmp, "[[displayName:{}]]", prop.displayName);
			}
			FmtTo(tmp, "[({}) {} = {}]\n", enumStr(prop.propType), prop.name, prop.defaultValue);
		}

		tmp += "------------------- passes ------------------\n";
		for (auto& pass : passes) {
			if (pass.name.size()) {
				FmtTo(tmp, "\"{}:\"\n", pass.name);
			}
			FmtTo(tmp, "[vsFunc = {}]\n", pass.vsFunc);
			FmtTo(tmp, "[psFunc = {}]\n", pass.psFunc);
		}
		fmt::format_to(ctx.out(), "{}", tmp);
	}

}