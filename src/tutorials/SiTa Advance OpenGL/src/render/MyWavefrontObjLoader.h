#pragma once

#include "MyEditMesh.h"

namespace sge {

class MyWavefrontObjLoader : public NonCopyable {
	using Mesh = MyEditMesh;
public:
	static void loadFile(Mesh& outInfo, StrView filename);
	static void loadMem(Mesh& outInfo, ByteSpan data, StrView filename);
private:
	void _loadMem(Mesh& outInfo, ByteSpan data, StrView filename);
	void _parseLine();

	void _nextLine();
	void _nextToken();

	void _parseLine_o();
	void _parseLine_s();

	void _parseLine_v();
	void _parseLine_vn();
	void _parseLine_vt();

	void _parseLine_f();

	void _error(StrView msg) {
		throw SGE_ERROR("{} line {}: {}", _filename, _lineNumber, msg);
	}

	String	_filename;

	StrView _source;
	StrView _sourceRemain;
	int		_lineNumber = 0;

	StrView _currentLine;
	StrView _lineRemain;
	String	_token;

	Mesh*	_outInfo = nullptr;

	struct Info {
		String objectName;
		String smoothingGroup;

		Vector< Tuple3f > v;
		Vector< Tuple3f > vt;
		Vector< Tuple3f > vn;

		Vector<int, 64> face_v;
		Vector<int, 64> face_vt;
		Vector<int, 64> face_vn;

		void clear();
		void resetFace();
	};
	Info _info;
};

}