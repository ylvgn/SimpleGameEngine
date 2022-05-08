#include <sge_core/base/UnitTest.h>

namespace sge {
	class Test_Struct_Declare : public UnitTestBase {

		// MyStruct1 ---------------------------
		struct MyStruct1 {
			int x, y;
		};

		struct MyStruct1 add(struct MyStruct1* a, struct  MyStruct1* b) {
			struct MyStruct1 o;
			o.x = a->x + b->x;
			o.y = a->y + b->y;
			return o;
		};

		// MyStruct1 ---------------------------

		// MyStruct2 ---------------------------
		typedef struct MyStruct2 {
			int x, y;
		} MyStruct2, *pMyStruct2;

		MyStruct2 add(pMyStruct2 a, pMyStruct2 b) { // pMyStruct2 --> MyStruct2*
			MyStruct2 o;
			o.x = a->x + b->x;
			o.y = a->y + b->y;
			return o;
		};
		// MyStruct2 ---------------------------

		// MyStruct3 ---------------------------
		typedef struct {
			int x, y;
		} MyStruct3;

		MyStruct3 add(MyStruct3* a, MyStruct3* b) {
			MyStruct3 o;
			o.x = a->x + b->x;
			o.y = a->y + b->y;
			return o;
		};
		// MyStruct3 ---------------------------

		// MyStruct4 ---------------------------
		struct MyStruct4 {
			int x, y;
		};

		MyStruct4 add(MyStruct4* a, MyStruct4* b) {
			MyStruct4 o;
			o.x = a->x + b->x;
			o.y = a->y + b->y;
			return o;
		};
		// MyStruct4 ---------------------------

	public:
		void main()
		{
			struct MyStruct1 t1;
			t1.x = 1;
			t1.y = 2;

			MyStruct2 t2;
			t2.x = 1;
			t2.y = 2;

			MyStruct3 t3;
			t3.x = 1;
			t3.y = 2;

			MyStruct4 t4;
			t4.x = 1;
			t4.y = 2;

			struct MyStruct1 ret1 = add(&t1, &t1);
			MyStruct2 ret2 = add(&t2, &t2);
			MyStruct3 ret3 = add(&t3, &t3);
			MyStruct4 ret4 = add(&t4, &t4);

			SGE_LOG("ret1 = {},{}", ret1.x, ret1.y);
			SGE_LOG("ret2 = {},{}", ret2.x, ret2.y);
			SGE_LOG("ret3 = {},{}", ret3.x, ret3.y);
			SGE_LOG("ret4 = {},{}", ret4.x, ret4.y);
		}
	};

} // namespace

void test_Struct_Declare() {
	using namespace sge;
	SGE_TEST_CASE(Test_Struct_Declare, main());
}