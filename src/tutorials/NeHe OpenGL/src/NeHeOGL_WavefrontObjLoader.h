#pragma once

#include "NeHeOGL_Mesh.h"

namespace sge {

class NeHeOGL_WavefrontObjLoader : public NonCopyable {
public:
	using Mesh = NeHeOGL_Mesh;

	static void loadFile(Mesh& outInfo, StrView filename);
	static void loadMem(Mesh& outInfo, ByteSpan data, StrView filename);
private:
	void _loadMem(Mesh& outInfo, ByteSpan data, StrView filename);

	void _nextLine();
	void _parseLine();
	void _nextToken();

	void _parseLine_o();

// vertex information
	void _parseLine_v();
	void _parseLine_vn();
	void _parseLine_vt();

// element statement
	void _parseLine_f();

	void _parseLine_s();

	void _error(StrView msg) {
		throw SGE_ERROR("{} line {}: {}", _filename, _lineNumber, msg);
	}

	StrView _source;
	StrView _sourceRemain;
	int		_lineNumber = 0;

	StrView _currentLine;
	StrView _lineRemain;
	String	_token;

	String	_filename;  // for error message

	Mesh*	_outInfo = nullptr;

	struct Info {
		String objectName;
		String smoothingGroup;

		Vector< Tuple3f > v;
		Vector< Tuple2f > vp;
		Vector< Tuple3f > vt;
		Vector< Tuple3f > vn;

		void clear();
	};

	Info _info;
};

}