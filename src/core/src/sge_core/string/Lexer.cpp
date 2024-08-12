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
	_filename	= filename;
	_source		= source;
	_cur		= _source.begin();

	_ch			= 0;
	_col		= 0;
	_line		= 1;

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

void Lexer::errorUnexpectedChar() {
	error("Unexpected character [{}]", _ch);
}

bool Lexer::nextToken() {
	if (!_nextToken()) return false;
	//SGE_DUMP_VAR(_line, _token);
	return true;
}

bool Lexer::nextLine() {
	_ch = 0;
	if (_cur == nullptr) return false;
	if (_cur >= _source.end()) return false;
	_nextLine();
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

void Lexer::expectOperator(StrView s) {
	if (!_token.isOperator(s)) {
		error("expected token {}", s);
		return;
	}
	nextToken();
}

void Lexer::expectNewline() {
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

void Lexer::readBool(bool& v) {
	if (!_token.isBool(_token.str)) {
		errorUnexpectedToken();
	}
	if (_token.str == "true") v = true;
	else v = false;
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
			if (c == ' ' ) { tmp += '-';	continue; }
			if (c == '\n') { tmp += c;		continue; }
			if (c == '\t') { tmp += "----";	continue; }
			tmp += '-';
			i++;
		}
		tmp += "^^^\n";
	}

	FmtTo(tmp, "  token={}\n  file={}:{}:{}\n", _token, _filename, _line, _col);
	throw SGE_ERROR("{}", tmp);
}

bool Lexer::_nextToken() {
	_token.setNone();

	for (;;) {
		trimSpaces();
		if (!_ch) return false;

		// check comment
		if (_ch == '#') {
			_parseCommentSingleLine();
			continue;
		}

		// check newline
		if (_ch == '\n') {
			_token.type = TokenType::Newline;
			_token.str += "<newline>";
			nextChar();
			return true;
		}

		// check comment
		if (_ch == '/') {
			nextChar(); // skip '/'

			if (_ch == '/') { // case '//'
				_parseCommentSingleLine();
				continue;
			}

			if (_ch == '*') { // case /**/
				_parseCommentBlock();
				continue;
			}

			_token.type = TokenType::Operator;
			_token.str = '/';
			return true;
		}

		// check string (double quote)
		if (_ch == '\"') {
			return _parseString();
		}

		// check number
		if (isDigit(_ch)) {
			return _parseNumber();
		}

#if 0 // check number with + or - !!<----- dont handle this internal
		if (_ch == '-' || _ch == '+') {
			char oldCh = _ch;
			nextChar(); // skip + -
			if (isDigit(_ch)) {
				_token.str = oldCh;
				return _parseNumber();
			}
			_token.type = TokenType::Operator;
			_token.str = oldCh;
			return true;
		}
#endif

		// check identifier
		if (_ch == '_' || isAlpha(_ch)) {
			return _parseIdentifier();
		}

		// default must be operator
		_token.type = TokenType::Operator;
		_appendAndNextChar();
		return true;
	}
}

std::pair<StrView, StrView> Lexer::_nextLine() {
	if (_cur == nullptr || _cur >= _source.end()) {
		return { StrView(), StrView() };
	}
	auto pair = StringUtil::splitByChar(_cur, "\n");
	_ch = *pair.first.end();
	_cur = pair.second.begin();
	if (_cur) {
		_line++;
		_col = 0;
	}
	else if (!pair.first.empty()) {
		_col = pair.first.length() - 1;
	}
	return pair;
}

// e.g. 1234.56e-78
bool Lexer::_parseNumber() {
	// _ch start as '0' ~ '9'
	_token.type = TokenType::Number;

	bool hasDot = false;
	bool hasE   = false;

	while (_ch) {
			
		// check dot
		if (_ch == '.') {
			if (hasDot || hasE) {
				errorUnexpectedChar();
				return false;
			}
			hasDot = true;
			_appendAndNextChar();
			continue;
		}
			
		// check e
		if (_ch == 'e' || _ch == 'E') {
			if (hasE) {
				errorUnexpectedChar();
				return false;
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
	return true;
}

void Lexer::_parseCommentSingleLine() {
	nextChar(); // _ch start as '/'

	for (;;) {
		if (!_ch) return;
		if (_ch == '\n') {
			nextChar(); // skip '\n'
			return;
		} else {
			nextChar();
		}
	}	
}

void Lexer::_parseCommentBlock() {
	nextChar(); // _ch start as '*'

	for (;;) {
		if (!_ch) return;
		if (_ch == '*') {
			nextChar(); // skip '*'
			if (_ch == '/') {
				nextChar(); // skip '/'
				return;
			}
		} else {
			nextChar();
		}
	}
}

bool Lexer::_parseString() {
	nextChar(); // _ch start as '\"'
	_token.type = TokenType::String;

	for (;;) {
		if (_ch == '\\') { // handle escape character
			nextChar();
			switch (_ch) { // case \\, \/, \"
				case '\\':
				case '/':
				case '\"':
					_token.str += _ch;
					nextChar(); // bypass
					break;
				case 'b': _token.str += '\b'; break;
				case 'f': _token.str += '\f'; break;
				case 'n': _token.str += '\n'; break;
				case 'r': _token.str += '\r'; break;
				case 't': _token.str += '\t'; break;
				default:
					error("Unknown escape character [{}]", _ch);
			}
		}
		else if (_ch == '\"') {
			nextChar(); // skip '\"'
			break;
		} else {
			_token.str += _ch;
			nextChar(); // bypass
		}
	}
	return true;
}

bool Lexer::_parseIdentifier() {
	// _ch start as '_' || isAlpha(_ch)
	_token.type = TokenType::Identifier;
	_appendAndNextChar();

	for (;;) {
		if (!_ch) break;
		if (isAlphaDigitUnderscore(_ch)) {
			_appendAndNextChar();
		} else {
			break;
		}
	}

	// check invalid, now _ch must be empty character
	if (!_ch && _ch != ' ' && _ch != '\t' && _ch != '\n' && _ch != '\r') {
		errorUnexpectedChar();
		return false;
	}
	return true;
}

void Lexer::_appendAndNextChar() {
	_token.str += _ch;
	nextChar();
}

} // namespace