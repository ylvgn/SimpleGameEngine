#include <sge_core/base/UnitTest.h>
#include "MyShape.h"

namespace sge {

class Test_Reflection : public UnitTestBase {
	void foo(MyShape& shape) {
		if (const auto* ti = my_typeof(shape)) {
			SGE_DUMP_VAR(*ti);

			TempString name = ti->name;
			if (name == "MyCircle") {
				MyCircle* c = my_cast<MyCircle>(&shape);
				SGE_DUMP_VAR(c->r, c->testVectorInt, c->testVectorStr);

				for (auto& item : c->testVectorVectorInt) {
					SGE_DUMP_VAR(item);
				}
			}
		}
	}

public:
	void main()
	{
		MyShape s;
		foo(s);
		s.x = 1;
		s.y = 2;
		SGE_DUMP_VAR("=====================");

		MyCircle c;
		c.r = 1.2f;
		c.testVectorInt.emplace_back(1);
		c.testVectorInt.emplace_back(22);

		c.testVectorStr.emplace_back("hello");
		c.testVectorStr.emplace_back("world");

		c.testVectorVectorInt.push_back({ 1 , 2, 3 });
		c.testVectorVectorInt.push_back({ 11 , 22, 33 });
		foo(c);
		SGE_DUMP_VAR("=====================");

		MyRect r;
		r.w = 10;
		r.h = 200;
		foo(r);
	}
};

} // namespace

void test_Reflection() {
	using namespace sge;
	SGE_TEST_CASE(Test_Reflection, main());
}