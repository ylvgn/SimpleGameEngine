#include "NeHeOGL_Lesson011.h"

namespace sge {

void NeHeOGL_Lesson011::onInitedGL() {
	Base::onInitedGL();
	{
		NeHeOGL_Texture2D::CreateDesc desc;
		desc.imageToUpload.loadFile("Tim.bmp");
		desc.samplerState.filter = TextureFilter::Linear;
		_tex.create(desc);
	}

	glPolygonMode(GL_BACK, GL_FILL);  // Back Face Is Filled In
	glPolygonMode(GL_FRONT, GL_LINE); // Front Face Is Drawn With Lines

	{
		_mesh.pos.resize(kSize * kSize);
		_mesh.uv[0].resize(kSize * kSize);

		const float kDegAngle = 40.f;
		const float kScale = 1.f / 5.0f;

		auto* pos = _mesh.pos.begin();
		auto* uv = _mesh.uv[0].begin();
		for (float y = 0; y < kSize; ++y)
			for (float x = 0; x < kSize; ++x) {
				// center the "wave":
				// for example:
					// size = 45
					// left-top (0, 0), bottom-right(45-1,45-1), from now center = (44/2, 44/2)
					// when scaled, div 5, 45/5 == 9, from now center = (9/2, 9/2)
					// we also subtract 4.5, so 9-4.5 = 4.5, and equivalent to (4.5, 4.5)
				float posX = x * kScale; 
				float posY = y * kScale;
				float posZ = Math::sin(Math::radians(posX * kDegAngle));

				pos->set(posX * 0.5f, posY * 0.5f, posZ); // pos->set(posX - 4.5f, posY - 4.5f, posZ);
				uv->set(0,0);

				pos++;
				uv++;
			}

		SGE_ASSERT(pos == _mesh.pos.end());
		SGE_ASSERT(uv == _mesh.uv[0].end());
	}

	OGLUtil::throwIfError();
}

void NeHeOGL_Lesson011::onDraw() {
	setFrameBufferSize(_clientRect.size);

	OGL::drawGridAndOriginAxis();

	_example1();

	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson011::_example1() {
	auto uptime = static_cast<float>(_uptime.get());

	static constexpr float kRotateSpeedX = 10.f;
	static constexpr float kRotateSpeedY = 20.f;
	static constexpr float kRotateSpeedZ = 40.f;
	float angleInDegreesX = uptime * kRotateSpeedX;
	float angleInDegreesY = uptime * kRotateSpeedY;
	float angleInDegreesZ = uptime * kRotateSpeedZ;
	OGL::rotatef(angleInDegreesX, Vec3f::s_right());
	OGL::rotatef(angleInDegreesY, Vec3f::s_up());
	OGL::rotatef(angleInDegreesZ, Vec3f::s_forward());

	{
		_tex.bind();
		{
			int N = kSize - 1;
			OGL::Scoped_glBegin scoped(NeHeOGL_BeginMode::Quads);

			for (int x = 0; x < N; ++x)
				for (int y = 0; y < N; ++y) {
					// Notice the quads are drawn clockwise
					// This means the face you see initially will be the back. The back is filled in. The front is made up of lines
					float uv_x  = static_cast<float>(x)   / N;
					float uv_y  = static_cast<float>(y)   / N;
					float uv_x1 = static_cast<float>(x+1) / N;
					float uv_y1 = static_cast<float>(y+1) / N;

					auto* uvLB = _getVertexUv(x,   y);		uvLB->set(uv_x,  uv_y);
					auto* uvLT = _getVertexUv(x,   y+1);	uvLT->set(uv_x,  uv_y1);
					auto* uvRT = _getVertexUv(x+1, y+1);	uvRT->set(uv_x1, uv_y1);
					auto* uvRB = _getVertexUv(x+1, y);		uvRB->set(uv_x1, uv_y);

					_bindMeshByQuads(x, y);
				}
		}
		_tex.unbind();
	}

	if (_wiggleCount >= 2) {
		for (int y = 0; y < kSize; ++y) {
			auto* tmpPos = _getVertexPos(0, y);
			float tmpWave = tmpPos->z;
			for (int x = 0; x < kSize - 1; ++x) {
				auto* cur = _getVertexPos(x,   y);
				auto* ne  = _getVertexPos(x+1, y);
				cur->z = ne->z;
			}
			auto* lastPos = _getVertexPos(kSize - 1, y);
			lastPos->z = tmpWave;
		}
		_wiggleCount = 0;
	}
	_wiggleCount++;
}

void NeHeOGL_Lesson011::_bindMesh(int x, int y) {
	auto* pos = _getVertexPos(x, y);
	auto* uv  = _getVertexUv(x, y);
	OGL::texCoord2f(*uv);
	OGL::vertex3f(*pos);
}

void NeHeOGL_Lesson011::_bindMeshByQuads(int x, int y) {
	_bindMesh(x,   y);		// First Texture Coordinate (Bottom Left)
	_bindMesh(x,   y+1);	// Second Texture Coordinate (Top Left)
	_bindMesh(x+1, y+1);	// Third Texture Coordinate (Top Right)
	_bindMesh(x+1, y);		// Fourth Texture Coordinate (Bottom Right)
}

}