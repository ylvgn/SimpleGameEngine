#pragma once

namespace sge {

enum class TokenType {
	None,
	Identifier, // x, color, UP
	Operator,   // {}
	Number,     // true, 6.02e23, 
	String,     // "music"
	Comment,    // // /**/
	Newline,    // '\n'
};

struct Token {
	TokenType type;
	String value;

	inline StrView mylog() {
		String type_name;
		switch (type) {
#define E(T) case TokenType::T: { type_name = #T;} break;
			E(None)
			E(Identifier)
			E(Operator)
			E(Number)
			E(String)
			E(Comment)
			E(Newline)
#undef E
		default: break;
		}
		return type_name + " : " + value;
	}

}; // Token

class Lexer {
public:
	Lexer() = default;
	Lexer(StrView source) {
		_source = source;
		_cur = source.begin();
	}

	bool moveNext();
	Token getToken();

private:
	StrView	_source;
	const char* _cur = nullptr;

}; // Lexer

} // namespace
