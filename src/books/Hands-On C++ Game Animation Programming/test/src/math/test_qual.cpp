#include <sge_core/base/UnitTest.h>

namespace sge {

class Test_qual : public UnitTestBase {
public:
	void test_0() {
		{
			quat q { 1,2,3,4 };
			SGE_ASSERT(q.x == q.data[0]);
			SGE_ASSERT(q.y == q.data[1]);
			SGE_ASSERT(q.z == q.data[2]);
			SGE_ASSERT(q.w == q.data[3]);
		}

		{
			quat q (1,2,3,4);
			SGE_ASSERT(q.x == q.data[0]);
			SGE_ASSERT(q.y == q.data[1]);
			SGE_ASSERT(q.z == q.data[2]);
			SGE_ASSERT(q.w == q.data[3]);
		}
	}

	void test_1() {
		quat q{ 1,2,3,4 };
		quat result = q.conjugate();
		quat expect{ -1,-2,-3,4 };
		SGE_ASSERT(expect == result);
	}

	void test_2() {
		// The inverse of a normalized quaternion is its conjugate.

		quat q{ 1,2,3,4 };
		q.normalized();
		quat result1 = q.inverse();
		quat result2 = q.conjugate();
		SGE_ASSERT(result1.equals(result2));
	}

	void test_3() {
		quat  q      = quat::s_angleAxis(Math::radians(45.f), vec3f::s_right());
		vec3f v      = vec3f::s_up();
		vec3f result = q * v;
		vec3f expect = vec3f(0, 1, 1).normalize();
		SGE_ASSERT(expect.equals(result));
	}

	void test_4() {
		{
			quat q{ 0,0,0,1 };
			mat4 result = mat4::s_quat(q);
			mat4 expect({ 1, 0, 0, 0 },
					    { 0, 1, 0, 0 },
					    { 0, 0, 1, 0 },
					    { 0, 0, 0, 1 });
			SGE_ASSERT(expect.equals(result));
		}
		
		{
			quat q{ 1,2,3,4 };
			mat4 result = mat4::s_quat(q);
			mat4 expect({-25,  28, -10, 0},
						{-20, -19,  20, 0},
						{ 22,  4,  -9,  0},
						{ 0,   0,   0,  1});
//			SGE_LOG("result={}", result);
			SGE_ASSERT(expect.equals(result));
		}
		 
		{
			quat q{ 2,2,5,7 };
			mat4 result = mat4::s_quat(q);
			mat4 expect({-57,  78, -8,  0 },
						{-62, -57,  48, 0 },
						{48,  -8,  -15, 0 },
						{0,    0,   0,  1 });
			SGE_ASSERT(expect.equals(result));
		}
	}
};

}

void test_qual() {
	using namespace sge;
	SGE_TEST_CASE(Test_qual, test_0());
	SGE_TEST_CASE(Test_qual, test_1());
	SGE_TEST_CASE(Test_qual, test_2());
	SGE_TEST_CASE(Test_qual, test_3());
	SGE_TEST_CASE(Test_qual, test_4());
}