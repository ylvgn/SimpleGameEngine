#include <sge_core/base/UnitTest.h>
#include <sge_core/pointer/SPtr.h>
#include <sge_core/base/Object.h>

namespace sge {
	class Test_SharePtr : public UnitTestBase {
		
		struct MyClass : public Object {
			int a;
			bool b;
		};

	public:
		void test1() {
			SPtr<MyClass> p1(new MyClass());
			p1->a = 100;

			SGE_DUMP_VAR(p1->_refCount, p1->a);
		
			SPtr<MyClass> p2(p1.ptr());
			p2->a = 200;

			SGE_DUMP_VAR(p2->_refCount, p1->a);

			SPtr<MyClass> p3(p2.ptr());

			SGE_DUMP_VAR(p1->_refCount, p2->_refCount, p3->a);

			{
				p1->a = 300;
				p1 = nullptr;
			}
			
			SGE_DUMP_VAR(p2->_refCount, p3->_refCount, p2->a);
		}
	}; // Test_SharePtr

} // namespace

void test_SharePtr() {
	using namespace sge;
	SGE_TEST_CASE(Test_SharePtr, test1());
}