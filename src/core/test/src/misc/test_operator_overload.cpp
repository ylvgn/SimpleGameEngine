#include <sge_core/base/UnitTest.h>

namespace sge {

	struct MyStruct {
		int x, y;

		// 写法1 (default public method)
		MyStruct operator+(const MyStruct& r) const {
			MyStruct o;
			o.x = x + r.x;
			o.y = y + r.y;
			return o;
		}
	};

#if 0 // 写法2
	MyStruct operator+(MyStruct& a, MyStruct& b) {
		MyStruct o;
		o.x = a.x + b.x;
		o.y = a.y + b.y;
		return o;
	}
#endif
	

	class MyClass
	{
		int x, y;
		// 写法3 (default private method) !!
		MyClass operator+(const MyClass& r) const {
			MyClass o;
			o.x = x + r.x;
			o.y = y + r.y;
			return o;
		}
	};

	class Test_Operator_Overload : public UnitTestBase {
	public:
		void referenceVSPointer()
		{
			int i = 10, j = 20;
			
			SGE_LOG("1: i={}", i);

			// pointer
			int* a = &i;
			*a = 100;
			SGE_LOG("2: i={}, a={}", i, *a);

			a = &j; // reassign pointer address
			SGE_LOG("3: i={}, j={}, a={}", i, j, *a);

			// reference
			int& b = i;
			b = 99;
			//b = &j; // Error!! reference is not allow reassign address
			SGE_LOG("4: i={}, b={}", i, b);
		}

		void main() {
			MyStruct a {1, 2};
			MyStruct b {3, 4};
			MyStruct c = a + b;
			SGE_LOG("c={}, {}", c.x, c.y);
		}
	};

} // namespace

void test_Operator_Overload() {
	using namespace sge;

	SGE_TEST_CASE(Test_Operator_Overload, referenceVSPointer());
	SGE_TEST_CASE(Test_Operator_Overload, main());
}