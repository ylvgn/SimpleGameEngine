#include "NeHeOGL_Lesson009.h"

namespace sge {

void NeHeOGL_Lesson009::StarInfo::randomColor() {
	color.set(rand()%256, rand()%256, rand()%256, 255);
}

void NeHeOGL_Lesson009::StarInfo::setVertexColor(const Color4b& c) {
	auto& vertices = mesh.vertices;
	for (auto& v : vertices) {
		v.color.set(c);
	}
}

void NeHeOGL_Lesson009::onInitedGL() {
	Base::onInitedGL();

	setClearColor({ 0, 0, 0, 0.5f }); // Black Background
	setCameraPos({ -0.78f, -0.08f, 17.f });
	setCameraOrbitAngle({ 22.35f, 19.05f });

	_starList.resize(kStarCount);
	for (int i = 0; i < kStarCount; ++i) {
		auto& star = _starList[i];
		star.randomColor();
		star.distToOrigin = (static_cast<float>(i) / kStarCount) * kMaxDistFromOrigin;
		star.mesh.createRect(1, 1);
	}

	{
		Texture2D::CreateDesc desc;
		desc.imageToUpload.loadFile("Star.bmp");
		desc.samplerState.filter = Texture2D::TextureFilter::Linear;
		_starTex.create(desc);
	}

	OGLUtil::throwIfError();
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
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really Nice Perspective Calculations

#define SGE_PLANE_ROT_XOY 0
#define SGE_PLANE_ROT_ZOY 1
#define SGE_PLANE_ROT_XOZ 2
#define SGE_PLANE_ROT SGE_PLANE_ROT_XOZ

	beginTransparentPass();
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE); // additive blend

		_starTex.bind();

	#if (SGE_PLANE_ROT_XOZ == SGE_PLANE_ROT)
		glRotatef(90, 1, 0, 0);
	#elif (SGE_PLANE_ROT_ZOY == SGE_PLANE_ROT)
		glRotatef(90,0, 1, 0);
	#endif

		int starCount = static_cast<int>(_starList.size());
		for (int i = 0; i < starCount; ++ i) {
			auto& star = _starList[i];
			{
				OGL::Scoped_glPushMatrix scoped;			// Reset The View Before We Draw Each Star
		#if (SGE_PLANE_ROT_XOY == SGE_PLANE_ROT)
			#if 0
				OGL::rotatef(_tilt, Vec3f::s_right());		// Tilt The View
				OGL::rotatef(star.angle, Vec3f::s_up());	// Rotate To The Current Stars Angle
				glTranslatef(star.distToOrigin,0,0);		// Move Forward On The X Plane (XOY)
				OGL::rotatef(-star.angle, Vec3f::s_up());	// Cancel The Current Stars Angle
				OGL::rotatef(-_tilt, Vec3f::s_right());		// Cancel The Screen Tilt
			#else
				OGL::rotatef(-_tilt, Vec3f::s_up());
				OGL::rotatef(star.angle, Vec3f::s_right());
				glTranslatef(0, star.distToOrigin, 0);
				OGL::rotatef(-star.angle, Vec3f::s_right());
				OGL::rotatef(_tilt, Vec3f::s_up());
			#endif
		#elif (SGE_PLANE_ROT_ZOY == SGE_PLANE_ROT)
				OGL::rotatef(star.angle, Vec3f::s_forward());
				glTranslatef(0, star.distToOrigin, 0);
				OGL::rotatef(-star.angle, Vec3f::s_forward());
		#elif (SGE_PLANE_ROT_XOZ == SGE_PLANE_ROT)
				OGL::rotatef(_tilt, Vec3f::s_right());
				OGL::rotatef(star.angle, Vec3f::s_up());
				glTranslatef(star.distToOrigin, 0, star.distToOrigin);
				OGL::rotatef(-star.angle, Vec3f::s_up());
				OGL::rotatef(-_tilt, Vec3f::s_right());
		#else
			#error "unsupported"
		#endif
				if (_isTwinkle) { // because use GL_SRC_ALPHA, GL_ONE, draw the same pos will additive!
					const auto& anotherStar			= _starList[(starCount - i) - 1]; // just for symmetry
					const auto& anotherStarColor	= anotherStar.color;
					star.setVertexColor(anotherStarColor);
					star.mesh.drawInherit();
				}

				OGL::rotatef(_spin, Vec3f::s_forward()); // star local rotate on The Z Axis

				star.setVertexColor(star.color);
				star.mesh.drawInherit();

				_spin += 0.15f;  // Used To Spin All The Stars, add spin speed

				star.angle += static_cast<float>(i) / starCount;
				star.distToOrigin -= 0.01f;
				if (star.distToOrigin < 0.0f) {
					star.distToOrigin += kMaxDistFromOrigin;
					star.randomColor();
				}
			}
		}
		_starTex.unbind();
	}
	endTransparentPass();

#undef SGE_PLANE_ROT_XOY
#undef SGE_PLANE_ROT_ZOY
#undef SGE_PLANE_ROT_XOZ
#undef SGE_PLANE_ROT
}

}