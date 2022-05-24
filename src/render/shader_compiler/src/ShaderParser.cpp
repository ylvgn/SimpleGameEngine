#include "lexer.h"
#include "ShaderParser.h"
#include "sge_core/file/MemMapFile.h"

namespace sge {

	void ShaderParser::loadFile(ShaderInfo& outputInfo, StrView filename) {
		MemMapFile mm;
		mm.open(filename);
		loadMem(outputInfo, mm);
	}

	void ShaderParser::loadMem(ShaderInfo& outputInfo, Span<const u8> src) {
		ShaderParser inst;
		inst._loadMem(outputInfo, src);
	}

	void ShaderParser::_loadMem(ShaderInfo& outputInfo, Span<const u8> src) {
		outputInfo.clean();
		auto source = StrView_make(src);

	}

} // namespace
