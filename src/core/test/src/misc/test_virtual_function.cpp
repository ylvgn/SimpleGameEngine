#include <sge_core/base/UnitTest.h>

namespace sge {
	class Test_Virtual_Function : public UnitTestBase {
		struct A {
			virtual ~A() = default;
			virtual void foo() {
				SGE_LOG("A foo");
			};
		};

		struct B : public A {
			using Base = A;
		private:
			virtual void foo() override {
				Base::foo();
				SGE_LOG("B foo");
			}
		};

		struct C : public B {
			using Base = B;
			virtual void foo() override {
				Base::Base::foo();
				//Base::foo(); // inaccessible
				SGE_LOG("C foo");
			}
		};

	public:
		void main()
		{
			C c;
			c.foo();
		}
	};

} // namespace

void test_Virtual_Function() {
	using namespace sge;
	SGE_TEST_CASE(Test_Virtual_Function, main());
}