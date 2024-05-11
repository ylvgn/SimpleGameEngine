#include <sge_core/base/UnitTest.h>
#include <sge_core/pointer/WPtr.h>

namespace sge {

class test_WPtr : public UnitTestBase {
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
			if (auto b = other2.toSPtr()) {
				otherName.append(b->name.data(), b->name.size());
			} else {
				otherName.append("Null(Btn)");
			}

			otherName.append(", ");
			if (auto w = otherWin.toSPtr()) {
				otherName.append(w->name);
			}
			else {
				otherName.append("Null(Win)");
			}

			String selfName;
			if (self) {
				selfName.assign(self->name);
			}
			else {
				selfName.assign("Null(self)");
			}

			fmt::format_to(ctx.out(), "{}({}) Weak: {}; Self: {}", name, fmt::ptr(this), otherName, selfName);
		}
		~MyButton() {
			SGE_LOG("~MyButton({:p}): {}", fmt::ptr(this), *this);
		}

		String name;

		WPtr<MyButton> other;
		WPtr<MyButton> other2;

		WPtr<MyWindow> otherWin;

		SPtr<MyButton> self;
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

		SPtr<MyButton> aBtn;
		SPtr<MyButton> bBtn;
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
		SGE_LOG("test2 ============== Start"); // UPtr and WPtr
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
		SGE_LOG("test3 ============== Start"); // SPtr and WPtr
		{
			auto aBtn = SPtr<MyButton>(new MyButton("ButtonA"));
			SGE_LOG("aBtn:{:p}\n", fmt::ptr(aBtn.ptr()));
			aBtn->other = aBtn.ptr();
			aBtn->other2 = aBtn.ptr();
			
			aBtn->other = nullptr;
			aBtn->other = aBtn->other2;
 
			SGE_DUMP_VAR(*aBtn.ptr());
		}
		SGE_LOG("test3 ============== End");
	}

	void test_4() {
		SGE_LOG("test4 ============== Start"); // SPtr and WPtr
		{
			auto aBtn = SPtr<MyButton>(new MyButton("ButtonA"));
			auto bBtn = SPtr<MyButton>(new MyButton("ButtonB"));
			SGE_LOG("aBtn:{:p}\nbBtn:{:p}\n", fmt::ptr(aBtn.ptr()), fmt::ptr(bBtn.ptr()));

			aBtn->other = aBtn.ptr();
			bBtn->other = aBtn.ptr();

//			aBtn->self = aBtn; // cycle reference not allow!!!
			bBtn->self = aBtn; // this is ok

			SGE_DUMP_VAR(*aBtn.ptr());
			SGE_DUMP_VAR(*bBtn.ptr());
		}
		SGE_LOG("test4 ============== End");
	}

	void test_5() {
		SGE_LOG("test5 ============== Start"); // SPtr and WPtr
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
			WPtr<MyWindow> weakWin(win);

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

SGE_FORMATTER(test_WPtr::MyButton)
SGE_FORMATTER(test_WPtr::MyWindow)

} // namespace 

void test_WeakPtr() {
	using namespace sge;
//	SGE_TEST_CASE(test_WPtr, test_1());
//	SGE_TEST_CASE(test_WPtr, test_2()); // will occur error
	SGE_TEST_CASE(test_WPtr, test_3());
//	SGE_TEST_CASE(test_WPtr, test_4());
//	SGE_TEST_CASE(test_WPtr, test_5());
//	SGE_TEST_CASE(test_WPtr, test_6());
}
