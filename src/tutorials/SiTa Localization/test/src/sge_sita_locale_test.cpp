#include <sge_sita_locale.h>

#define	RUN_TEST( fn )	\
	SGE_LOG("\n===== " #fn " ================================\n"); \
	void fn(); \
	fn(); \
// ------

namespace sge {

class SiTaLocaleTest : public ConsoleApp {
protected:
	virtual void onRun() override {
		#if 0
			_runTempTest();
		#else
			_runAllTest();
		#endif
	}

private:

	void _runTempTest() {
		RUN_TEST(test_Utf);
	}

	void _runAllTest() {
		RUN_TEST(test_Utf);
	}
};

}

int main(int argc, const char** argv) {
	sge::SiTaLocaleTest app;
	app.run();

	return 0;
}
