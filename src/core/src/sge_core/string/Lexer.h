#pragma once

#include <sge_core/base/Error.h>
#include <sge_core/string/StringUtil.h>

namespace sge {

#define Lexer_TokenType_ENUM_LIST(E) \
	E(None,)		\
	E(Identifier,)	\
	E(Operator,)	\
	E(Number,)		\
	E(String,)		\
	E(Newline,)		\
//----
SGE_ENUM_CLASS(Lexer_TokenType, u8)

class Lexer : public NonCopyable {
public:
	using TokenType = Lexer_TokenType;

	struct Token {
		String		str;
		TokenType	type = TokenType::None;

		bool isNone() const					{ return type == TokenType::None; }

		bool isIdentifier() const			{ return type == TokenType::Identifier; }
		bool isIdentifier(StrView s) const	{ return type == TokenType::Identifier && s == str; }

		bool isOperator() const				{ return type == TokenType::Operator; }
		bool isOperator(StrView s) const	{ return type == TokenType::Operator && s == str; }

		bool isString() const				{ return type == TokenType::String; }
		bool isString(StrView s) const		{ return type == TokenType::String && s == str; }

		bool isNumber() const				{ return type == TokenType::Number; }

		bool isNewline() const				{ return type == TokenType::Newline; }

		void setNone()						{ type = TokenType::None; str.clear(); }

		void onFormat(fmt::format_context& ctx) const;
	};

	void reset(ByteSpan source, StrView filename);
	void reset(StrView  source, StrView filename);

	bool nextChar();
	bool nextToken();
	bool nextLine();

	void trimSpaces();
	void skipNewlineTokens();

	bool isEOF() const { return _cur == nullptr || _source.empty() || _cur >= _source.end(); }

	bool isAlpha                (char c) const { return isLowerCase(c) || isUpperCase(c); }
	bool isLowerCase            (char c) const { return c >= 'a' && c <= 'z'; }
	bool isUpperCase            (char c) const { return c >= 'A' && c <= 'Z'; }
	bool isDigit                (char c) const { return c >= '0' && c <= '9'; }
	bool isAlphaDigitUnderscore (char c) const { return isAlpha(c) || c == '_' || isDigit(c); }

	template<class... Args>
	void error(const Args &... args) {
		auto msg = Fmt(args...);
		_error(msg);
	}

	void errorUnexpectedToken();
	void errorUnexpectedChar();

	void expectOperator(StrView s);
	void expectNewline();

	void readString(String& outputStr);
	void readIdentifier(String& outputStr);

	template<class E> void readEnum(E& v);
	void readBool(bool& v); //!!<-------- pass 1 bit bool& cause undefined behaviour

	void readNumber(u8 & v) { _readNumber(v); }
	void readNumber(u16& v) { _readNumber(v); }
	void readNumber(u32& v) { _readNumber(v); }
	void readNumber(u64& v) { _readNumber(v); }
	void readNumber(i8 & v) { _readNumber(v); }
	void readNumber(i16& v) { _readNumber(v); }
	void readNumber(i32& v) { _readNumber(v); }
	void readNumber(i64& v) { _readNumber(v); }
	void readNumber(f32& v) { _readNumber(v); }
	void readNumber(f64& v) { _readNumber(v); }

	StrView getLastFewLines(size_t lineCount);

	const char& ch		() const { return _ch;		}
	const char* cur		() const { return _cur;		}
	StrView source		() const { return _source;	}
	size_t line			() const { return _line;	}
	size_t col			() const { return _col;		}
	const Token& token	() const { return _token;	}

protected:
	void _error(StrView msg);

	bool _nextToken();
	Pair<StrView, StrView> _nextLine();

	void _parseCommentSingleLine();
	void _parseNewline();
	bool _parseCommentBlock();

	void _parseIdentifier();
	bool _parseNumber();
	bool _parseString();
	bool _parseOperator();

	Token			_token;
	String			_filename;  // for error message
	StrView			_source;

	size_t			_col        = 0;
	size_t			_line       = 0;
	const char*     _cur        = nullptr;
	char			_ch         = 0;
private:
	template<class T> void _readNumber(T& v);
};

template<class E> inline
void Lexer::readEnum(E& v) {
	if (!token().isIdentifier()) {
		errorUnexpectedToken();
		return;
	}
	if (!enumTryParse(v, _token.str)) {
		error("read enum [{}]", _token.str);
		return;
	}
	nextToken();
}

template<class T> inline
void Lexer::_readNumber(T& v) {
	if (!_token.isNumber()) {
		errorUnexpectedToken();
		return;
	}
	if (!StringUtil::tryParse(_token.str, v)) {
		errorUnexpectedToken();
		return;
	}
	nextToken();
}

SGE_FORMATTER(Lexer::Token)

} // namespace sge