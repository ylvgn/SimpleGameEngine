#pragma once
#include <sge_core/string/Lexer.h>
#include <sge_render/shader/ShaderInfo.h>

namespace sge {

class ShaderParser : public Lexer {
public:
	static void loadFile(ShaderInfo& outInfo, StrView filename);
	static void loadMem(ShaderInfo& outInfo, ByteSpan data, StrView filename);

private:
	void _loadMem(ShaderInfo& outInfo, ByteSpan data, StrView filename);

	void _readShader();
	void _readProperties();
	void _readProperty();
	void _readPass();

	template<class T> void _readEnum(T& outputShaderPropType);

	ShaderInfo* _outInfo = nullptr;
	MemMapFile _memMapFile;

}; // ShaderParser

template<class T> inline
void ShaderParser::_readEnum(T& outputShaderPropType) {
	if (!_token.isIdentifier()) {
		errorUnexpectedToken();
		return;
	}
	if (!enumTryParse(outputShaderPropType, _token.str)) {
		error("read enum [{}]", _token.str);
		return;
	}
	nextToken();
}

} // namespace
