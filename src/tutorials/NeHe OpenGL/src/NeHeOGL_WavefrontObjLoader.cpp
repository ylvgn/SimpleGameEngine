#include "NeHeOGL_WavefrontObjLoader.h"

namespace sge {

void NeHeOGL_WavefrontObjLoader::Info::clear() {
	v.clear();
	vt.clear();
	vn.clear();
}

void NeHeOGL_WavefrontObjLoader::loadFile(Mesh& outInfo, StrView filename) {
	MemMapFile mm;
	mm.open(filename);
	loadMem(outInfo, mm.span(), filename);
}

void NeHeOGL_WavefrontObjLoader::loadMem(Mesh& outInfo, ByteSpan data, StrView filename) {
	NeHeOGL_WavefrontObjLoader loader;
	loader._loadMem(outInfo, data, filename);
}

void NeHeOGL_WavefrontObjLoader::_loadMem(Mesh& outInfo, ByteSpan data, StrView filename) {
	outInfo.clear();
	_info.clear();

	_outInfo		= &outInfo;
	_filename		= filename;

	_source			= StrView_make(data);
	_sourceRemain	= _source;

	while (!_sourceRemain.empty()) {
		_nextLine();
		if (_lineRemain.empty()) break;
		_parseLine();
	}
}

void NeHeOGL_WavefrontObjLoader::_nextLine() {
	auto pair		= StringUtil::splitByChar(_sourceRemain, "\n");
	_currentLine	= pair.first;
	_sourceRemain	= pair.second;
	_lineRemain		= _currentLine;
	++_lineNumber;
}

void NeHeOGL_WavefrontObjLoader::_nextToken() {
	auto pair	= StringUtil::splitByChar(_lineRemain, " \t\r");
	_token		= StringUtil::trimChar(pair.first, " \t\r");
	_lineRemain = StringUtil::trimChar(pair.second, " \t\r");
}

void NeHeOGL_WavefrontObjLoader::_parseLine() {
	_nextToken();

	if (_token.empty())
		return;

	if (_token[0] == '#') // skip comment
		return;

// ----
	if (_token == "o") return _parseLine_o();

// ---- vertex data
	// A right-hand coordinate system is used to specify the coordinate locations.
	if (_token == "v")  return _parseLine_v();
	if (_token == "vt") return _parseLine_vt();
	if (_token == "vn") return _parseLine_vn();

// ---- vertex data
	if (_token == "f") return _parseLine_f();

// ----
	if (_token == "s") return _parseLine_s();
}

void NeHeOGL_WavefrontObjLoader::_parseLine_o() {
	_nextToken();
	if (_token.empty())
		return;
	_info.objectName = _token;
}

void NeHeOGL_WavefrontObjLoader::_parseLine_s() {
	_nextToken();
	if (_token.empty())
		return;
	_info.smoothingGroup = _token;
}

void NeHeOGL_WavefrontObjLoader::_parseLine_v() {
// Polygonal and free-form geometry statement
	Tuple4f v(0,0,0,1); // x y z w(1)

	for (int i = 0; i < 4; ++i) {
		_nextToken();
		if (i < 3 && _token.empty())
			_error("_parseLine_v error: missing component");

		if (_token.empty())
			break;

		if (!StringUtil::tryParse(_token, v[i])) {
			_error("_parseLine_v tryPase error");
		}
	}
	_info.v.emplace_back(v.x / v.w, v.y / v.w, v.z / v.w);
}

void NeHeOGL_WavefrontObjLoader::_parseLine_vt() {
	Tuple3f v(0,0,0); // u v(0) w(0)

	for (int i = 0; i < 3; ++i) {
		_nextToken();
		if (_token.empty()) {
			if (i == 1) _error("_parseLine_vt error: missing x");
			break;
		}

		if (!StringUtil::tryParse(_token, v[i])) {
			_error("_parseLine_vt tryPase error");
		}
	}
	_info.vt.emplace_back(v);
}

void NeHeOGL_WavefrontObjLoader::_parseLine_vn() {
// Polygonal and free-form geometry statement
	Tuple3f v(0, 0, 0); // i, j, k

	for (int i = 0; i < 3; ++i) {
		_nextToken();
		if (_token.empty())
			_error("_parseLine_vn error: missing component");

		if (!StringUtil::tryParse(_token, v[i])) {
			_error("_parseLine_vn tryPase error");
		}
	}
	_info.vn.emplace_back(v.x, v.y, v.z);
}

void NeHeOGL_WavefrontObjLoader::_parseLine_f() {
// starting with 1. These reference numbers are used in element statements
	// The first reference number is the geometric vertex(v).
	// The second reference number is the texture vertex(vt). It follows the first slash.
	// The third reference number is the vertex normal(vn). It follows the second slash.

	// f v1/vt1/vn1	v2/vt2/vn2	...
	// f v1//vn1	v2//vn2		...
	// f v1/vt1		v2/vt2		...
	while (!_lineRemain.empty()) {
		_nextToken();
		if (_token.empty()) break;

		int v, vt, vn;
		v = vt = vn = 0;

		// v
		auto pair = StringUtil::splitByChar(_token, "/");
		if (!StringUtil::tryParse(pair.first, v)) {
			_error("_parseLine_f tryPase error v");
		}

		// vt
		pair = StringUtil::splitByChar(pair.second, "/");
		if (!pair.first.empty()) {
			if (!StringUtil::tryParse(pair.first, vt)) {
				_error("_parseLine_f tryPase error vt");
			}
		}

		// vn
		pair = StringUtil::splitByChar(pair.second, "/");
		if (!pair.first.empty()) {
			if (!StringUtil::tryParse(pair.first, vn)) {
				_error("_parseLine_f tryPase error vn");
			}
		}

		Mesh::VertexDataType vertex;
		vertex.pos.set(0,0,0);
		vertex.color.set(255, 255, 255, 255);
		vertex.uv.set(0,0);
		vertex.normal.set(0,0,0);

		{ // v
			auto& arr = _info.v;
			if (v < 0) vertex.pos.set(arr[arr.size() + v]);
			else if (v > 0) vertex.pos.set(arr[v - 1]);
		}

		{ // vt
			auto& arr = _info.vt;
			
			if (vt < 0) {
				auto& value = arr[arr.size()+vt];
				vertex.uv.set(value.x, value.y);
			}
			else if (vt > 0) {
				auto& value = arr[vt-1];
				vertex.uv.set(value.x, value.y);
			}
		}

		{ // vn
			auto& arr = _info.vn;
			if (vn < 0) vertex.normal.set(arr[arr.size() + vn]);
			else if (vn > 0) vertex.normal.set(arr[vn-1]);
		}

		using IndiceArrType = decltype(Mesh::indices);
		using IndexType		= IndiceArrType::value_type;

		if (_outInfo->vertexCount() > std::numeric_limits<IndexType>::max()) {
			_error("too many vertices count");
		}

		_outInfo->vertices.push_back(vertex);
		_outInfo->indices.push_back(static_cast<IndexType>(_outInfo->vertices.size() - 1));
	}
}

}