#include <sge_core/base/UnitTest.h>

namespace sge {

class Test_StringView : public UnitTestBase {
public:	
	StrView f1() {
		return StrView();
	}

	StrView f2() {
		return "";
	}

	StrView f3() {
		return String();
	}

	void test_1() {
		if (f1() == f2()) {
			SGE_DUMP_VAR("f1 == f2"); // error
		}

		if (f2() == f3()) {
			SGE_DUMP_VAR("f2 == f3"); // error
		}

		if (f1() == f3()) {
			SGE_DUMP_VAR("f1 == f3"); // error
		}
	}

	void test_2() {
		String str = "hello \"world";

		for (auto& s : str) {
			if (s == '"') {
				SGE_LOG("1111111111");
			}

			if (s == '\"') {
				SGE_LOG("22222222");
			}
		}
	}

	void test_3() {
		
		String test = "f 1 2 3 4";
		
		for (int i = 0; i < test.size(); i += 2)
		{
			StrView _token(test.begin() + i, 1);

			SGE_LOG("token={}", _token);
			int vi = 0, vt = 0, vn = 0;

			auto pair = StringUtil::splitByChar(_token, '/');
			if (!StringUtil::tryParse(pair.first, vi)) {
				SGE_LOG("====vi====>{}", _token);
			}

			//vt
			pair = StringUtil::splitByChar(pair.second, '/');
			if (pair.first.size()) {
				if (!StringUtil::tryParse(pair.first, vt)) {
					SGE_LOG("====vt====>{}", _token);
				}
			}
			//vn
			pair = StringUtil::splitByChar(pair.second, '/');
			if (pair.first.size()) {
				if (!StringUtil::tryParse(pair.first, vn)) {
					SGE_LOG("====vn====>{}", _token);
				}
			}

			SGE_LOG("vi={} vt={} vn={}", vi, vt, vn);
		}
	}

	template<size_t N>	static void foo(String_<N>& s)	{ SGE_LOG("foo String_: {}", s); }
	template<size_t N>	static void foo(StringW_<N>& s) { wprintf(L"foo StringW_: %ws", s.c_str()); }
						static void foo(String& s)		{ SGE_LOG("foo String: {}", s); }
						static void foo(StringW& s)		{ wprintf(L"foo StringW: %ws", s.c_str()); }

	void test_4() {
		String s1 = "s1 - String";
		foo(s1);

		TempString s2 = "s2 - TempString";
		foo(s2);

		StringW s3 = L"s3 - StringW";
		foo(s3);

		String_<1> s4 = "s4 - String_1";
		foo(s4);

		StringW_<1> s5 = L"s5 - StringW_1";
		foo(s5);
	}

	void test_5() {
		char ch1[] = "char[]";
		StrView s1(ch1);
		SGE_LOG("{}, len={}", s1, s1.size());

		char ch2[20];
		ch2[0] = 'c';
		ch2[1] = 'h';
		ch2[2] = 'a';
		ch2[3] = 'r';
		ch2[4] = '[';
		ch2[5] = ']';
		ch2[6] = 0;

		StrView s2(ch2);
		SGE_LOG("{}, len={}", s2, s2.size());

		const char* sz = "string literal";
		StrView s3(sz);
		SGE_LOG("{}, len={}", s3, s3.size());

		std::string stdStr = "std::string";
		StrView s4(stdStr.c_str());
		SGE_LOG("{}, len={}", s4, s4.size());

		TempString tmpStr = "TempString";
		StrView s5(tmpStr);
		SGE_LOG("{}, len={}", s5, s5.size());
	}

	template<size_t N> inline
	static void foo2(const char (&ch)[N]) { SGE_LOG("{}, N={}", ch, N); }

	void test_6() {
		foo2("hello1");

		const char s2[] = "hello2";
		foo2(s2);

		const char s3[20] = "hello3";
		foo2(s3);
	}
};

} // namespace 

void test_StringView() {
	using namespace sge;
	//SGE_TEST_CASE(Test_StringView, test_1());
	//SGE_TEST_CASE(Test_StringView, test_2());
	//SGE_TEST_CASE(Test_StringView, test_3());
	//SGE_TEST_CASE(Test_StringView, test_4());
	//SGE_TEST_CASE(Test_StringView, test_5());
	SGE_TEST_CASE(Test_StringView, test_6());
}
