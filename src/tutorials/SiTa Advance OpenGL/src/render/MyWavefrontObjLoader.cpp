#include "MyWavefrontObjLoader.h"

namespace sge {

void MyWavefrontObjLoader::Info::clear() {
	objectName.clear();
	smoothingGroup.clear();

	v.clear();
	vt.clear();
	vn.clear();

	face_v.clear();
	face_vt.clear();
	face_vn.clear();
}

void MyWavefrontObjLoader::Info::clearFace() {
	face_v.clear();
	face_vt.clear();
	face_vn.clear();
}

void MyWavefrontObjLoader::loadFile(Mesh& outInfo, StrView filename) {
	MemMapFile mm;
	mm.open(filename);
	loadMem(outInfo, mm.span(), filename);
}

void MyWavefrontObjLoader::loadMem(Mesh& outInfo, ByteSpan data, StrView filename) {
	MyWavefrontObjLoader loader;
	loader._loadMem(outInfo, data, filename);
}

void MyWavefrontObjLoader::_loadMem(Mesh& outInfo, ByteSpan data, StrView filename) {
	outInfo.clear();
	_info.clear();

	_filename		= filename;
	_outInfo		= &outInfo;

	_source			= StrView_make(data);
	_sourceRemain	= _source;

	while (!_sourceRemain.empty()) {
		_nextLine();
		_parseLine();
	}
}

void MyWavefrontObjLoader::_nextLine() {
	auto pair		= StringUtil::getLine(_sourceRemain);
	_currentLine	= pair.first;
	_sourceRemain	= pair.second;
	_lineRemain		= _currentLine;

	++_lineNumber;
}

void MyWavefrontObjLoader::_nextToken() {
	auto pair	= StringUtil::splitByChar(_lineRemain, " \t\r");
	_token		= StringUtil::trimChar(pair.first, " \t\r");
	_lineRemain = StringUtil::trimChar(pair.second, " \t\r");
}

void MyWavefrontObjLoader::_parseLine() {
	_nextToken();

	if (_token.empty()) return;
	if (_token[0] == '#') return;

	if (_token == "v")		 _parseLine_v();
	else if (_token == "vt") _parseLine_vt();
	else if (_token == "vn") _parseLine_vn();
	else if (_token == "f")  _parseLine_f();
	else if (_token == "o")  _parseLine_o();
	else if (_token == "s")  _parseLine_s();
}

void MyWavefrontObjLoader::_parseLine_o() {
	_nextToken();
	if (_token.empty())
		return;
	_info.objectName = _token;
}

void MyWavefrontObjLoader::_parseLine_s() {
	_nextToken();
	if (_token.empty())
		return;
	_info.smoothingGroup = _token;
}

void MyWavefrontObjLoader::_parseLine_v() {
	Tuple4f v(0,0,0,1); // x y z w(1)

	for (int i = 0; i < 4; ++i) {
		_nextToken();
		if (_token.empty()) {
			if (i < 3) _error("_parseLine_v error: missing component");
			break;
		}

		if (!StringUtil::tryParse(_token, v[i])) {
			_error("_parseLine_v tryPase error");
		}
	}
	_info.v.emplace_back(v.x / v.w, v.y / v.w, v.z / v.w);
}

void MyWavefrontObjLoader::_parseLine_vt() {
	Tuple3f v(0,0,0); // u v(0) w(0)

	for (int i = 0; i < 3; ++i) {
		_nextToken();
		if (_token.empty()) {
			if (!i) _error("_parseLine_vt error: missing u");
			break;
		}
		if (!StringUtil::tryParse(_token, v[i])) {
			_error("_parseLine_vt tryPase error");
		}
	}
	_info.vt.emplace_back(v);
}

void MyWavefrontObjLoader::_parseLine_vn() {
	Tuple3f v(0,0,0);

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

void MyWavefrontObjLoader::_parseLine_f() {
	_info.clearFace();

	while (!_lineRemain.empty()) {
		_nextToken();

		if (_token.empty())
			break;

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

		// ------
		// index start from 1 and Negative values indicate relative vertex numbers
		{ // v
			auto& arr = _info.v;
			if (v < 0) _info.face_v.emplace_back(static_cast<int>(arr.size() + v));
			else if (v > 0) _info.face_v.emplace_back(v - 1);
		}
		{ // vt
			auto& arr = _info.vt;
			if (vt < 0) { _info.face_vt.emplace_back(static_cast<int>(arr.size() + vt)); }
			else if (vt > 0) { _info.face_vt.emplace_back(vt - 1); }
		}
		{ // vn
			auto& arr = _info.vn;
			if (vn < 0) _info.face_vn.emplace_back(static_cast<int>(arr.size() + vn));
			else if (vn > 0) _info.face_vn.emplace_back(vn - 1);
		}
	}

	size_t newAddVertexCount = _info.face_v.size();
	if (newAddVertexCount < 3)
		_error("_parseLine_f vertex count < 3 not a triangle");

	auto indicesStart = static_cast<u32>(_outInfo->pos.size());

	bool isExist_vt = _info.face_vt.size() == newAddVertexCount;
	bool isExist_vn = _info.face_vn.size() == newAddVertexCount;

	if (!isExist_vt && _info.face_vt.size() > 0)
		_error("_parseLine_f invalid .obj format: face vt");

	if (!isExist_vn && _info.face_vn.size() > 0)
		_error("_parseLine_f invalid .obj format: face vn");

	for (size_t i = 0; i < newAddVertexCount; i++) {
		{ // v
			auto& arr		= _info.v;
			auto& faceArr	= _info.face_v;
			auto face_v		= faceArr[i];
			if (face_v < 0 || face_v >= arr.size())
				_error("_parseLine_f face v out of range");
			_outInfo->pos.emplace_back(arr[face_v]);
		}
		{ // vt
			auto& arr = _info.vt;
			auto& faceArr = _info.face_vt;
			if (isExist_vt) {
				auto face_vt = faceArr[i];
				if (face_vt < 0 || face_vt >= arr.size())
					_error("_parseLine_f face vt out of range");
				const auto& tmpUv = arr[face_vt];
				_outInfo->uv[0].emplace_back(tmpUv.x, tmpUv.y);
			} else {
				_outInfo->uv[0].emplace_back(0, 0);
			}
		}
		{ // vn
			auto& arr = _info.vn;
			auto& faceArr = _info.face_vn;
			if (isExist_vn) {
				auto face_vn = faceArr[i];
				if (face_vn < 0 || face_vn >= arr.size())
					_error("_parseLine_f face vn out of range");
				_outInfo->normal.emplace_back(arr[face_vn]);
			} else {
				_outInfo->normal.emplace_back(0,0,0);
			}
		}
	}

	// ------
	using IndexType = decltype(_outInfo->index)::value_type;

	for (size_t i = 0; i < newAddVertexCount; i++) {
		if (i >= 3) {
			// FYI: https://www.reddit.com/r/opengl/comments/3t5fmu/understanding_wavefront_obj_model_format_face/
			_outInfo->index.emplace_back(static_cast<IndexType>(indicesStart/*+0*/));
			_outInfo->index.emplace_back(static_cast<IndexType>(indicesStart + i-1));
		}

		IndexType vi = static_cast<IndexType>(indicesStart + i);
		size_t currentVertexCount = _outInfo->pos.size();
		if (vi >= currentVertexCount) {
			throw SGE_ERROR("_parseLine_f vi out of range");
		}
		if (currentVertexCount > std::numeric_limits<IndexType>::max()) {
			_error("_parseLine_f too many vertex count");
		}
		_outInfo->index.emplace_back(vi);
	}
}

}