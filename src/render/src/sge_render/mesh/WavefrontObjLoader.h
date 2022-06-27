#pragma once
#include "EditMesh.h"

// wiki: https://en.wikipedia.org/wiki/Wavefront_.obj_file
// spec: https://www.martinreddy.net/gfx/3d/OBJ.spec
namespace sge {

class WavefrontObjLoader : public Lexer {
public:
	static void readFile(EditMesh& outInfo, StrView filename);
	static void readMem(EditMesh& outInfo, ByteSpan data, StrView filename);
private:
	void _readMem(EditMesh& outInfo, ByteSpan data, StrView filename);

	void _parseLine_v();
	void _parseLine_vt();
	void _parseLine_vn();

	void _parseLine_f();
	void _parseLine_f_inner(Tuple3<int>& out, StrView str, char splitSymbol);

	template<class T>
	void _readLineNumbers(T& out) {
		String str;
		int index = 0;
		while (_ch && _ch != '\n') {
			_readNumber(str);
			//SGE_DUMP_VAR(index, v);
			if (!StringUtil::tryParse(str, out.data[index++])) {
				_error("error _readLineNumbers");
			}
		}
	};
	void _readNumber(String& out);

	EditMesh* _outInfo = nullptr;
	Vector<Tuple3f> _tmpPos;
	Vector<Tuple2f> _tmpUv;
	Vector<Tuple3f> _tmpNormal;

}; // WavefrontObjLoader

} // namespace

