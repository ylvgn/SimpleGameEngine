#include "lexer.h"
#include "ShaderParser.h"
#include "sge_core/file/MemMapFile.h"

namespace sge {

	void ShaderParser::loadFile(ShaderParserOuputInfo& outputInfo, StrView filename) {
		MemMapFile mm;
		mm.open(filename);
		loadMem(outputInfo, mm);
	}

	void ShaderParser::loadMem(ShaderParserOuputInfo& outputInfo, Span<const u8> src) {
		ShaderParser inst;
		inst._loadMem(outputInfo, src);
	}

	void ShaderParser::_loadMem(ShaderParserOuputInfo& outputInfo, Span<const u8> src) {
		outputInfo.clean();
		auto source = StrView_make(src);
		_lexer.reset(new Lexer(source));

		while (_lexer->moveNext())
		{
			Token token = _lexer->getToken();
			switch (token.type)
			{
				case TokenType::None:break;
				case TokenType::Comment:break;
				case TokenType::Identifier:break;
				case TokenType::String:break;
				case TokenType::Number:break;
				case TokenType::Operator:break;
				case TokenType::Newline:break;
			default:
				break;
			}
		}
	}

} // namespace
