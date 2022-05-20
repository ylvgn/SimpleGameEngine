#include "lexer.h"

namespace sge {

	bool Lexer::moveNext() {
		return _cur != _source.end();
	}

	Token Lexer::getToken() {
		String value = "";
		TokenType tokenType = TokenType::None;
		auto cur = _cur;
		bool is_break = false;

		// 0: no comment
		// 1: //
		// 2: /*
		int isCommented = 0;
		while(cur != _source.end()) {

			if (is_break) break;

			// commented
			if (isCommented) {
				value += *cur;
				if (isCommented == 2) {
					if (*cur == '*') {
						cur++;
					}
					if (*cur == '/') { // "*/"  --> comment end
						value += *cur;
						isCommented = false;
						tokenType = TokenType::Comment;
						cur++;
						is_break = true;
						continue;
					}
				}
				else
				{
					if (*cur == '\n') { // "//"  --> comment end
						isCommented = false;
						tokenType = TokenType::Comment;
						is_break = true;
					}
				}
				cur++;
				continue;
			}

			// check string
			if (*cur == '\"') {
				SGE_ASSERT(value == "");
				value += *cur;
				cur++;
				while (cur != _source.end() && *cur != '\n' && *cur != '\"') {
					value += *cur;
					cur++;
				}

				// invalid
				if (cur == _source.end() || *cur == '\n') {
					tokenType = TokenType::None;
					is_break = true;
					break;
				}

				value += *cur; // add right quote "

				tokenType = TokenType::String;
				is_break = true;

				cur++; // skip "
				continue;
			}

			// check number
			if (*cur == '+' || *cur == '-' || *cur == '.' || (*cur >= '0' && *cur <= '9')) {
				SGE_ASSERT(value == "");
				int dot = 0;
				int e = 0;

				// 0 : no symbol
				// 1: -, + : 1 symbol
				int symbol = 0;

				if (ispunct(*cur)) { // check + - .
					if (*cur == '.') {
						dot = 1;
					}
					else if (*cur == '-') {
						symbol = 1;
						value += "-";
					}
					else if (*cur == '+') {
						symbol = 1;
						value += "+";
					}
					cur++; // skip + - .
				}

				// check '.'
				if (dot) {
					value += ".";

					// invalid: .-,  .+
					if (ispunct(*cur)) {
						value += *cur;
						tokenType = TokenType::None;
						is_break = true;
						break;
					}
				}

				// check 'e'
				if (!symbol && !dot) {
					if (*cur == 'e' || *cur == 'E') {
						e = 1;
						value += *cur;
						cur++; // skip 'e', 'E'
					}
				}
				else { // exist +- or . -> next must be number
					if (*cur < '0' && *cur > '9') {
						value += *cur;
						tokenType = TokenType::None;
						is_break = true;
						break;
					}
				}

				while (cur != _source.end() && *cur != '\n' && *cur != ' ') {
					// check 'e' 'E'
					if (*cur == 'e' || *cur == 'E') {
						e++;
					}

					value += *cur;
					cur++;

					if (ispunct(*cur)) {
						// check '.'
						if (!e && !dot && (*cur == '.')) {
							dot++;
							continue;
						}
						// check '+' '-'
						else if (e == 1 && (*cur == '+' || *cur == '-')) {
							auto last = *(cur - 1);
							// check 1e-num, 1e+num
							if (last == 'e' || last == 'E' && dot <= 1) {
								value += *cur;
								cur++;
								continue;
							}
						}
						break;
					}
				}

				// num + '.'x2 -> invalid
				if (*cur == '.') {
					value += '.';
					tokenType = TokenType::None;
					is_break = true;
					break;
				}

				// 1: num + ' ', num + '\n'
				// 2: num + '?'
				// 3: 1e9, 1e-9, 1ee9-(may be invalid number)
				tokenType = TokenType::Number;
				is_break = true;
				value.clear();
				continue;
			}

			// check comment
			if ((*cur == '/') && (cur + 1 != _source.end())) { // startwith '/'
				SGE_ASSERT(value == "");
				value += *cur;
				cur++;

				// check *(cur + 1)
				if (*cur == '/') {
					isCommented = 1; // '//'
				}
				else if (*cur == '*') {
					isCommented = 2; // '/*'
				}

				if (isCommented) { // exist comment?
					value += *cur;
					cur++;
					continue;
				}

				// unknown *cur now -> just insert single '/'
				tokenType = TokenType::Operator;
				is_break = true;
				continue;
			}

			// check identifier, startwith _ a~z  A~Z
			if (*cur == '_' || ((*cur) >= 'a' && (*cur) <= 'z') || ((*cur) >= 'A' && (*cur) <= 'Z')) {
				SGE_ASSERT(value == "");
				while (cur != _source.end() && *cur != '\n' && *cur != ' ' && !ispunct(*cur)) {
					value += *cur;
					cur++;
				}

				tokenType = TokenType::Identifier;
				is_break = true;
				continue;
			}

			// check operator
			if (ispunct(*cur)) {
				SGE_ASSERT(value == "");
				value = *cur;

				tokenType = TokenType::Operator;
				is_break = true;

				cur++;
				continue;
			}

			// check newline
			if (*cur == '\n' && !isCommented) {
				SGE_ASSERT(value == "");
				value = *cur;
				tokenType = TokenType::Newline;
				is_break = true;
				cur++;
				continue;
			}

			// check blank
			if (*cur == '\n' || *cur == ' ' || *cur == '\r' || *cur == '\t') {
				SGE_ASSERT(value == "");
				cur++;
				continue;
			}

			// check end
			if (cur == _source.end()) {
				SGE_ASSERT(value == "");
				break;
			}

			// unbelievable error
			SGE_LOG("failed cur={}", *cur);
			SGE_ASSERT(false);
		}

		_cur = cur;
		Token res = Token{ tokenType , value };
		SGE_LOG("{}", res.mylog());
		return res;
	}

} // namespace

