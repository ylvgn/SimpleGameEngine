#include <sge_core/base/UnitTest.h>
#include <sge_core/math/Math.h>

namespace sge {

class Test_Math : public UnitTestBase {
public:	
	void nextPow2() {
		SGE_TEST_CHECK(0 == Math::nextPow2(0));
		SGE_TEST_CHECK(1 == Math::nextPow2(1));
		SGE_TEST_CHECK(2 == Math::nextPow2(2));
		SGE_TEST_CHECK(4 == Math::nextPow2(3));
		SGE_TEST_CHECK(8 == Math::nextPow2(5));
	}

	void nextPow16() {
		SGE_TEST_CHECK(0 == Math::nextPow16(0));
		SGE_TEST_CHECK(16 == Math::nextPow16(1));
		SGE_TEST_CHECK(16 == Math::nextPow16(15));
		SGE_TEST_CHECK(32 == Math::nextPow16(17));
		SGE_TEST_CHECK(32 == Math::nextPow16(32));
		SGE_TEST_CHECK(64 == Math::nextPow16(33));
	}
};

} // ----- end of namespace -----

void test_Math() {
	using namespace sge;
	SGE_TEST_CASE(Test_Math, nextPow2());
	SGE_TEST_CASE(Test_Math, nextPow16());
}
