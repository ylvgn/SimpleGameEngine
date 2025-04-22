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
	
// NOTE: _ch is current char value, *cur is next char pointer
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
//	SGE_DUMP_VAR(_line, _token);
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
		error("Expected operator token {}", s);
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
		return;
	}
	outputStr = _token.str;
	nextToken();
}

void Lexer::readString(String& outputStr) {
	if (!_token.isString()) {
		errorUnexpectedToken();
		return;
	}
	outputStr = _token.str;
	nextToken();
}

void Lexer::readBool(bool& v) {
	if (!_token.isIdentifier()) {
		errorUnexpectedToken();
		return;
	}

	if (_token.str == "true")
		v = true;
	else if (_token.str == "false")
		v = false;
	else {
		errorUnexpectedToken();
	}
	nextToken();
}

StrView Lexer::getLastFewLines(size_t lineCount) {
	if (!_cur)
		return StrView();

	auto n = lineCount;
	auto* start = _source.data();

	auto* p = _cur;
	for (; p >= start && n > 0; ++p) {
		if (*p == '\n') {
			--n;
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

		{ // check comment
			if (_ch == '#') {
				_parseCommentSingleLine();
				continue;
			}

			if (_ch == '/') {
				auto ch0 = _ch;
				if (nextChar()) { // skip '/'
					if (_ch == '/') {
						_parseCommentSingleLine();
						continue;
					}
					else if (_ch == '*') {
						_parseCommentBlock();
						continue;
					}
				}
				_token.type = TokenType::Operator;
				_token.str += ch0;
				return true;
			}
		}

		if (_ch == '\n') {
			_parseNewline();
			return true;
		}

		if (_ch == '_' || isAlpha(_ch)) {
			_parseIdentifier();
			return true;
		}

		if (isDigit(_ch)) {
			return _parseNumber();
		}

		if (_ch == '\"') {
			return _parseString();
		}

		if (_parseOperator()) {
			return true;
		}

		{
			SGE_ASSERT(false);
			errorUnexpectedChar();
		}

		return false;
	}
}

Pair<StrView, StrView> Lexer::_nextLine() {
	if (_cur == nullptr || _cur >= _source.end()) {
		return Pair_make<StrView>();
	}

	auto pair = StringUtil::getLine(_cur);
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

bool Lexer::_parseNumber() {
	_token.type = TokenType::Number;

	bool hasDot = false;
	bool hasE   = false;

	for(;;) {
		if (_ch == '.') {
			if (hasDot || hasE) {
				errorUnexpectedChar();
				return false;
			}
			hasDot = true;

			_token.str += _ch;
			if (!nextChar()) return true; // e.g. 1234.
			else continue;
		}

		// e.g. 1234.56e-78, 1234e+56
		if (_ch == 'e' || _ch == 'E') {
			if (hasE) {
				errorUnexpectedChar(); // e.g. 12e34e
				return false;
			}
			hasE = true;

			_token.str += _ch;
			if (!nextChar()) {
				errorUnexpectedChar(); // e.g. 1234e
				return false;
			}

			if (_ch == '+' || _ch == '-') {
				_token.str += _ch;
				if (!nextChar()) {
					errorUnexpectedChar(); // e.g. 1234e+
					return false;
				}
				if (isDigit(_ch)) {
					_token.str += _ch;
					if (!nextChar()) return true; // e.g. 1234e+1
					else continue;
				}
			} else {
				errorUnexpectedChar(); // e.g. 1234e?
				return false;
			}
		}

		if (isDigit(_ch)) {
			_token.str += _ch;
			if (!nextChar()) return true;
		} else {
			break;
		}
	}

	return true;
}

void Lexer::_parseCommentSingleLine() {
	nextChar(); // skip '/' or '#'

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

void Lexer::_parseNewline() {
	_token.type = TokenType::Newline;

	static const char* kNewlineStr = "<newline>";
	_token.str += kNewlineStr;
	nextChar();
}

bool Lexer::_parseCommentBlock() {
	nextChar(); // skip '*'

	for (;;) {
		if (!_ch) break;
		if (_ch == '*') {
//			auto ch0 = _ch;
			if (nextChar()) { // skip '*'
				if (_ch == '/') {
					nextChar(); // skip '/'
					return true;
				} else {
					nextChar();
				}
			} else {
				break;
			}
		}
		else {
			nextChar();
		}
	}
	return false;
}

bool Lexer::_parseString() {
	_token.type = TokenType::String;
	nextChar(); // skip '\"'

	for (;;) {
		if (_ch == '\\') { // handle escape character
			if (!nextChar()) {
				error("_parseString unexpected EOF");
				return false;
			}
			switch (_ch) { // case \\, \/, \"
				case '\\':
				case '/':
				case '\"':
					_token.str += _ch;
					nextChar();
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
			if (!nextChar()) {
				error("_parseString unexpected EOF");
				return false;
			}
		}
	}
	return true;
}

void Lexer::_parseIdentifier() {
	_token.type = TokenType::Identifier;

	do {
		_token.str += _ch;
		if (!nextChar())
			break;
	} while (isAlphaDigitUnderscore(_ch));
}

bool Lexer::_parseOperator() {
	_token.type = TokenType::Operator;

	if (_ch == '+' || _ch == '-' || _ch == '*' || _ch == '/' || _ch == '%') {
//		auto ch0 = _ch;
		_token.str += _ch;
		if (nextChar()) {
			if (_ch == '=') {
				_token.str += _ch;
				nextChar();
				return true;
			}
		}
		nextChar();
		return true;
	}

	static const wchar_t kOps[] = L"=[](){}<>`~!@#$%^&,.?";
	for (auto& c : kOps) {
		if (c == _ch) {
			_token.str += _ch;
			nextChar();
			return true;
		}
	}

	_token.setNone();
	return false;
}

}