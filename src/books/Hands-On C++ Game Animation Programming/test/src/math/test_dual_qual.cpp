#include <sge_core/base/UnitTest.h>

namespace sge {

class Test_dual_qual : public UnitTestBase {
public:
	void test_0() {
		dual_quat dq{ 1,2,3,4, 5,6,7,8 };
		SGE_ASSERT(dq.data[0] == dq.x && dq.x == dq.real.x);
		SGE_ASSERT(dq.data[1] == dq.y && dq.y == dq.real.y);
		SGE_ASSERT(dq.data[2] == dq.z && dq.z == dq.real.z);
		SGE_ASSERT(dq.data[3] == dq.w && dq.w == dq.real.w);

		SGE_ASSERT(dq.data[4] == dq.dx && dq.dx == dq.dual.x);
		SGE_ASSERT(dq.data[5] == dq.dy && dq.dy == dq.dual.y);
		SGE_ASSERT(dq.data[6] == dq.dz && dq.dz == dq.dual.z);
		SGE_ASSERT(dq.data[7] == dq.dw && dq.dw == dq.dual.w);
	}

	void test_1() {
		dual_quat dq { 1,2,3,4, 5,6,7,8  };
		dual_quat ans{ 3,4,5,6, 7,8,9,10 };
		SGE_ASSERT(ans == dq+2.0f);

		dq += 2;
		SGE_ASSERT(ans == dq);
	}

	void test_2() {
		dual_quat dq { 1,2,3,4, 5,6,7,8 };
		dual_quat ans{ 2,4,6,8, 10,12,14,16 };

		SGE_ASSERT(ans == dq * 2.f);

		dq *= 2.f;
		SGE_ASSERT(ans == dq);
	}

	void test_3() {
		dual_quat a{ 1,2,3,4, 5,6,7,8 };
		dual_quat b{ 0,1,2,3, -1,-2,-3,-4 };
		dual_quat c{ 1,3,5,7, 4,4,4,4 };
		SGE_ASSERT(c == a + b);
	}

	void test_4() {
		Transform t1;
		t1.position = vec3f(1,2,3);
		t1.rotation = quat(1,2,2,4).normalize();

		dual_quat dq(t1.rotation, t1.position);
		Transform t2 = Transform::s_dual_quat(dq);

		SGE_ASSERT(t1.position.equals(t2.position));
		SGE_ASSERT(t1.rotation.equals(t2.rotation));

		vec3f  pos;
		quat4f rot;
		dq.toRotationTranslation(rot, pos);
		SGE_ASSERT(t1.position.equals(pos));
		SGE_ASSERT(t1.rotation.equals(rot));
	}
};

}

void test_dual_qual() {
	using namespace sge;
	SGE_TEST_CASE(Test_dual_qual, test_0());
	SGE_TEST_CASE(Test_dual_qual, test_1());
	SGE_TEST_CASE(Test_dual_qual, test_2());
	SGE_TEST_CASE(Test_dual_qual, test_3());
	SGE_TEST_CASE(Test_dual_qual, test_4());
}