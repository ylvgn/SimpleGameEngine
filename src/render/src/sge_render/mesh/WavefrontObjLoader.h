#pragma once
#include "EditMesh.h"

namespace sge {

struct WavefrontObjLoader_Info {
	Vector< Tuple3f > v;
	Vector< Tuple3f > vt;
	Vector< Tuple3f > vn;

	Vector<int, 64> face_v;
	Vector<int, 64> face_vt;
	Vector<int, 64> face_vn;

	void clear();
	void clearFace();
};

class WavefrontObjLoader : public Lexer {
	using Base = Lexer;
	using Info = WavefrontObjLoader_Info;
public:
	static void readFile	(EditMesh& outInfo, StrView filename);
	static void readMem		(EditMesh& outInfo, ByteSpan data, StrView filename);
private:
	void _readMem(EditMesh& outInfo, ByteSpan data, StrView filename);

	void _nextTmpLine();
	void _nextTmpToken();

	void _parseLine_v();
	void _parseLine_vt();
	void _parseLine_vn();
	void _parseLine_f();

	StrView	_tmpToken;
	StrView	_tmpCurrentLine;
	StrView	_tmpLineRemain;

	EditMesh*	_outInfo = nullptr;

	Info		_info;
};

}

