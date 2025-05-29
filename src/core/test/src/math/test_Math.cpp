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
		SGE_TEST_CHECK(0  == Math::nextPow16(0));
		SGE_TEST_CHECK(16 == Math::nextPow16(1));
		SGE_TEST_CHECK(16 == Math::nextPow16(15));
		SGE_TEST_CHECK(32 == Math::nextPow16(17));
		SGE_TEST_CHECK(32 == Math::nextPow16(32));
		SGE_TEST_CHECK(64 == Math::nextPow16(33));
	}

	void alignTo() {
		// signed int
		SGE_TEST_CHECK(0 == Math::alignTo(1, 0));
		SGE_TEST_CHECK(0	== Math::alignTo(0, 0));
		SGE_TEST_CHECK(0	== Math::alignTo(-1, 0));
		SGE_TEST_CHECK(2	== Math::alignTo(2, 1));
		SGE_TEST_CHECK(0	== Math::alignTo(0, 4));
		SGE_TEST_CHECK(0	== Math::alignTo(0, 7));
		SGE_TEST_CHECK(8	== Math::alignTo(2, 8));
		SGE_TEST_CHECK(12	== Math::alignTo(4, 12));
		SGE_TEST_CHECK(24	== Math::alignTo(16, 12));
		SGE_TEST_CHECK(64	== Math::alignTo(64, 32));
		SGE_TEST_CHECK(96	== Math::alignTo(65, 32));
		SGE_TEST_CHECK(128	== Math::alignTo(65, 64));
		SGE_TEST_CHECK(-128 == Math::alignTo(-65, -64));
		SGE_TEST_CHECK(-32	== Math::alignTo(-31, -32));

		// unsigned int
		SGE_TEST_CHECK(u8(8)	 == Math::alignTo(u8(8), u8(8)));
		SGE_TEST_CHECK(u8(8)	 == Math::alignTo(u8(8), u8(2)));
		SGE_TEST_CHECK(u8(17)	 == Math::alignTo(u8(8), u8(17)));
		SGE_TEST_CHECK(u8(34)	 == Math::alignTo(u8(18), u8(17)));
		SGE_TEST_CHECK(u8(8)	 == Math::alignTo(u8(8), u8(8)));
		SGE_TEST_CHECK(u32(200)  == Math::alignTo(u32(199), u32(200)));
		SGE_TEST_CHECK(u32(256)  == Math::alignTo(u32(199), u32(256)));
		SGE_TEST_CHECK(u32(512)  == Math::alignTo(u32(500), u32(256)));
		SGE_TEST_CHECK(u32(1025) == Math::alignTo(u32(1024), u32(1025)));
		SGE_TEST_CHECK(u32(1024) == Math::alignTo(u32(1024), u32(1024)));
	}
};

} // ----- end of namespace -----

void test_Math() {
	using namespace sge;
	SGE_TEST_CASE(Test_Math, nextPow2());
	SGE_TEST_CASE(Test_Math, nextPow16());
	SGE_TEST_CASE(Test_Math, alignTo());
}
