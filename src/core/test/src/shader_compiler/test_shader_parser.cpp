#include <sge_core/base/UnitTest.h>
#include <sge_core/string/UtfUtil.h>

#include <sge_core/base/Error.h>
#include "../../../src/render/shader_compiler/src/sge_shader_compiler/ShaderParser.h"

namespace sge {

	class Test_ShaderParser : public UnitTestBase {
	public:
		void main();
	}; // Test_ShaderParser


	void Test_ShaderParser::main() {

		ShaderParserOuputInfo info;
		ShaderParser::loadFile(info, "test_shader.hlsl");
	}

} // namespace

void test_ShaderParser() {
	using namespace sge;

	SGE_LOG("{}", "AAA");
	//SGE_TEST_CASE(Test_ShaderParser, main());
}