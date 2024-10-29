#include <sge_core/base/UnitTest.h>
#include <sge_core/pointer/WPtr.h>

namespace sge {

class Test_WPtr_MyWindow;

class Test_WPtr_MyButton : public RefCountBase {
	using MyWindow = Test_WPtr_MyWindow;
	using MyButton = Test_WPtr_MyButton;
public:
	Test_WPtr_MyButton(StrView name_) : name(name_) { SGE_LOG("ctor {}({:p})",		name, fmt::ptr(this)); }
	~Test_WPtr_MyButton()							{ SGE_LOG("dtor {}({:p}):{}",	name, fmt::ptr(this), *this); }

	void onFormat(fmt::format_context& ctx) const;

	String name;

	WPtr<MyButton> other;
	WPtr<MyButton> other2;
	WPtr<MyWindow> otherWin;

	SPtr<MyButton> self;
};

#if 0
#pragma mark ========= Test_WPtr_MyWindow ============
#endif
class Test_WPtr_MyWindow : public RefCountBase {
	using MyButton = Test_WPtr_MyButton;
public:
	Test_WPtr_MyWindow(StrView name_) : name(name_) { SGE_LOG("ctor {}({:p})",		name, fmt::ptr(this)); }
	~Test_WPtr_MyWindow()							{ SGE_LOG("dtor {}({:p}):{}",	name, fmt::ptr(this), *this); }

	void onFormat(fmt::format_context& ctx) const;

	String name;
	SPtr<MyButton> aBtn;
	SPtr<MyButton> bBtn;
};

inline
void Test_WPtr_MyWindow::onFormat(fmt::format_context& ctx) const {
	if (aBtn && bBtn) {
		fmt::format_to(ctx.out(), "\naBtn: {}\nbBtn: {}", *aBtn.ptr(), *bBtn.ptr());
	}
	else if (aBtn) {
		fmt::format_to(ctx.out(), "\naBtn: {}\nbBtn: nullptr", *aBtn.ptr());
	}
	else if (bBtn) {
		fmt::format_to(ctx.out(), "\naBtn: nullptr\nbBtn:{}", *bBtn.ptr());
	}
}

inline
void Test_WPtr_MyButton::onFormat(fmt::format_context& ctx) const {
	String otherName;
	if (auto& b = other.toSPtr()) {
		FmtTo(otherName, "{}({:p})", b->name, fmt::ptr(b.ptr()));
	} else {
		otherName.assign("other(nullptr)");
	}

	otherName.append("\n\t\t");
	if (auto& b = other2.toSPtr()) {
		FmtTo(otherName, "{}({:p})", b->name, fmt::ptr(b.ptr()));
	} else {
		otherName.append("other2(nullptr)");
	}

	otherName.append("\n\t\t");
	if (auto& w = otherWin.toSPtr()) {
		FmtTo(otherName, "{}({:p})", w->name, fmt::ptr(w.ptr()));
	} else {
		otherName.append("otherWin(nullptr)");
	}

	TempString selfName = self != nullptr ? self->name : "self(nullptr)";

	fmt::format_to(ctx.out(), "{}({:p})\n\tWeak:\n\t\t{};\n\tSelf:\n\t\t{}", name, fmt::ptr(this), otherName, selfName);
}

SGE_FORMATTER(Test_WPtr_MyButton)
SGE_FORMATTER(Test_WPtr_MyWindow)


#if 0
#pragma mark ========= Test_WPtr ============
#endif
class Test_WPtr : public UnitTestBase {
	using MyWindow = Test_WPtr_MyWindow;
	using MyButton = Test_WPtr_MyButton;
public:

	void test_1() {
		SGE_LOG("test1 ============== Start");
		{
			MyWindow win("test1-MyWindow");
			win.aBtn = new MyButton("ButtonA");
			win.bBtn = new MyButton("ButtonB");
			win.aBtn->other = win.bBtn;
			win.bBtn->other = win.aBtn;
//			win.bBtn->otherWin = &win;	// not allow stack value with raw pointer
			win.bBtn->otherWin = new MyWindow("test1-other-Window"); // this line is ok

			SGE_LOG("win:{:p}\naBtn:{:p}\nbBtn:{:p}\n", fmt::ptr(&win), fmt::ptr(win.aBtn.ptr()), fmt::ptr(win.bBtn.ptr()));

			SGE_DUMP_VAR(win);
		}
		SGE_LOG("test1 ============== End");
	}

	void test_2() {
		SGE_LOG("test2 ============== Start"); // UPtr(EASTL) and WPtr (not allow)
		{
			auto aBtn = UPtr_make<MyButton>("ButtonA");
			auto bBtn = UPtr_make<MyButton>("ButtonB");
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
			auto aBtn = SPtr_make(new MyButton("ButtonA"));
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
			auto aBtn = SPtr_make(new MyButton("ButtonA"));
			auto bBtn = SPtr_make(new MyButton("ButtonB"));
			SGE_LOG("aBtn:{:p}\nbBtn:{:p}\n", fmt::ptr(aBtn.ptr()), fmt::ptr(bBtn.ptr()));

			aBtn->other = aBtn.ptr();
			bBtn->other = aBtn.ptr();

//			aBtn->self = aBtn; // not allow SPtr cycle reference!!!
			bBtn->self = aBtn; // this line is ok

			SGE_DUMP_VAR(*aBtn.ptr());
			SGE_DUMP_VAR(*bBtn.ptr());
		}
		SGE_LOG("test4 ============== End");
	}

	void test_5() {
		SGE_LOG("test5 ============== Start"); // SPtr and WPtr
		{
			auto aBtn = SPtr_make(new MyButton("ButtonA"));
			auto bBtn = SPtr_make(new MyButton("ButtonB"));
			SGE_LOG("aBtn:{:p}\nbBtn:{:p}\n", fmt::ptr(aBtn.ptr()), fmt::ptr(bBtn.ptr()));

			aBtn->other = bBtn.ptr();
			bBtn->other = aBtn.ptr();

			SGE_DUMP_VAR(*aBtn.ptr());
			SGE_DUMP_VAR(*bBtn.ptr());
		}
		SGE_LOG("test5 ============== End");
	}

	void test_6() {
		SGE_LOG("test6 ============== Start");
		auto other = SPtr_make(new MyWindow("test_6-other-MyWindow"));

		{
			auto win = SPtr_make(new MyWindow("test_6-MyWindow"));
			WPtr<MyWindow> weakWin(win);

			win->aBtn = new MyButton("ButtonA");
			win->bBtn = new MyButton("ButtonB");
			win->aBtn->other = win->bBtn;
			win->bBtn->other = win->aBtn;

			win->bBtn->otherWin = weakWin;
			win->aBtn->otherWin = other;

			SGE_LOG("win:{:p}\naBtn:{:p}\nbBtn:{:p}\n", fmt::ptr(win.ptr()), fmt::ptr(win->aBtn.ptr()), fmt::ptr(win->bBtn.ptr()));
			SGE_DUMP_VAR(*win.ptr());
		}
		SGE_LOG("test6 ============== End");
	}
};

} // namespace sge

void test_WPtr() {
	using namespace sge;
	SGE_TEST_CASE(Test_WPtr, test_1());
//	SGE_TEST_CASE(Test_WPtr, test_2()); // will occur error
	SGE_TEST_CASE(Test_WPtr, test_3());
	SGE_TEST_CASE(Test_WPtr, test_4());
	SGE_TEST_CASE(Test_WPtr, test_5());
	SGE_TEST_CASE(Test_WPtr, test_6());
}
