#include <sge_core/base/UnitTest.h>
#include <sge_core/string/UtfUtil.h>

#include <sge_core/base/Error.h>

namespace sge {

	enum class TokenType {
		None,
		Identifier, // x, color, UP
		Operator,   // {}
		Number,     // true, 6.02e23, 
		String,     // "music"
		Comment,    // // /**/
	};

	class Token
	{
	public:
		TokenType type;
		String value;

		Token() = default;
		Token(TokenType type_, StrView value_) {
			type = type_;
			value = value_;
		}

		String tokenTypeToStr() {
			String str;
			switch (type)
			{
			case sge::TokenType::None: str = "None"; break;
			case sge::TokenType::Identifier: str = "Identifier"; break;
			case sge::TokenType::Operator: str = "Operator"; break;
			case sge::TokenType::Number: str = "Number"; break;
			case sge::TokenType::String: str = "String"; break;
			case sge::TokenType::Comment: str = "Comment"; break;
			default: str = "Unknown"; break;
			}
			return str + ":\t" + value;
		}
	};

	class Test_Lexer : public UnitTestBase {
	public:
		void main();
		Vector<Token> getTokens(StrView str);
	}; // Test_Lexer

	void Test_Lexer::main() {

		const char* text = "Shader \"MyTest\" {\n"
			"	Properties\n"
			"{\n"
			"Color4f color = (1, 1, 1, 1) // aaa \n"
			"float f = .2f\n"
			"int i = -1e-5\n"
			"texture2D tex = \"white\" {}\n"
			"}\n"
			"\n"

			"/*ajdkflasdfasdf\n"
			"	fgadjflsfda\n"
			"		adfppwewer\n"
			"			adsfsdkkk\n"
			"			abadkfoadof\n"
			"sajfhaksdfh\n"
			"*/\n"

			"Pass\n"
			"{ // comment hhh \n"
			"VsFunc VShader\n"
			"PsFunc PShader\n"
			"}\n"
			"}";

		auto tokens = getTokens(text);
	}

	Vector<Token> Test_Lexer::getTokens(StrView str) {
		Vector<Token> res;

		// 0: no comment
		// 1: //
		// 2: /*
		int isCommented = 0;
		String value = "";
		for (auto cur = str.begin(); cur != str.end();) {

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

						auto item = Token(TokenType::Comment, value);
						res.push_back(item);
						//SGE_LOG("{}: {}", res.size(), value); 
						value.clear();
						cur++;
						continue;
					}
				}
				else
				{
					if (*cur == '\n') { // "//"  --> comment end
						isCommented = false;
						auto item = Token(TokenType::Comment, value);
						res.push_back(item);
						//SGE_LOG("{}: {}", res.size(), value); 
						value.clear();
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
				while (cur != str.end() && *cur != '\n' && *cur != '\"') {
					value += *cur;
					cur++;
				}

				// invalid
				if (cur == str.end() || *cur == '\n') {
					auto item = Token(TokenType::None, value);
					res.push_back(item);
					//SGE_LOG("{}: {}", res.size(), value); 
					value.clear();
					break;
				}

				value += *cur; // add right quote "
				auto item = Token(TokenType::String, value);
				res.push_back(item);
				//SGE_LOG("{}: {}", res.size(), value); 
				value.clear();
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
					} else if (*cur == '+') {
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
						auto item = Token(TokenType::None, value);
						res.push_back(item);
						//SGE_LOG("{}: {}", res.size(), value); 
						value.clear();
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
				} else { // exist +- or . -> next must be number
					if (*cur < '0' && *cur > '9') {
						value += *cur;
						auto item = Token(TokenType::None, value);
						res.push_back(item);
						//SGE_LOG("{}: {}", res.size(), value); 
						value.clear();
						break;
					}
				}

				while (cur != str.end() && *cur != '\n' && *cur != ' ') {
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
					auto item = Token(TokenType::None, value);
					res.push_back(item);
					//SGE_LOG("{}: {}", res.size(), value); 
					value.clear();
					break;
				}

				// 1: num + ' ', num + '\n'
				// 2: num + '?'
				// 3: 1e9, 1e-9, 1ee9-(may be invalid number)
				auto item = Token(TokenType::Number, value);
				res.push_back(item);
				//SGE_LOG("{}: {}", res.size(), value); 
				value.clear();
				continue;
			}

			// check comment
			if ((*cur == '/') && (cur + 1 != str.end())) { // startwith '/'
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
				auto item = Token(TokenType::Operator, value);
				res.push_back(item);
				//SGE_LOG("{}: {}", res.size(), value); 
				value.clear();
				continue;
			}

			// check identifier, startwith _ a~z  A~Z
			if (*cur == '_' || ((*cur) >= 'a' && (*cur) <= 'z') || ((*cur) >= 'A' && (*cur) <= 'Z')) {
				SGE_ASSERT(value == "");
				while (cur != str.end() && *cur != '\n' && *cur != ' ' && !ispunct(*cur)) {
					value += *cur;
					cur++;
				}

				auto item = Token(TokenType::Identifier, value);
				res.push_back(item);
				//SGE_LOG("{}: {}", res.size(), value); 
				value.clear();
				continue;
			}

			// check operator
			if (ispunct(*cur)) {
				SGE_ASSERT(value == "");
				value = *cur;
				auto item = Token(TokenType::Operator, value);
				res.push_back(item);
				//SGE_LOG("{}: {}", res.size(), value); 
				value.clear();
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
			if (cur == str.end()) {
				SGE_ASSERT(value == "");
				break;
			}

			// unbelievable
			SGE_LOG("failed cur={}", *cur);
			SGE_ASSERT(false);
		}

		SGE_LOG("\n---------------saved---------------\n");

		for (auto& item : res) {
			SGE_LOG("{}", item.tokenTypeToStr());
		}

		return res;
	}

} // namespace

void test_Lexer() {
	using namespace sge;

	SGE_TEST_CASE(Test_Lexer, main());
}