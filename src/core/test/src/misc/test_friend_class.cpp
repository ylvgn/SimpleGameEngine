#include <sge_core/base/UnitTest.h>

namespace sge {

class Test_Friend_Class : public UnitTestBase {

	class ASon; // forward declare

	class A {};

	class B {
	public:
		int _publicMember;

	friend class ASon;
	friend class A;
	protected:
		int _protectedMember;

	private:
		int _privateMember;
	};

	class ASon : public  A {
	public:
		B _b;

		void foo() {
			_b._publicMember = 1;
			_b._protectedMember = 1; // must friend class ASon;
		}
	};

public:
	void main()
	{
		ASon test;
		test._b._publicMember = 1;
		//test._b._protectedMember = 1; // Test_Friend_Class is not class B's friend
	}
}; // Test_Friend_Class

} // namespace

void test_Friend_Class() {
	using namespace sge;
	SGE_TEST_CASE(Test_Friend_Class, main());
}