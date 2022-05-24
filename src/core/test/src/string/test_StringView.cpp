#include <sge_core/base/UnitTest.h>
#include <sge_core/string/Fmt.h>

namespace sge {

class Test_StringView : public UnitTestBase {
public:	
	StrView f1() {
		return StrView();
	}

	//StrView f2() {
	//	return "";
	//}

	StrView f3() {
		return String();
	}

	void test_stringview() {
		//if (StrView() == "") {
		//	SGE_DUMP_VAR("StrView() == \"\"");
		//}

		////if (f2() == f3()) {
		////	SGE_DUMP_VAR("f2 == f3");
		////}
		//
		//if (StrView() == String()) {
		//	SGE_DUMP_VAR("f1 == f3");
		//}
	}
};

} // namespace 

void test_StringView() {
	using namespace sge;
	SGE_TEST_CASE(Test_StringView, test_stringview());
}
