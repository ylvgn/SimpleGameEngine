#pragma once

#include <sge_core/base/Error.h>
#include <sge_core/string/String.h>

namespace sge {

class Lexer : public NonCopyable {
public:
	enum class TokenType {
		None,
		Identifier, // x, color, UP
		Operator,   // {}
		Number,     // true, 6.02e23, 
		String,     // "music"
		Newline,    // '\n'
	};

	struct Token {

		TokenType	type = TokenType::None;
		String		str;

		bool isNone() const { return type == TokenType::None; }

		bool isIdentifier() const { return type == TokenType::Identifier; }
		bool isIdentifier(StrView s) const { return type == TokenType::Identifier && s == str; }

		bool isOperator() const { return type == TokenType::Operator; }
		bool isOperator(StrView s) const { return type == TokenType::Operator && s == str; }

		bool isString() const { return type == TokenType::String; }
		bool isString(StrView s) const { return type == TokenType::String && s == str; }

		bool isNewline() const { return type == TokenType::Newline; }

		void setNone() { type = TokenType::None; str.clear(); }
		void reset(TokenType type_) { type = type_; str.clear(); }
		void reset(TokenType type_, StrView s) { type = type_; str.assign(s.begin(), s.end()); }

		void onFormat(fmt::format_context& ctx) const;

	}; // Token

	void reset(ByteSpan source, StrView filename);
	void reset(StrView source, StrView filename);

	bool nextChar();
	bool nextToken();

	void trimSpaces();
	void skipNewlineTokens();

	bool isAlpha                (char c) { return isLowerCase(c) || isUpperCase(c); }
	bool isLowerCase            (char c) { return c >= 'a' && c <= 'z'; }
	bool isUpperCase            (char c) { return c >= 'A' && c <= 'Z'; }
	bool isDigit                (char c) { return c >= '0' && c <= '9'; }
	bool isAlphaDigitUnderscore (char c) { return isAlpha(c) || c == '_' || isDigit(c); }

	const Token& token() const { return _token; }

	template<class... Args>
	void error(const Args &... args) {
		auto msg = Fmt(args...);
		_error(msg);
	}

	void errorUnexpectedToken();

	void expectOperator(StrView s);
	void expectNewline();

	void readString(String& outputStr);
	void readIdentifier(String& outputStr);

	StrView getLastFewLines(size_t lineCount);

	const char& ch()  const { return _ch;     }
	const char* cur() const { return _cur;    }
	StrView source()  const { return _source; }
	size_t line()     const { return _line;   }
	size_t col()      const { return _col;    }

protected:
	void _error(StrView msg);

	bool _nextToken();
	void _appendAndNextChar();
	void _errorUnexpectedChar();

	bool _parseString();
	void _parseIdentifier();
	void _parseNumber();
	void _parseNewline();
	void _parseCommentBlock();
	void _parseCommentSingleLine();

	Token			_token;
	String			_filename;  // for error message
	StrView			_source;
	char			_ch         = 0;
	size_t			_col        = 0;
	size_t			_line       = 0;
	const char*     _cur        = nullptr;

}; // Lexer

inline
const char* enumStr(Lexer::TokenType v) {
	switch (v) {
#define E(T) case Lexer::TokenType::T: return #T;
		E(None)
		E(Identifier)
		E(Operator)
		E(Number)
		E(String)
		E(Newline)
#undef E
		default: {
			SGE_ASSERT(false);
			return "";
		}
	}
}

} // namespace

SGE_FORMATTER(sge::Lexer::Token)
SGE_FORMATTER_ENUM(sge::Lexer::TokenType)
