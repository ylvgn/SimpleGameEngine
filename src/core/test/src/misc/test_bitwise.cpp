#include <sge_core/base/UnitTest.h>

namespace sge {
	class Test_BitWise : public UnitTestBase {
		struct MyStruct {
			int a;                   // 4 bytes (from 0)
			float b;                 // 4 bytes (from 4)
			bool c;                  // 1 bytes (from 8)
			char d;                  // 1 bytes (from 9)
			const char* e = nullptr; // 8 bytes(x64) (expect from 10, but from 16)
			int* f = nullptr;        // 8 bytes(x64) (from 16+8=24)
			char g;                  // 1 bytes (from 33)
			MyStruct* h = nullptr;   // 8 bytes(x64) (expect from 34, but from 40)
		};
	public:
		void test1()
		{
			u16 a = 258; // 1 00000010 (256 + 2)
			u8 b = static_cast<u8>(a >> 8); // 1
			u8 c = static_cast<u8>(a);      // 2
			SGE_DUMP_VAR(a, b, c);
		}

		void test2()
		{
			int a = 1; // 00000001
			u16 b = static_cast<u16>(a << 8); // 1 00000000 -> 256
			u16 c = b | 2;
			SGE_DUMP_VAR(a, b, c); // 1 00000010 -> 256 + 2 = 258
		}

		void test3() {
#define E(Member) \
			auto Member##1 = memberOffset(&MyStruct::Member); \
			auto Member##2 = offsetof(MyStruct, Member); \
			SGE_DUMP_VAR(Member##1, Member##2); \
			\
// ---------
			E(a)
			E(b)
			E(c)
			E(d)
			E(e)
			E(f)
			E(g)
			E(h)
#undef E
			size_t size_of = sizeof(MyStruct);
			SGE_DUMP_VAR(size_of);

			//SGE_DUMP_VAR(sizeof(u16));
			//SGE_DUMP_VAR(sizeof(float));
#if 0
			auto a1 = memberOffset(&MyStruct::a);
			auto a2 = offsetof(MyStruct, a);
			SGE_DUMP_VAR(a1, a2);

			auto b1 = memberOffset(&MyStruct::b);
			auto b2 = offsetof(MyStruct, b);
			SGE_DUMP_VAR(b1, b2);

			auto c1 = memberOffset(&MyStruct::c);
			auto c2 = offsetof(MyStruct, c);
			SGE_DUMP_VAR(c1, c2);

			auto d1 = memberOffset(&MyStruct::d);
			auto d2 = offsetof(MyStruct, d);
			SGE_DUMP_VAR(d1, d2);

			auto e1 = memberOffset(&MyStruct::e);
			auto e2 = offsetof(MyStruct, e);
			SGE_DUMP_VAR(e1, e2);

			auto f1 = memberOffset(&MyStruct::f);
			auto f2 = offsetof(MyStruct, f);
			SGE_DUMP_VAR(f1, f2);

			auto g1 = memberOffset(&MyStruct::g);
			auto g2 = offsetof(MyStruct, g);
			SGE_DUMP_VAR(g1, g2);

			auto h1 = memberOffset(&MyStruct::h);
			auto h2 = offsetof(MyStruct, h);
			SGE_DUMP_VAR(h1, h2);
#endif
		}

		void test4() {
			MyStruct* tmp = nullptr; // (MyStruct*)0;

			const auto& aa = reinterpret_cast<char const&>(tmp->a);
			intptr_t a = reinterpret_cast<intptr_t>(&aa);
			
			auto const& bb = reinterpret_cast<const char&>(tmp->b);
			intptr_t b = reinterpret_cast<intptr_t>(&bb);

			auto& cc = reinterpret_cast<char&>(tmp->c);
			intptr_t c = reinterpret_cast<intptr_t>(&cc);

			size_t d = reinterpret_cast<size_t>(&(tmp->d));
			intptr_t e = reinterpret_cast<intptr_t>(&(tmp->e));
			
			SGE_DUMP_VAR(a, b, c);
			SGE_DUMP_VAR(d, e);

			{
				float MyStruct::* attr = &MyStruct::b;
				float* offsetPtr = &(tmp->*attr);
				intptr_t offset = reinterpret_cast<intptr_t>(offsetPtr);
				SGE_DUMP_VAR(b, offset);
			}
		}
	}; // Test_BitWise

} // namespace

void test_BitWise() {
	using namespace sge;
	SGE_TEST_CASE(Test_BitWise, test1());
	SGE_TEST_CASE(Test_BitWise, test2());

	SGE_TEST_CASE(Test_BitWise, test3());
	SGE_TEST_CASE(Test_BitWise, test4());
}