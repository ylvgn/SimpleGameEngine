#include "NeHeOGL_Lesson009.h"

namespace sge {

void NeHeOGL_Lesson009::StarInfo::randomColor() {
	color.set(rand()%256, rand()%256, rand()%256, 255);
}

void NeHeOGL_Lesson009::onInitedGL() {
	Base::onInitedGL();

	setClearColor({ 0, 0, 0, 0.5f }); // Black Background
	setCameraPos({ -0.78f, -0.08f, 17.f });
	setCameraAngle({ 22.35f, 19.05f });

	_starList.resize(kStarCount);
	for (int i = 0; i < kStarCount; ++i) {
		auto& star = _starList[i];
		star.randomColor();
		star.dist = (static_cast<float>(i) / kStarCount) * 5.0f;
		star.mesh.createRect(1, 1);
	}

	{
		Texture2D::CreateDesc desc;
		desc.imageToUpload.loadFile("Star.bmp");
		desc.samplerState.filter = Texture2D::TextureFilter::Linear;
		_starTex.create(desc);
	}
}

void NeHeOGL_Lesson009::onUIKeyboardEvent(UIKeyboardEvent& ev) {
	Base::onUIKeyboardEvent(ev);

	if (!_isPressedT && ev.isDown(KeyCode::T)) {
		_isTwinkle = !_isTwinkle;
		_isPressedT = true;
		SGE_DUMP_VAR(static_cast<int>(_isTwinkle));
	}
	if (ev.isUp(KeyCode::T)) {
		_isPressedT = false;
	}
}

void NeHeOGL_Lesson009::onRender() {
	_example1();
}

void NeHeOGL_Lesson009::_example1() {
	glEnable(GL_TEXTURE_2D);	// Enable Texture Mapping
	glShadeModel(GL_SMOOTH);	// Enable Smooth Shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really Nice Perspective Calculations

	beginOpaquePass();
	{
		// do nothing
	}
	endOpaquePass();


	beginTransparentPass();
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE); // additive blend
		_starTex.bind();
		int starCount = static_cast<int>(_starList.size());
		for (int i = 0; i < starCount; ++ i) {
			auto& star = _starList[i];
			OGL::Scoped_glPushMatrix scoped; // Reset The View Before We Draw Each Star

			glRotatef(_tilt,		1.0f,0.0f,0.0f);	// Tilt The View
			glRotatef(star.angle,	0.0f,1.0f,0.0f);	// Rotate To The Current Stars Angle
			glTranslatef(star.dist,	0.0f,0.0f);			// Move Forward On The X Plane
			glRotatef(-star.angle,	0.0f,1.0f,0.0f);	// Cancel The Current Stars Angle
			glRotatef(-_tilt,		1.0f,0.0f,0.0f);	// Cancel The Screen Tilt

			if (_isTwinkle) {
				const auto& anotherStar = _starList[(starCount - i) - 1];
				const auto& anotherStarColor = anotherStar.color;
#if 0 // Assign A Color Using Bytes
				glColor4ub(anotherStarColor.r, anotherStarColor.g, anotherStarColor.b, 255);
#else
				auto& vertices = star.mesh.vertices;
				for (auto& v : vertices) {
					v.color.set(anotherStarColor);
				}
#endif
				star.mesh.drawInherit();
			}

			glRotatef(_spin, 0.0f, 0.0f, 1.0f); // Rotate The Star On The Z Axis
#if 0 // Assign A Color Using Bytes
			OGL::color4b(star.color);
#else
			auto& vertices = star.mesh.vertices;
			for (auto& v : vertices) {
				v.color.set(star.color);
			}
#endif
			star.mesh.drawInherit();

			_spin += 0.01f;                    // Used To Spin The Stars
			star.angle += static_cast<float>(i) / _starList.size();
			star.dist  -= 0.01f;
			if (star.dist < 0.0f)
			{
				star.dist += 5.0f;
				star.randomColor();
			}
		}
		_starTex.unbind();
	}
	endTransparentPass();
}

}