#include "WavefrontObjLoader.h"

namespace sge {

#if 0
#pragma mark ========= WavefrontObjLoader_Info ============
#endif
void WavefrontObjLoader_Info::clear() {
	v.clear();
	vt.clear();
	vn.clear();

	face_v.clear();
	face_vt.clear();
	face_vn.clear();
}

void WavefrontObjLoader_Info::clearFace() {
	face_v.clear();
	face_vt.clear();
	face_vn.clear();
}

#if 0
#pragma mark ========= WavefrontObjLoader ============
#endif
void WavefrontObjLoader::readFile(EditMesh& outInfo, StrView filename) {
	MemMapFile mm;
	mm.open(filename);
	readMem(outInfo, mm, filename);
}

void WavefrontObjLoader::readMem(EditMesh& outInfo, ByteSpan data, StrView filename) {
	WavefrontObjLoader inst;
	inst._readMem(outInfo, data, filename);
}

void WavefrontObjLoader::_readMem(EditMesh& outInfo, ByteSpan data, StrView filename) {
	outInfo.clear();
	_info.clear();

	_outInfo = &outInfo;
	reset(data, filename);

	for (;;) {
		while (_token.isNewline()) {
			if (!nextToken()) break;
		}

		if (_token.isNone()) break;

		if (_token.isIdentifier("v"))		_parseLine_v();
		else if (_token.isIdentifier("vt"))	_parseLine_vt();
		else if (_token.isIdentifier("vn"))	_parseLine_vn();
		else if (_token.isIdentifier("f"))	_parseLine_f();
		else if (!nextToken()) break;
	}
}

void WavefrontObjLoader::_nextTmpLine() {
	auto pair = _nextLine();
	nextToken();
	_tmpCurrentLine = pair.first;
	_tmpLineRemain = _tmpCurrentLine;
}

void WavefrontObjLoader::_nextTmpToken() {
	auto pair = StringUtil::splitByChar(_tmpLineRemain, " \t\r");
	_tmpToken = StringUtil::trimChar(pair.first, " \t\r");
	_tmpLineRemain = StringUtil::trimChar(pair.second, " \t\r");
}

void WavefrontObjLoader::_parseLine_v() {
	Tuple4f v(0,0,0,1);

	_nextTmpLine();
	for (int i = 0; i < 4; ++i) {
		_nextTmpToken();

		if (_tmpToken.empty()) {
			if (i < 3) error("_parseLine_v error: missing component\ncurrent source line: {}", _tmpCurrentLine);
			break;
		}

		if (!StringUtil::tryParse(_tmpToken, v[i]))
			error("_parseLine_v tryPase error:\ncurrent _tmpToken: {}", _tmpToken);
	}
	_info.v.emplace_back(v.x / v.w, v.y / v.w, v.z / v.w);
}

void WavefrontObjLoader::_parseLine_vt() {
	Tuple3f v(0,0,0);

	_nextTmpLine();
	for (int i = 0; i < 3; ++i) {
		_nextTmpToken();

		if (_tmpToken.empty()) {
			if (!i) error("_parseLine_vt error: missing u\ncurrent source line: {}", _tmpCurrentLine);
			break;
		}

		if (!StringUtil::tryParse(_tmpToken, v[i]))
			error("_parseLine_vt tryPase error:\ncurrent _tmpToken: {}", _tmpToken);
	}
	_info.vt.emplace_back(v);
}

void WavefrontObjLoader::_parseLine_vn() {
	Tuple3f v(0,0,0);

	_nextTmpLine();
	for (int i = 0; i < 3; ++i) {
		_nextTmpToken();

		if (_tmpToken.empty())
			error("_parseLine_vn error: missing u\ncurrent source line: {}", _tmpCurrentLine);

		if (!StringUtil::tryParse(_tmpToken, v[i]))
			error("_parseLine_vn tryPase error:\ncurrent _tmpToken: {}", _tmpToken);
	}
	_info.vn.emplace_back(v);
}

void WavefrontObjLoader::_parseLine_f() {
	_info.clearFace();

	_nextTmpLine();

	while (!_tmpLineRemain.empty()) {
		_nextTmpToken();

		if (_tmpToken.empty())
			break;

		int v, vt, vn;
		v = vt = vn = 0;

		// v
		auto pair = StringUtil::splitByChar(_tmpToken, "/");
		if (!StringUtil::tryParse(pair.first, v))
			error("_parseLine_f tryPase error v:\ncurrent _tmpToken: {}", _tmpToken);

		// vt
		pair = StringUtil::splitByChar(pair.second, "/");
		if (!pair.first.empty()) {
			if (!StringUtil::tryParse(pair.first, vt))
				error("_parseLine_f tryPase error vt:\ncurrent _tmpToken: {}", _tmpToken);
		}

		// vn
		pair = StringUtil::splitByChar(pair.second, "/");
		if (!pair.first.empty()) {
			if (!StringUtil::tryParse(pair.first, vn))
				error("_parseLine_f tryPase error vn:\ncurrent _tmpToken: {}", _tmpToken);
		}

		// ------
		if (v != 0) {
			if (v > 0) { --v; }
			else { v = static_cast<int>(_info.v.size()) + v; }
			_info.face_v.emplace_back(v);
		}

		if (vt != 0) {
			if (vt > 0) { --vt; }
			else { vt = static_cast<int>(_info.vt.size()) + vt; }
			_info.face_vt.emplace_back(vt);
		}

		if (vn != 0) {
			if (vn > 0) { --vn; }
			else { vn = static_cast<int>(_info.vn.size()) + vn; }
			_info.face_vn.emplace_back(vn);
		}
	}

	size_t newAddVertexCount = _info.face_v.size();
	if (newAddVertexCount < 3)
		error("_parseLine_f vertex count < 3, invalid triangle\ncurrent source line: {}", _tmpCurrentLine);

	size_t indicesStart = _outInfo->pos.size();

	bool isExist_vt = _info.face_vt.size() == newAddVertexCount;
	bool isExist_vn = _info.face_vn.size() == newAddVertexCount;

	if (!isExist_vt && _info.face_vt.size() > 0)
		error("_parseLine_f invalid vt:\ncurrent source line: {}", _tmpCurrentLine);

	if (!isExist_vn && _info.face_vn.size() > 0)
		error("_parseLine_f invalid vn:\ncurrent source line: {}", _tmpCurrentLine);

#if 0 // flip face front/back
	eastl::reverse(face_v.begin(), face_v.end());
	eastl::reverse(face_vt.begin(), face_vt.end());
	eastl::reverse(face_vn.begin(), face_vn.end());
#endif

	for (size_t i = 0; i < newAddVertexCount; ++i) {
		{ // v
			auto& arr		= _info.v;
			auto& faceArr	= _info.face_v;
			auto face_v		= faceArr[i];

			if (face_v < 0 || face_v >= arr.size())
				_error("_parseLine_f invalid v out of range");

			_outInfo->pos.emplace_back(arr[face_v]);
		}
		{ // vt
			auto& arr = _info.vt;
			auto& faceArr = _info.face_vt;
			if (isExist_vt) {
				auto face_vt = faceArr[i];

				if (face_vt < 0 || face_vt >= arr.size())
					_error("_parseLine_f invalid vt out of range");

				const auto& tmpUv = arr[face_vt];
				_outInfo->uv[0].emplace_back(tmpUv.x, tmpUv.y);
			} else {
				_outInfo->uv[0].emplace_back(0,0);
			}
		}
		{ // vn
			auto& arr = _info.vn;
			auto& faceArr = _info.face_vn;
			if (isExist_vn) {
				auto face_vn = faceArr[i];

				if (face_vn < 0 || face_vn >= arr.size())
					_error("_parseLine_f invalid vn out of range");

				_outInfo->normal.emplace_back(arr[face_vn]);
			} else {
				_outInfo->normal.emplace_back(0,0,0);
			}
		}
	}

	// ------
	using IndexType = decltype(_outInfo->indices)::value_type;

	for (size_t i = 0; i < newAddVertexCount; ++i) {
		if (i >= 3) {
			_outInfo->indices.emplace_back(static_cast<IndexType>(indicesStart /*+ 0*/));
			_outInfo->indices.emplace_back(static_cast<IndexType>(indicesStart + i - 1));
		}

		IndexType vi = static_cast<IndexType>(indicesStart + i);

		if (vi >= _outInfo->pos.size())
			_error("_parseLine_f invalid vi out of range");

		_outInfo->indices.emplace_back(vi);
	}
}

}