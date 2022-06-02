#include "WavefrontObjLoader.h"

namespace sge {

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
}

void WavefrontObjLoader::_readPos() {
	nextToken(); // skip v
	auto& o = _tmpPos.emplace_back(0, 0, 0);
	for (int i = 0; i < 3; i++) {
		float v;
		StringUtil::tryParse(_token.str, v);
		o.data[i] = v;
		nextToken();
	}
}

void WavefrontObjLoader::_readUv() {
	nextToken(); // skip vt
	auto& o = _tmpUv.emplace_back(0, 0);
	for (int i = 0; i < 2; i++) {
		float v;
		StringUtil::tryParse(_token.str, v);
		o.data[i] = v;
		nextToken();
	}
}

void WavefrontObjLoader::_readNormal() {
	nextToken(); // skip vn
	auto& o = _tmpNormal.emplace_back(0, 0, 0);
	for (int i = 0; i < 3; i++) {
		float v;
		StringUtil::tryParse(_token.str, v);
		o.data[i] = v;
		nextToken();
	}
}

void WavefrontObjLoader::_readFace() {
	const char* last = cur(); // start as ' '
	nextChar(); // skip f

	Tuple3<int> indexes = Tuple3<int>(0, 0, 0); // x:pos, y:uv, z:normal

	int checkFaceVertexCount = 0;
	while (nextChar()) {

		if (ch() == ' ' || ch() == '\n') {
			StrView str = StrView(last, cur() - last - 1);
			//SGE_LOG("now:{}", str);
			indexes.x = 0;
			indexes.y = 0;
			indexes.z = 0;
			if (StringUtil::hasChar(str, '/')) {
				__readFaceInnerWithChar(indexes, str, '/'); // v1/vt1/vn1 ...
			}
			else {
				__readFaceInnerWithChar(indexes, str, ' '); // v1 v2 v3 ...
			}

			checkFaceVertexCount++;

			int vi = indexes.x;
			int vt = indexes.y;
			int vn = indexes.z;

			SGE_LOG("\t\tvi:{}, vt:{}, vn:{}, ch:{}", vi, vt, vn, static_cast<int>(ch()));

			__readPosValue(vi);
			__readUvValue(vt);
			__readNormalValue(vn);
			__readIndices(vi);

			last = cur(); // skip ' '
		}

		if (ch() == '\n') {
			break;
		}
	}

	// at least triangle face??? --> face vi >= 3
	if (checkFaceVertexCount < 3) {
		_error("face indexes < 3");
	}

	nextToken(); // skip '\n'
}

// str : v1/v2/v3 or v1 v2 v3
void WavefrontObjLoader::__readFaceInnerWithChar(Tuple3<int>& out, StrView str, char splitSymbol = '/') {
	int index = 0;
	while (str.size()) {
		auto s = StringUtil::splitByChar(str, splitSymbol);
		auto& first = s.first;
		if (first.size()) {
			StringUtil::tryParse(first, out.data[index]);
		}
		str = s.second;
		if (splitSymbol == '/') {
			index++;
		}
	}
}

// index: 1 ~ size
size_t WavefrontObjLoader::__getRealIndex(size_t index, size_t size) {
	size_t res = 0;
	if (index > 0) {
		res = index - 1; // start from [0]
	}
	else if (index < 0) {
		if (size + index < 0) _error("out of range");
		res = size + index; // index < 0
	}
	if (res >= size) {
		_error("out of range");
	}
	return res;
}

void WavefrontObjLoader::__readPosValue(size_t index) {
	if (index == 0) {
		_error("face vi == 0");
	}
	index = __getRealIndex(index, _tmpPos.size());
	_outInfo->pos.emplace_back(_tmpPos[index]);
}

void WavefrontObjLoader::__readUvValue(size_t index) {
	index = __getRealIndex(index, _tmpUv.size());
	_outInfo->uv[0].emplace_back(_tmpUv[index]);
}

void WavefrontObjLoader::__readNormalValue(size_t index) {
	index = __getRealIndex(index, _tmpNormal.size());
	_outInfo->normal.emplace_back(_tmpNormal[index]);
}

void WavefrontObjLoader::__readIndices(size_t index) {
	index = __getRealIndex(index, _tmpPos.size());
	_outInfo->indices.emplace_back(static_cast<u32>(index));
}

} // namespace