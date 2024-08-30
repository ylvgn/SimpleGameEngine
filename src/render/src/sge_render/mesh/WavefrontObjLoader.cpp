#include "WavefrontObjLoader.h"

namespace sge {

void WavefrontObjLoader::Info::clear() {
	v.clear();
	vt.clear();
	vn.clear();
}

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
			if (i < 3) error("_parseLine_v error: missing component\ncur source line: {}", _tmpCurrentLine);
			break;
		}

		if (!StringUtil::tryParse(_tmpToken, v[i]))
			error("_parseLine_v tryPase error:\ncur _tmpToken: {}", _tmpToken);
	}
	_info.v.emplace_back(v.x / v.w, v.y / v.w, v.z / v.w);
}

void WavefrontObjLoader::_parseLine_vt() {
	Tuple3f v(0,0,0);

	_nextTmpLine();
	for (int i = 0; i < 3; ++i) {
		_nextTmpToken();

		if (_tmpToken.empty()) {
			if (!i) error("_parseLine_vt error: missing u\ncur source line: {}", _tmpCurrentLine);
			break;
		}

		if (!StringUtil::tryParse(_tmpToken, v[i]))
			error("_parseLine_vt tryPase error:\ncur _tmpToken: {}", _tmpToken);
	}
	_info.vt.emplace_back(v);
}

void WavefrontObjLoader::_parseLine_vn() {
	Tuple3f v(0,0,0);

	_nextTmpLine();
	for (int i = 0; i < 3; ++i) {
		_nextTmpToken();

		if (_tmpToken.empty())
			error("_parseLine_vn error: missing u\ncur source line: {}", _tmpCurrentLine);

		if (!StringUtil::tryParse(_tmpToken, v[i]))
			error("_parseLine_vn tryPase error:\ncur _tmpToken: {}", _tmpToken);
	}
	_info.vn.emplace_back(v);
}

void WavefrontObjLoader::_parseLine_f() {
	_face_v.clear();
	_face_vt.clear();
	_face_vn.clear();

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
			error("_parseLine_f tryPase error v:\ncur _tmpToken: {}", _tmpToken);

		// vt
		pair = StringUtil::splitByChar(pair.second, "/");
		if (!pair.first.empty()) {
			if (!StringUtil::tryParse(pair.first, vt))
				error("_parseLine_f tryPase error vt:\ncur _tmpToken: {}", _tmpToken);
		}

		// vn
		pair = StringUtil::splitByChar(pair.second, "/");
		if (!pair.first.empty()) {
			if (!StringUtil::tryParse(pair.first, vn))
				error("_parseLine_f tryPase error vn:\ncur _tmpToken: {}", _tmpToken);
		}

		// ------
		if (v != 0) {
			if (v > 0) { --v; }
			else { v = static_cast<int>(_info.v.size()) + v; }
			_face_v.emplace_back(v);
		}

		if (vt != 0) {
			if (vt > 0) { --vt; }
			else { vt = static_cast<int>(_info.vt.size()) + vt; }
			_face_vt.emplace_back(vt);
		}

		if (vn != 0) {
			if (vn > 0) { --vn; }
			else { vn = static_cast<int>(_info.vn.size()) + vn; }
			_face_vn.emplace_back(vn);
		}
	}

	size_t newAddVertexCount = _face_v.size();
	if (newAddVertexCount < 3)
		error("_parseLine_f vertex count < 3, invalid triangle\ncur source line: {}", _tmpCurrentLine);

	size_t startVertexIndex = _outInfo->pos.size();

	bool isExist_vt = _face_vt.size() == newAddVertexCount;
	bool isExist_vn = _face_vn.size() == newAddVertexCount;
	if (!isExist_vt && _face_vt.size() > 0)
		error("_parseLine_f invalid vt:\ncur source line: {}", _tmpCurrentLine);
	if (!isExist_vn && _face_vn.size() > 0)
		error("_parseLine_f invalid vn:\ncur source line: {}", _tmpCurrentLine);

#if 0 // flip face front/back
	eastl::reverse(_face_v.begin(), _face_v.end());
	eastl::reverse(_face_vt.begin(), _face_vt.end());
	eastl::reverse(_face_vn.begin(), _face_vn.end());
#endif

	for (size_t i = 0; i < newAddVertexCount; ++i) {
		{ // face_v
			auto& arr = _info.v;
			auto vi	  = _face_v[i];

			if (vi < 0 || vi >= arr.size())
				_error("out of range: invalid face v");

			_outInfo->pos.emplace_back(arr[vi]);
		}
		{ // face_vt
			auto& arr = _info.vt;
			if (!isExist_vt) {
				_outInfo->uv[0].emplace_back(0,0);
			} else {
				auto vti = _face_vt[i];

				if (vti < 0 || vti >= arr.size())
					_error("out of range: invalid face vt");

				const auto& tmpUv = arr[vti];
				_outInfo->uv[0].emplace_back(tmpUv.x, tmpUv.y);
			}
		}
		{ // face_vn
			auto& arr = _info.vn;
			if (!isExist_vn) {
				_outInfo->normal.emplace_back(0,0,0);
			} else {
				auto vni = _face_vn[i];

				if (vni < 0 || vni >= arr.size())
					_error("out of range: invalid face vn");

				_outInfo->normal.emplace_back(arr[vni]);
			}
		}
	}

	// ------
	using IndexType = decltype(_outInfo->indices)::value_type;

	for (size_t i = 0; i < newAddVertexCount; ++i) {
		if (i >= 3) {
			_outInfo->indices.emplace_back(static_cast<IndexType>(startVertexIndex /*+ 0*/));
			_outInfo->indices.emplace_back(static_cast<IndexType>(startVertexIndex + i - 1));
		}

		IndexType vertexIndex = static_cast<IndexType>(startVertexIndex + i);

		if (vertexIndex >= _outInfo->pos.size())
			_error("out of range: _parseLine_f invalid vertex index");

		_outInfo->indices.emplace_back(vertexIndex);
	}
}

}