#include "Lexer.h"
#include <sge_core/base/Error.h>
#include <sge_core/log/Log.h>
#include <sge_core/file/MemMapFile.h>

namespace sge {

void Lexer::Token::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "{}:{}", type, str);
}

void Lexer::reset(ByteSpan source, StrView filename) {
	reset(StrView_make(source), filename);
}

void Lexer::reset(StrView source, StrView filename) {
	_filename = filename;
	_source = source;
	_cur = _source.begin();
	_ch = 0;
	_col = 0;
	_line = 1;

	nextChar();
	nextToken();
}
	
// _ch is current char value, *cur is next char pointer
bool Lexer::nextChar() {
	_ch = 0;
	if (_cur == nullptr) return false;
	if (_cur >= _source.end()) return false;

	_ch = *_cur;
	_cur++;
	_col++;

	if (_ch == '\n') {
		_line++;
		_col = 0;
	}

	return true;
}

void Lexer::errorUnexpectedToken() {
	error("Unexpected token [{}]", _token.str);
}

bool Lexer::nextToken() {
	if (!_nextToken()) return false;
	SGE_DUMP_VAR(_line, _token);
	return true;
}

void Lexer::trimSpaces() {
	while (_ch == ' ' || _ch == '\t' || _ch == '\r') {
		nextChar();
	}
}

void Lexer::skipNewlineTokens() {
	while (_token.isNewline()) {
		nextToken();
	}
}

void Lexer::expectOperator(StrView s)
{
	if (!_token.isOperator(s)) {
		error("expected token {}", s);
		return;
	}
	nextToken();
}

void Lexer::expectNewline()
{
	if (!_token.isNewline()) {
		errorUnexpectedToken();
		return;
	}
	nextToken();
}

void Lexer::readIdentifier(String& outputStr) {
	if (!_token.isIdentifier()) {
		errorUnexpectedToken();
	}
	outputStr = _token.str;
	nextToken();
}

void Lexer::readString(String& outputStr) {
	if (!_token.isString()) {
		errorUnexpectedToken();
	}
	outputStr = _token.str;
	nextToken();
}

StrView Lexer::getLastFewLines(size_t lineCount) {
	if (!_cur) return StrView();

	auto n = lineCount;

	auto* start = _source.data();

	auto* p = _cur;
	for (; p >= start && n > 0; p--) {
		if (*p == '\n') {
			n--;
		}
	}
	p++;

	return StrView(p, _cur - p);
}

void Lexer::_error(StrView msg) {
	TempString tmp = msg;
	FmtTo(tmp, "\n{}\n", getLastFewLines(3));

	{
		TempString lastLine = getLastFewLines(1);
		size_t i = 0;
		for (auto& c : lastLine) {
			if (i >= _col) break;
			if (c == ' ') { tmp += '-';	continue; }
			if (c == '\n') { tmp += c;		continue; }
			if (c == '\t') { tmp += "----";	continue; }
			tmp += '-';
			i++;
		}
		tmp += "^^^\n";
	}

	FmtTo(tmp, "file - {}:{}:{}\n", _filename, _line, _col);
	throw SGE_ERROR("{}", tmp);
}

bool Lexer::_nextToken() {
	_token.setNone();

	for (;;) {
		trimSpaces();
		if (!_ch) return false;

		if (_ch == '#') {
			_parseCommentSingleLine();
			continue;
		}

		// check newline
		if (_ch == '\n') {
			_parseNewline();
			return true;
		}

		// check comment
		if (_ch == '/') {
			nextChar();

			if (_ch == '/') {
				_parseCommentSingleLine();
				continue;
			}

			if (_ch == '*') {
				_parseCommentBlock();
				continue;
			}

			_errorUnexpectedChar();
		}

		// check string
		if (_ch == '\"' || _ch == '\'') {
			return _parseString();
		}

		// check number
		if (isDigit(_ch)) {
			_parseNumber();
			return true;
		}

		// check identifier
		if (_ch == '_' || isAlpha(_ch)) {
			_parseIdentifier();
			return true;
		}

		// final operator
		_token.type = TokenType::Operator;
		_appendAndNextChar();
		return true;
	}
}

void Lexer::_parseNewline() {
	// _ch start as '\n'
	_token.reset(TokenType::Newline, "<newline>");
	nextChar();
}

// e.g. 1234.56e-78;
void Lexer::_parseNumber() {
	// _ch start as '0' ~ '9'
		
	_token.reset(TokenType::Number);
	bool hasDot = false;
	bool hasE = false;

	while (_ch) {
			
		// check dot
		if (_ch == '.') {
			if (hasDot || hasE) {
				_errorUnexpectedChar();
			}
			hasDot = true;
			_appendAndNextChar();
			continue;
		}
			
		// check e
		if (_ch == 'e' || _ch == 'E') {
			if (hasE) {
				_errorUnexpectedChar();
			}
			hasE = true;
			_appendAndNextChar(); // add 'e' or 'E'

			if (nextChar()) {
				if (_ch == '+' || _ch == '-') {
					_appendAndNextChar();
				}
			}
		}

		if (isDigit(_ch)) {
			_appendAndNextChar();
		} else {
			break;
		}
	}
}

void Lexer::_parseCommentSingleLine() {
	// _ch start as '/'
	TempString tmp;
	while (_ch != '\n' && nextChar()) {
		tmp += _ch;
	}
	tmp.pop_back(); // ignore last '\n'
	SGE_LOG("ignore comment single line:\n{}", tmp);
	nextChar(); // skip '\n'
}

void Lexer::_parseCommentBlock() {
	// _ch start as '*'
	TempString tmp = "/*";
	while (nextChar())
	{
		tmp += _ch;
		if (_ch == '*') {
			nextChar();
			if (_ch == '/') break;
		}
	}
	tmp += _ch;
	SGE_LOG("ignore comment block:\n{}", tmp);
	if (_ch == '/') {
		nextChar(); // skip '/'
	}
}

bool Lexer::_parseString() {
	// _ch start as \" or \'
	_token.reset(TokenType::String);

	const char validquote = _ch; // start quote \" \'

	_appendAndNextChar(); // skip \" \'
	auto* _st = _cur - 1;

	while (_ch && _ch != '\n' && _ch != '"' && _ch != '\'') {

		// handle \\, \/, \", \b, \f, \n, \r, \t
		if (_ch == '\\') {
			nextChar();
			switch (_ch) {
				case '\\':
				case '/':
				case '\"':
				case '\'':
					_appendAndNextChar(); // skip '\'
					break;
				case 'b':
				case 'f':
				case 'n':
				case 'r':
				case 't':
					_token.str += '\\';
					_appendAndNextChar(); // add '\xxxxx'
					break;
				default:
					_errorUnexpectedChar();
			}
			continue;
		}

		_appendAndNextChar();
	}

	// check valid, _ch must be \" or \'
	if (_ch != validquote) {
		auto invalidStrView = StrView(_st, _cur - _st);
		error("invalid string token: {}", invalidStrView);
		return false;
	}

	_appendAndNextChar(); // add \" \'
	return true;
}

void Lexer::_parseIdentifier() {
	_token.reset(TokenType::Identifier);
	_appendAndNextChar();

	while (_ch && isAlphaDigitUnderscore(_ch)) {
		_appendAndNextChar();
	}

	// check invalid, now _ch must be empty character
	if (!_ch && _ch != ' ' && _ch != '\t' && _ch != '\n' && _ch != '\r') {
		_errorUnexpectedChar();
	}
}

void Lexer::_appendAndNextChar()
{
	_token.str += _ch;
	nextChar();
}

void Lexer::_errorUnexpectedChar() {
	error("Unexpected character [{}]", _ch);
}

} // namespace