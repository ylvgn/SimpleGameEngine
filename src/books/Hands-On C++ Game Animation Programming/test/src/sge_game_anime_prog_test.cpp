#include <sge_game_anime_prog.h>

// SGE_OPTIMIZE_OFF

#define	RUN_TEST( fn )	\
	SGE_LOG("\n===== " #fn " ================================\n"); \
	void fn(); \
	fn(); \
//----

namespace sge {

void run_temp_test() {
	RUN_TEST(test_mat4)
}

void run_all_test() {
// math
	RUN_TEST(test_mat4)
}

int test_main() {
#if 1
	run_temp_test();
#else
	run_all_test();
#endif
	// TODO: check memory leak

	SGE_LOG("\n\n==== Program Ended ==== \n");
	return 0;
}

} // namespace

int main() {
	return sge::test_main();
}
