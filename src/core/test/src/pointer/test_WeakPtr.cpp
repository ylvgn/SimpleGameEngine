#include <sge_core/base/UnitTest.h>
#include <sge_core/pointer/WeakPtr.h>

namespace sge {

class Test_WeakPtr : public UnitTestBase {
public:

	class MyWindow;

	class MyButton : public RefCountBase {
	public:
		MyButton(StrView name_) : name(name_) {}

		void onFormat(fmt::format_context& ctx) const {
			String otherName;
			if (auto b = other.toSPtr()) {
				otherName.assign(b->name.data(), b->name.size());
			} else {
				otherName.assign("Null(Btn)");
			}

			otherName.append(", ");
			if (auto w = otherWin.toSPtr()) {
				otherName.append(w->name);
			}
			else {
				otherName.append("Null(Win)");
			}
			fmt::format_to(ctx.out(), "{}({}) with week: {}", name, fmt::ptr(this), otherName);
		}
		~MyButton() {
			SGE_LOG("~MyButton({:p}): {}", fmt::ptr(this), *this);
		}

		String name;

		WeakPtr<MyButton> other;
		WeakPtr<MyWindow> otherWin;
	};

	class MyWindow : public RefCountBase {
	public:
		void onFormat(fmt::format_context& ctx) const {
			fmt::format_to(ctx.out(), "\naBtn: {}\nbBtn: {}", *aBtn.ptr(), *bBtn.ptr());
		}
		~MyWindow() {
			SGE_LOG("~MyWindow({:p})", fmt::ptr(this));
		}

		String name;

		SPtr< MyButton > aBtn;
		SPtr< MyButton > bBtn;
	};

	void test_1() {

		SGE_LOG("test1 ============== Start");
		{
			MyWindow win;
			win.aBtn = new MyButton("ButtonA");
			win.bBtn = new MyButton("ButtonB");
			win.aBtn->other = win.bBtn;
			win.bBtn->other = win.aBtn;
			
			SGE_LOG("win:{:p}\naBtn:{:p}\nbBtn:{:p}\n", fmt::ptr(&win), fmt::ptr(win.aBtn.ptr()), fmt::ptr(win.bBtn.ptr()));

			SGE_DUMP_VAR(win);
		}
		SGE_LOG("test1 ============== End");
	}

	void test_2() {
		SGE_LOG("test2 ============== Start"); // UPtr and WeakPtr
		{
			auto aBtn = UPtr<MyButton>(new MyButton("ButtonA"));
			auto bBtn = UPtr<MyButton>(new MyButton("ButtonB"));
			SGE_LOG("aBtn:{:p}\nbBtn:{:p}\n", fmt::ptr(aBtn.get()), fmt::ptr(bBtn.get()));

			aBtn->other = aBtn.get();
			bBtn->other = aBtn.get();

			SGE_DUMP_VAR(*aBtn.get());
			SGE_DUMP_VAR(*bBtn.get());
		}
		SGE_LOG("test2 ============== End");
	}

	void test_3() {
		SGE_LOG("test3 ============== Start"); // SPtr and WeakPtr
		{
			auto aBtn = SPtr<MyButton>(new MyButton("ButtonA"));
			SGE_LOG("aBtn:{:p}\n", fmt::ptr(aBtn.ptr()));
			aBtn->other = aBtn.ptr();
			SGE_DUMP_VAR(*aBtn.ptr());
		}
		SGE_LOG("test3 ============== End");
	}

	void test_4() {
		SGE_LOG("test4 ============== Start"); // SPtr and WeakPtr
		{
			auto aBtn = SPtr<MyButton>(new MyButton("ButtonA"));
			auto bBtn = SPtr<MyButton>(new MyButton("ButtonB"));
			SGE_LOG("aBtn:{:p}\nbBtn:{:p}\n", fmt::ptr(aBtn.ptr()), fmt::ptr(bBtn.ptr()));

			aBtn->other = aBtn.ptr();
			bBtn->other = aBtn.ptr();

			SGE_DUMP_VAR(*aBtn.ptr());
			SGE_DUMP_VAR(*bBtn.ptr());
		}
		SGE_LOG("test4 ============== End");
	}

	void test_5() {
		SGE_LOG("test5 ============== Start"); // SPtr and WeakPtr
		{
			auto aBtn = SPtr<MyButton>(new MyButton("ButtonA"));
			auto bBtn = SPtr<MyButton>(new MyButton("ButtonB"));
			SGE_LOG("aBtn:{:p}\nbBtn:{:p}\n", fmt::ptr(aBtn.ptr()), fmt::ptr(bBtn.ptr()));

			aBtn->other = bBtn.ptr();
			bBtn->other = aBtn.ptr();

			SGE_DUMP_VAR(*aBtn.ptr());
			SGE_DUMP_VAR(*bBtn.ptr());
		}
		SGE_LOG("test5 ============== End");
	}

	void test_6() {
		bool a;
		SGE_LOG("{}", a);
		SGE_DUMP_VAR(a);

		SGE_LOG("test6 ============== Start");
		{
			SPtr<MyWindow> win = new MyWindow();
			WeakPtr<MyWindow> weakWin(win);

			win->aBtn = new MyButton("ButtonA");
			win->bBtn = new MyButton("ButtonB");
			win->aBtn->other = win->bBtn;
			win->bBtn->other = win->aBtn;

			SGE_LOG("win:{:p}\naBtn:{:p}\nbBtn:{:p}\n", fmt::ptr(win.ptr()), fmt::ptr(win->aBtn.ptr()), fmt::ptr(win->bBtn.ptr()));
			SGE_DUMP_VAR(*win.ptr());
		}
		SGE_LOG("test6 ============== End");
	}
};

SGE_FORMATTER(Test_WeakPtr::MyButton)
SGE_FORMATTER(Test_WeakPtr::MyWindow)

} // namespace 

void test_WeakPtr() {
	using namespace sge;
	SGE_TEST_CASE(Test_WeakPtr, test_1());
//	SGE_TEST_CASE(Test_WeakPtr, test_2()); // will occur error
	SGE_TEST_CASE(Test_WeakPtr, test_3());
	SGE_TEST_CASE(Test_WeakPtr, test_4());
	SGE_TEST_CASE(Test_WeakPtr, test_5());
	SGE_TEST_CASE(Test_WeakPtr, test_6());
}
