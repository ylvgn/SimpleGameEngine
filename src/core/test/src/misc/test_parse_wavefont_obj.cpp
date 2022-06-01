#include <sge_core/base/UnitTest.h>
#include <sge_core/file/MemMapFile.h>
#include <sge_core/file/Directory.h>
#include <sge_core/log/Log.h>
#include <sge_core/math/Tuple2.h>
#include <sge_core/math/Tuple3.h>
#include <sge_core/string/Lexer.h>

namespace sge {
// https://en.wikipedia.org/wiki/Wavefront_.obj_file

struct MyInfo {

	void _ListToStr(TempString& tmp, Vector<Tuple3f> list, const TempString& msg) const {
		tmp += "-------------------" + msg + "------------------\n";
		for (auto& item : list) {
			FmtTo(tmp, "{}:[({}, {}, {})]\n", msg, item.x, item.y, item.z);
		}
	}

	void _ListToStr(TempString& tmp, Vector<Tuple2f> list, const TempString& msg) const {
		tmp += "-------------------" + msg + "------------------\n";
		for (auto& item : list) {
			FmtTo(tmp, "{}:[({}, {})]\n", msg, item.x, item.y);
		}
	}

	Vector<Tuple3f> v;  // pos
	Vector<Tuple2f> vt; // uv
	Vector<Tuple3f> vn; // normal

	void clean() {
		v.clear();
		vt.clear();
		vn.clear();
	}

	void onFormat(fmt::format_context& ctx) const {
		TempString tmp;
#if 0
		tmp += "------------------- pos ------------------\n";
		for (auto& pos : v) {
			FmtTo(tmp, "pos:[({}, {}, {})]\n", pos.x, pos.y, pos.z);
		}

		tmp += "------------------- uv ------------------\n";
		for (auto& uv : vt) {
			FmtTo(tmp, "uv:[({}, {})]\n", uv.x, uv.y);
		}
#else // damn it !!
		TempString msg = "pos";
		_ListToStr(tmp, v, msg);

		msg = "uv";
		_ListToStr(tmp, vt, msg);

		msg = "vn";
		_ListToStr(tmp, vn, msg);
#endif
		fmt::format_to(ctx.out(), "{}", tmp);
	}

}; // MyInfo

class MyParser : public Lexer {
public:
	void MyParser::loadFile(MyInfo& outInfo, StrView filename) {
		MemMapFile mm;
		mm.open(filename);
		loadMem(outInfo, mm, filename);
	}

	void MyParser::loadMem(MyInfo& outInfo, ByteSpan data, StrView filename) {
		MyParser inst;
		inst._loadMem(outInfo, data, filename);
	}
private:
	MyInfo* _outInfo;

	void MyParser::_readPos() {
		nextToken(); // skip v
		auto& o = _outInfo->v.emplace_back(0, 0, 0);
		for (int i = 0; i < 3; i++) {
			float v;
			StringUtil::tryParse(_token.str, v);
			o.data[i] = v;
			nextToken();
		}
	}

	void MyParser::_readUv() {
		nextToken(); // skip vt
		auto& o = _outInfo->vt.emplace_back(0, 0);
		for (int i = 0; i < 2; i++) {
			float v;
			StringUtil::tryParse(_token.str, v);
			o.data[i] = v;
			nextToken();
		}
	}

	void MyParser::_readNormal() {
		nextToken(); // skip vn
		auto& o = _outInfo->vn.emplace_back(0, 0, 0);
		for (int i = 0; i < 3; i++) {
			float v;
			StringUtil::tryParse(_token.str, v);
			o.data[i] = v;
			nextToken();
		}
	}

	// str : v1/v2/v3 or v1 v2 v3
	void MyParser::__readFaceInnerWithChar(Tuple3<int>& out, StrView str, char splitSymbol = '/') {
		int index = 0;
		while (str.size()) {
			auto s = StringUtil::splitByChar(str, splitSymbol);
			auto& first = s.first;
			if (first.size()) {
				StringUtil::tryParse(first, out.data[index]);
			}
			str = s.second;
			index++;
		}
	}

	void MyParser::_readFace() {

		// 1. Vertex indices: f v1 v2 v3 ....
		// 2. Vertex texture coordinate indices: f v1/vt1 v2/vt2 v3/vt3 ...
		// 3. Vertex normal indices:
			// f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
			// f v1//vn1 v2//vn2 v3//vn3 ...

		const char* last = cur(); // start as ' '
		nextChar(); // skip f

		Tuple3<int> indexes = Tuple3<int>(0, 0, 0); // x:pos, y:uv, z:normal

		// readline
		while (nextChar()) {

			// read v1 v2 v3 or v1/v2/v3
			if (ch() == ' ' || ch() == '\n') {
				StrView str = StrView(last, cur() - last - 1);
				SGE_LOG("now:{}", str);
				indexes.x = 0;
				indexes.y = 0;
				indexes.z = 0;
				if (StringUtil::hasChar(str, '/')) {
					__readFaceInnerWithChar(indexes, str, '/'); // v1/v2/v3 ...
				} else {
					__readFaceInnerWithChar(indexes, str, ' '); // v1 v2 v3 ...
				}

				int pos = indexes.x;
				int uv = indexes.y;
				int nl = indexes.z;
				SGE_LOG("\t\tpos:{}, uv:{}, nl:{}, ch:{}", pos, uv, nl, static_cast<int>(ch()));
				last = cur(); // skip ' '
			}

			if (ch() == '\n') {
				break;
			}
		}
		nextToken(); // skip '\n'
	}

	void MyParser::_loadMem(MyInfo& outInfo, ByteSpan data, StrView filename) {
		outInfo.clean();
		_outInfo = &outInfo;
		
		reset(data, filename);
		for (;;) {
			if (_token.isIdentifier("v")) {
				_readPos();
				continue;
			}

			if (_token.isIdentifier("vt")) {
				_readUv();
				continue;
			}

			if (_token.isIdentifier("vn")) {
				_readNormal();
				continue;
			}
			if (_token.isIdentifier("f")) {
				_readFace();
				continue;
			}

			if (!nextToken()) {
				break;
			}
		}

		SGE_LOG("{}", _token);
	}
}; // MyParser

class Test_ParseWavefontObj : public UnitTestBase {

public:
	void main()
	{
		Directory::setCurrent("../../../../../examples/Test101/");
		//SGE_LOG("{}", Directory::getCurrent());

		MyInfo info;
		MyParser p;
		p.loadFile(info, "Assets/Mesh/test.obj");

		SGE_LOG("{}", info);
	}
}; // Test_ParseWavefontObj

} // namespace

SGE_FORMATTER(sge::MyInfo)

void test_ParseWavefontObj() {
	using namespace sge;
	SGE_TEST_CASE(Test_ParseWavefontObj, main());
}