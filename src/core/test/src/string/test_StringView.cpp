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
			SGE_DUMP_VAR("yes");
		}

		if (f2() == f3()) {
			SGE_DUMP_VAR("yes");
		}
		
		if (f1() == f3()) {
			SGE_DUMP_VAR("yes");
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
};

} // namespace 

void test_StringView() {
	using namespace sge;
	SGE_TEST_CASE(Test_StringView, test_3());
}
