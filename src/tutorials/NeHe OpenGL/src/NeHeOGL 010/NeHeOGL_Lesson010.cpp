#include "NeHeOGL_Lesson010.h"

namespace sge {

void NeHeOGL_Lesson010::onInitedGL() {
	Base::onInitedGL();

	WorldLoader::s_readFile(_sector, "World.txt");

	{
		NeHeOGL_Texture2D::CreateDesc desc;
		desc.imageToUpload.loadFile("Mud.bmp");
		desc.samplerState.filter = TextureFilter::Linear;
		_tex.create(desc);
	}
}

void NeHeOGL_Lesson010::onUIMouseEvent(UIMouseEvent& ev) {
	using Button	= UIMouseEvent::Button;
	using Type		= UIMouseEvent::Type;
	using Modifier	= UIEventModifier;
	if (ev.isMove()) {
		float d = 0.15f;
		_camerOrbitAngle += ev.deltaPos.yx() * d;
	}
}

void NeHeOGL_Lesson010::onUIKeyboardEvent(UIKeyboardEvent& ev) {
	if (ev.isDown(KeyCode::RightArrow)) {
		_camerOrbitAngle.y += 1.5f;
	}
	if (ev.isDown(KeyCode::LeftArrow)) {
		_camerOrbitAngle.y -= 1.5f;
	}

	static constexpr float kOffsetGround = 0.25f;

	if (ev.isDown(KeyCode::UpArrow)) {
		float s, c;
		Math::sincos(-_camerOrbitAngle.y * kPIOver180, s, c);
		_camerMovePos.set(_camerMovePos.x - s * 0.05f, _camerMovePos.y, _camerMovePos.z - c * 0.05f);

		if (_walkBiasAngle >= 359.f) _walkBiasAngle = 0.f + (_walkBiasAngle - 359.0f);
		else _walkBiasAngle += 10.f;

		_camerMovePos.y = kOffsetGround + Math::sin(_walkBiasAngle * kPIOver180) / 20.f;
	}
	if (ev.isDown(KeyCode::DownArrow)) {
		float s, c;
		Math::sincos(-_camerOrbitAngle.y * kPIOver180, s, c);
		_camerMovePos.set(_camerMovePos.x + s * 0.05f, _camerMovePos.y, _camerMovePos.z + c * 0.05f);

		if (_walkBiasAngle <= 1.f) _walkBiasAngle = 359.f + (_walkBiasAngle - 1.f) + 1.f;
		else _walkBiasAngle -= 10.f;

		_camerMovePos.y = kOffsetGround + Math::sin(_walkBiasAngle * kPIOver180) / 20.f;
	}
}

void NeHeOGL_Lesson010::onDraw() {
	float width = _clientRect.w;
	float height = _clientRect.h;

	if (height == 0) height = 1;

	float aspect = width / height;

// set viewport
	glViewport(0, 0, static_cast<int>(_clientRect.w), static_cast<int>(_clientRect.h));

// set render state
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_TEXTURE_2D);

	glColor4f(1, 1, 1, 1);
	glClearColor(0.f, 0.2f, 0.2f, 0.f);
	glClearDepth(1);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

// set matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, aspect, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

// setup player camera
	glRotatef(_camerOrbitAngle.x, 1,0,0);	// Rotate Up And Down To Look Up And Down
	glRotatef(_camerOrbitAngle.y, 0,1,0);	// Rotate Depending On Direction Player Is Facing
	OGL::translatef(-_camerMovePos);		// Translate The Scene Based On Player Position

	OGL::drawGridAndOriginAxis();

// test case
	_example1();

// swap buffer
	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson010::_example1() {
	_tex.bind();
	OGL::Scoped_glBegin begin(NeHeOGL_BeginMode::Triangles);
	for (auto& triangle : _sector.triangles) {
		for (int i = 0; i < 3; ++i) {
			auto& vertex = triangle.vertex[i];
			OGL::texCoord2f(vertex.uv);
			OGL::vertex3f(vertex.pos);
			OGL::normal3f({ 0.f,0.f,1.f });
		}
	}
	_tex.unbind();
}


void NeHeOGL_Lesson010::WorldLoader::s_readFile(SECTOR& outInfo, StrView filename) {
	MemMapFile mm;
	mm.open(filename);
	s_readMem(outInfo, mm.span(), filename);
}

void NeHeOGL_Lesson010::WorldLoader::s_readMem(SECTOR& outInfo, ByteSpan bytes, StrView filename) {
	WorldLoader inst;
	inst._readMem(outInfo, bytes, filename);
}

void NeHeOGL_Lesson010::WorldLoader::_readMem(SECTOR& outInfo, ByteSpan bytes, StrView filename) {
	_outInfo = &outInfo;
	_filename = filename;
	_source = StrView_make(bytes);
	_sourceRemain = _source;

	while (!_sourceRemain.empty()) {
		_nextLine();
		if (_currentLine.empty()) continue;
		_lineRemain = _currentLine;
		_nextToken();
		_parseLine();
	}

#if 0
	int j = 0;
	for (auto& triangle : outInfo.triangles) {
		SGE_LOG("========================== {} Start", j);
		for (int i = 0; i < 3; ++i) {
			auto& vertex = triangle.vertex[i];
			SGE_DUMP_VAR(vertex.pos, vertex.uv);
		}
		SGE_LOG("========================== {} End\n", j);
		j++;
	}
#endif
}

void NeHeOGL_Lesson010::WorldLoader::_nextLine() {
	auto pair = StringUtil::splitByChar(_sourceRemain, "\n");
	_currentLine = pair.first;
	_sourceRemain = pair.second;
	_lineNumber++;
}

void NeHeOGL_Lesson010::WorldLoader::_nextToken() {
	{ // trim spaces
		auto* p = _lineRemain.begin();
		auto* e = _lineRemain.end();
		while (p < e) {
			char ch = *p;
			if (ch != ' ' && ch != '\t' && ch != '\r')
				break;
			p++;
		}
		if (p != _lineRemain.begin())
			_lineRemain = StrView(p, e - p - 1);
	}

	auto pair = StringUtil::splitByChar(_lineRemain, " \t\r");
	_token = StringUtil::trimChar(pair.first, " \t\r");
	_lineRemain = StringUtil::trimChar(pair.second, " \t\r");
}

void NeHeOGL_Lesson010::WorldLoader::_parseLine() {
	if (_token == "//") return;

	if (_token == "NUMPOLLIES") {
		_parseLine_NUMPOLLIES();
		return;
	}

	_parseLine_tagTRIANGLE();
}

void NeHeOGL_Lesson010::WorldLoader::_parseLine_NUMPOLLIES() {
	_nextToken();

	int triangleCount;
	if (!StringUtil::tryParse(_token, triangleCount)) {
		_error("error read NUMPOLLIES");
	}
	_outInfo->triangles.resize(triangleCount);

	for (auto& v : _outInfo->triangles) {
		v.vertex.resize(3);
	}

	_tagSECTOR_index   = 0;
	_tagTRIANGLE_index = 0;
}

void NeHeOGL_Lesson010::WorldLoader::_parseLine_tagTRIANGLE() {
	_internal_parseLine_tagTRIANGLE(_outInfo->triangles[_tagSECTOR_index], _tagTRIANGLE_index);

	_tagTRIANGLE_index = (_tagTRIANGLE_index + 1) % 3;
	if (_tagTRIANGLE_index % 3 == 0) {
		++_tagSECTOR_index;
	}
}

void NeHeOGL_Lesson010::WorldLoader::_internal_parseLine_tagTRIANGLE(TRIANGLE& outTriangle, int index) {
	auto& o = outTriangle.vertex[index];
	if (!StringUtil::tryParse(_token, o.pos.x)) {
		_error("error read tagTRIANGLE pos.x");
	}
	_nextToken();

	if (!StringUtil::tryParse(_token, o.pos.y)) {
		_error("error read tagTRIANGLE pos.y");
	}
	_nextToken();

	if (!StringUtil::tryParse(_token, o.pos.z)) {
		_error("error read tagTRIANGLE pos.z");
	}
	_nextToken();

	if (!StringUtil::tryParse(_token, o.uv.x)) {
		_error("error read tagTRIANGLE uv.z");
	}
	_nextToken();
	if (!StringUtil::tryParse(_token, o.uv.y)) {
		_error("error read tagTRIANGLE uv.y");
	}
	_nextToken();

//	SGE_DUMP_VAR(_lineNumber, o.pos, o.uv);
}

}