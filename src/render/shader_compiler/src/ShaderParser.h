#pragma once

#include <sge_core/string/Lexer.h>
#include <sge_render/shader/ShaderInfo.h>

namespace sge {

class ShaderParser : public Lexer {
public:
	static void readFile(ShaderInfo& outInfo, StrView filename);
	static void readMem (ShaderInfo& outInfo, ByteSpan data, StrView filename);
private:
	void _readMem(ShaderInfo& outInfo, ByteSpan data, StrView filename);

	void _readShader();
	void _readProperties();
	void _readProperty();
	void _readPass();

	void _readBlendFunc(RenderState::BlendFunc& v);

	ShaderInfo* _outInfo = nullptr;
	MemMapFile  _memMapFile;
}; // ShaderParser

} // namespace sge
