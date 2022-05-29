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
};

} // namespace 

void test_StringView() {
	using namespace sge;
	SGE_TEST_CASE(Test_StringView, test_2());
}
