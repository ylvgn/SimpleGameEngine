#include <fstream>
#include <sstream>

#include <sge_sita_locale.h>

namespace sge {

class Test_Utf : public UnitTestBase {
public:	

	template<typename STRVIEW>
	void dumpHex(STRVIEW s) {
		auto p = s.begin();
		auto e = s.end();
		for (; p != e; ++p) {
			auto c = static_cast<u32>(*p);
			SGE_LOG("0x{:x}", c);
		}
	}

	void test() {
		String filename(__FILE__);
		filename.append("_sample.txt");

		std::ifstream inFile(filename.c_str());
		std::stringstream strStream;
		strStream << inFile.rdbuf(); //read the file( utf-8 file)

		const StringA src = strStream.str().c_str();
//		dumpHex(src);
		SGE_DUMP_VAR(src);

		StringW w;
		StringA a;

		Utf::decodeUtf8(w, src);
//		dumpHex(w);
		SGE_DUMP_VAR(w);

		Utf::encodeUtf8(a, w);
//		dumpHex(a);
		SGE_DUMP_VAR(a);

		SGE_ASSERT(a == src);
	}
};

} // namespace 

void test_Utf() {
	using namespace sge;
	SGE_TEST_CASE(Test_Utf, test());
}
