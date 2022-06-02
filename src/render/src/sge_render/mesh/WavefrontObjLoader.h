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

	void _readPos();
	void _readUv();
	void _readNormal();

	void _readFace();
	void __readFaceInnerWithChar(Tuple3<int>& out, StrView str, char splitSymbol);

	EditMesh* _outInfo = nullptr;

	Vector<Tuple3f> _tmpPos;
	Vector<Tuple2f> _tmpUv;
	Vector<Tuple3f> _tmpNormal;

	void __readPosValue(size_t index);
	void __readUvValue(size_t index);
	void __readNormalValue(size_t index);
	void __readIndices(size_t index);

	size_t __getRealIndex(size_t index, size_t size);

}; // WavefrontObjLoader

} // namespace

