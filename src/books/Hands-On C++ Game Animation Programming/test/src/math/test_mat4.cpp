#include <sge_core/base/UnitTest.h>

namespace sge {

class Test_mat4 : public UnitTestBase {
public:
	void test_0() {
		mat4f a{2, 3, 2, 5,
				1, 4, 6, 2,
				3, 3, 5, 2,
				4, 6, 2, 7};
		a.transposed();

		mat4f b{2, 1, 3, 4,
				3, 4, 3, 6,
				2, 6, 5, 2,
				5, 2, 2, 7};

		SGE_ASSERT(a == b);
	}

	void test_1() {
		mat4f a{2, 3, 2, 5,
				1, 4, 6, 2,
				3, 3, 5, 2,
				4, 6, 2, 7};
		a.transposed();

		mat4f b{2, 3, 1, 2,
				2, 3, 5, 3,
				2, 3, 3, 1,
				1, 1, 4, 3};
		b.transposed();

		mat4f axb{19, 26, 43, 30,
				  24, 35, 47, 26,
				  24, 35, 41, 26,
				  31, 43, 68, 49};
		axb.transposed();
		SGE_ASSERT(axb == (a * b));
	}

	void test_2() {
		mat4f m{2, 3, 2, 5,
			    1, 4, 6, 2,
			    3, 3, 5, 2,
			    4, 6, 2, 7};
		m.transposed();
		vec4f ans{ 19,24,24,31 };
		SGE_ASSERT(ans == (m * vec4f(2, 2, 2, 1)));
	}

	void test_3() {
		mat4f m{2, 3, 2, 5,
			    1, 4, 6, 2,
			    3, 3, 5, 2,
			    4, 6, 2, 7};
		mat4f invM = m.inverse();
		mat4f mI = mat4f::s_identity();
		SGE_ASSERT(mI.equals(m * invM, 0.000001f));
	}
};

}

void test_mat4() {
	using namespace sge;
	SGE_TEST_CASE(Test_mat4, test_0());
	SGE_TEST_CASE(Test_mat4, test_1());
	SGE_TEST_CASE(Test_mat4, test_2());
	SGE_TEST_CASE(Test_mat4, test_3());
}