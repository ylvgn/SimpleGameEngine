#include <sge_core/base/UnitTest.h>
#include <sge_core/string/UtfUtil.h>
#include <sge_core/base/Error.h>

#include <nlohmann/json.hpp> // https://github.com/nlohmann/json

namespace sge {

	class Test_Json : public UnitTestBase {
	public:
		void main();
	}; // Test_Json

	void Test_Json::main() {
		using namespace nlohmann;
		// create an empty structure (null)
		json j;

		// add a number that is stored as double (note the implicit conversion of j to an object)
		j["pi"] = 3.141;

		// add a Boolean that is stored as bool
		j["happy"] = true;

		// add a string that is stored as std::string
		j["name"] = "Niels";

		// add another null object by passing nullptr
		j["nothing"] = nullptr;

		// add an object inside the object
		j["answer"]["everything"] = 42;

		// add an array that is stored as std::vector (using an initializer list)
		j["list"] = { 1, 0, 2 };

		// add another object (using an initializer list of pairs)
		j["object"] = { {"currency", "USD"}, {"value", 42.99} };

		// instead, you could also write (which looks very similar to the JSON above)
		json j2 = {
		  {"pi", 3.141},
		  {"happy", true},
		  {"name", "Niels"},
		  {"nothing", nullptr},
		  {"answer", {
			{"everything", 42}
		  }},
		  {"list", {1, 0, 2}},
		  {"object", {
			{"currency", "USD"},
			{"value", 42.99}
		  }}
		};

		std::string j1str = j.dump();
		std::string j2str = j2.dump();
		SGE_LOG("j=\n{}", j1str);
		SGE_LOG("j2=\n{}", j2str);
	}
} // namespace

void test_Json() {
	using namespace sge;

	SGE_TEST_CASE(Test_Json, main());
}