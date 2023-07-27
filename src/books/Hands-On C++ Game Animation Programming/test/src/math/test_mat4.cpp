#include <sge_core/base/UnitTest.h>

namespace sge {

class Test_mat4 : public UnitTestBase {
public:

	void test_operator1() {
		mat4f m1{2, 3, 2, 5,
				 1, 4, 6, 2,
				 3, 3, 5, 2,
				 4, 6, 2, 7};
		m1.transpose();

		mat4f m2{2, 3, 1, 2,
				 2, 3, 5, 3,
				 2, 3, 3, 1,
				 1, 1, 4, 3};
		m2.transpose();

		// test mat4f*mat4f
		mat4f m1mulm2{19, 26, 43, 30,
				      24, 35, 47, 26,
				      24, 35, 41, 26,
				      31, 43, 68, 49};
		m1mulm2.transpose();
		SGE_ASSERT(m1mulm2 == (m1 * m2));
	}

	void test_operator2() {
		mat4f m{2, 3, 2, 5,
			    1, 4, 6, 2,
			    3, 3, 5, 2,
			    4, 6, 2, 7};
		m.transpose();
		vec4f result{ 19,24,24,31 };
		SGE_ASSERT(result == (m * vec4f(2, 2, 2, 1)));
	}

	void test_inverse() {
		mat4f m{2, 3, 2, 5,
			    1, 4, 6, 2,
			    3, 3, 5, 2,
			    4, 6, 2, 7};
		mat4f inv = m.inverse();
		SGE_ASSERT(mat4f::s_identity().equals(m * inv, 0.000001f));
	}
};

}

void test_mat4() {
	using namespace sge;
	SGE_TEST_CASE(Test_mat4, test_operator1());
	SGE_TEST_CASE(Test_mat4, test_operator2());
	SGE_TEST_CASE(Test_mat4, test_inverse());
}