#include <fstream>
#include <sstream>
#include <clocale>
#include <Windows.h>
#include <codecvt>

#include <sge_sita_locale.h>

namespace sge {

class MyApp : public ConsoleApp {
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

	virtual void onRun() override {
#define	E(TestFunc) \
	do{ \
		SGE_LOG("\n[--TEST_CASE--] " #TestFunc "\n"); \
		TestFunc; \
	} while(false) \
//----
		E(test1());
		E(test2());
#undef E
	}

	void test1() {
		String filename(__FILE__);
		filename.append("_sample.txt");

		std::ifstream inFile(filename.c_str());
		std::stringstream strStream;
		strStream << inFile.rdbuf(); //read the file (utf-8 file)

		const StringA src = strStream.str().c_str();
//		dumpHex(src);
		SGE_DUMP_VAR(src);

		StringW w;
		StringA a;

		Utf::convert(w, src);
		SGE_DUMP_VAR(w);

		Utf::convert(a, w);
		SGE_DUMP_VAR(a);

		SGE_ASSERT(a == src);
	}

	void test2() {
		const char* sz8			= u8"A你";
		const wchar_t* szW		= L"A你";
		const char32_t* sz32	= U"A你";

		SGE_UNUSED(sz32);

		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convertor;
			auto s = convertor.to_bytes(szW);
			std::cout << s << "\n";
			SGE_ASSERT(0 == s.compare(sz8));
		}

		//mbstowcs - multi-byte string to wide char string
		{
			// Maps a UTF-16 (wide character) string to a new character string.
			// The new character string is not necessarily from a multi-byte character set.
			char buf[256];
			int n = WideCharToMultiByte(CP_UTF8, 0, szW, 2, buf, 256, nullptr, nullptr);
			buf[n] = 0;
			std::cout << buf << "\n";
		}
	}
};

}

int main(int argc, const char** argv) {
	sge::MyApp app;
	app.run();

	return 0;
}
