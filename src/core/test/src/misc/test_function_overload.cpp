#include <sge_core/base/UnitTest.h>

namespace sge {
	class Test_Function_Overload : public UnitTestBase {

		float func1(float a) {
			SGE_LOG("func1");
			return 0;
		}

		double func1(double b) {
			SGE_LOG("func2");
			return 0;
		}

		float func1(float a, float b) {
			SGE_LOG("func3");
			return 0;
		}

	public:
		void main()
		{
			float a = 0.5f;
			double b = 0.6;

			func1(a);         // func1
			func1(b);         // func2
			func1(0.6f, 0.8f);  // func3
		}
	};

} // namespace

void test_Function_Overload() {
	using namespace sge;
	SGE_TEST_CASE(Test_Function_Overload, main());
}