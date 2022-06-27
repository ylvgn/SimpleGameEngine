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
		if (_token.isIdentifier("v"))	{ _parseLine_v();	continue; }
		if (_token.isIdentifier("vt"))	{ _parseLine_vt();	continue; }
		if (_token.isIdentifier("vn"))	{ _parseLine_vn();	continue; }
		if (_token.isIdentifier("f"))	{ _parseLine_f();	continue; }
		if (!nextToken())				{ break; }
	}
}

void WavefrontObjLoader::_readNumber(String& out) {
	out.clear();
	trimSpaces();
	auto* st = cur() - 1;
	int i = 0;
	while (_ch != ' ' && _ch != '\n') {
		i++;
		if (!nextChar()) break;
	}
	out.assign(st, st + i);
}

void WavefrontObjLoader::_parseLine_v() {
	nextChar(); // skip v

	/*
	w is the weight required for rational curvesand surfaces.It is
		not required for non - rational curves and surfaces.If you do not
		specify a value for w, the default is 1.0.
	*/
	Tuple4f value(0, 0, 0, 1);
	_readLineNumbers(value);
	nextToken(); // skip to \n

	Tuple3f pos(value.x / value.w
				, value.y / value.w
				, value.z / value.w);
	_tmpPos.emplace_back(pos);
}

void WavefrontObjLoader::_parseLine_vt() {
	nextChar(); // skip vt
	auto& o = _tmpUv.emplace_back(0, 0);
	_readLineNumbers(o);
	nextToken(); // skip to \n
}

void WavefrontObjLoader::_parseLine_vn() {
	nextChar(); // skip vn
	auto& o = _tmpNormal.emplace_back(0, 0, 0);
	_readLineNumbers(o);
	nextToken(); // skip to \n
}

void WavefrontObjLoader::_parseLine_f() {
	const char* last = cur(); // start as ' '
	nextChar(); // skip f

	Vector_<int, 64>	face_vi;
	Vector_<int, 64>	face_vt;
	Vector_<int, 64>	face_vn;
	Tuple3<int> _tmp = Tuple3<int>(0, 0, 0); // x:pos, y:uv, z:normal

	while (nextChar()) {

		if (ch() == ' ' || ch() == '\n') {
			StrView str = StrView(last, cur() - last - 1);
			//SGE_LOG("now:{}", str);
			_tmp.x = 0;
			_tmp.y = 0;
			_tmp.z = 0;
			if (StringUtil::hasChar(str, '/')) {
				_parseLine_f_inner(_tmp, str, '/'); // v1/vt1/vn1 ...
			} else {
				_parseLine_f_inner(_tmp, str, ' '); // v1 v2 v3 ...
			}
			int vi = _tmp.x;
			int vt = _tmp.y;
			int vn = _tmp.z;

			//SGE_LOG("\t\tvi:{}, vt:{}, vn:{}, ch:{}", vi, vt, vn, static_cast<int>(ch()));

			// index start from 1 and Negative values indicate relative vertex numbers
			if (vi != 0) {
				if (vi > 0) { vi--; }
				else { vi = static_cast<int>(_tmpPos.size()) + vi; }
				face_vi.emplace_back(vi);
			}

			if (vt != 0) {
				if (vt > 0) { vt--; }
				else { vt = static_cast<int>(_tmpUv.size()) + vt; }
				face_vt.emplace_back(vt);
			}

			if (vn != 0) {
				if (vn > 0) { vn--; }
				else { vn = static_cast<int>(_tmpNormal.size()) + vn; }
				face_vn.emplace_back(vn);
			}

			last = cur(); // skip ' '
		}

		if (ch() == '\n') {
			break;
		}
	}
	nextToken(); // skip '\n'

	// at least triangle face --> face vi >= 3
	if (face_vi.size() < 3) {
		_error("face size < 3");
	}

	u32 indicesStart = static_cast<u32>(_outInfo->pos.size());

#if 0 // flip face front/back
	eastl::reverse(face_vi.begin(), face_vi.end());
	eastl::reverse(face_vt.begin(), face_vt.end());
	eastl::reverse(face_vn.begin(), face_vn.end());
#endif

	for (int i = 0; i < face_vi.size(); i++) {
		auto vi = face_vi[i];

		if (vi >= _tmpPos.size()) _error("face vi out of range");
		_outInfo->pos.emplace_back(_tmpPos[vi]);

		if (face_vt.size() >= face_vi.size()) {
			auto vt = face_vt[i];
			if (vt >= _tmpUv.size()) _error("face vt out of range");
			_outInfo->uv[0].emplace_back(_tmpUv[vt]);
		}

		if (face_vn.size() >= face_vn.size()) {
			auto vn = face_vn[i];
			if (vn >= _tmpNormal.size()) _error("face vn out of range");
			_outInfo->normal.emplace_back(_tmpNormal[vn]);
		}
	}

	for (size_t i = 0; i < face_vi.size(); i++) {
		if (i >= 3) {
			_outInfo->indices.emplace_back(static_cast<u32>(indicesStart + 0));
			_outInfo->indices.emplace_back(static_cast<u32>(indicesStart + i - 1));
		}

		u32 vi = static_cast<u32>(indicesStart + i);
		if (vi >= _outInfo->pos.size()) {
			throw SGE_ERROR("vi out of range");
		}
		_outInfo->indices.emplace_back(vi);
	}
}

// str : v1/v2/v3 or v1 v2 v3
void WavefrontObjLoader::_parseLine_f_inner(Tuple3<int>& out, StrView str, char splitSymbol = '/') {
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

} // namespace