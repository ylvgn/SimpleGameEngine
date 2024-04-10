#pragma once

#include "../NeHeOGL_NativeUI.h"

namespace sge {
/*
	* 3D Shapes: cone and cube
	* FYI: https://nehe.gamedev.net/tutorial/3d_shapes/10035/
*/

class NeHeOGL_Lesson005 : public NeHeOGL_NativeUIWindow {
	using Base = NeHeOGL_NativeUIWindow;
	using This = NeHeOGL_Lesson005;

	using VertexIndex = u16;

public:
	~NeHeOGL_Lesson005();
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onDraw() override;
private:

	void _drawMyGrid();
	void _drawMyCoordinate();

	void _example1();
	void _shaded(float angleInDegrees);
	void _wireFrame(float angleInDegrees);

	void _example2();
	void _cullFace();
	void _frontFace();
	void _depthTest();
	void _bufferWriteMask();
	void _zFight();

	void _example3();
	void _example4();

	void _example5();
	void _drawMySphere(float radius, int subAxis, int subHeight);
	void _drawMygluSphere(double radius, int slices, int stacks);

	void _example6();
	void _drawMyCube1();
	void _drawMyCube2();
	void _drawMyCube3();
	void _drawMyCube4();
	void _drawMyCube5();

	void _example7();

	static constexpr float d = 1.f;

	static constexpr float PI  = Math::PI<float>();
	static constexpr float PI2 = 2.f * PI;

//------------------------------------------
//		   1-----2
//		  /  0  /
//		 3-----4
	static constexpr float kCone[5][3] = {
		{ 0,  d,  0}, // 0 Top
		{-d, -d, -d}, // 1
		{ d, -d, -d}, // 2
		{-d, -d,  d}, // 3
		{ d, -d,  d}, // 4
	};

//------------------------------------------
//		  0--------1
//		 /|       /|
//		3--------2 |
//      | |      | |
//		| 4------|-5
//      |/       |/
//      7--------6
	static constexpr float kCube[8][3] = {
		{-d, d,-d}, // 0
		{ d, d,-d}, // 1
		{ d, d, d}, // 2
		{-d, d, d}, // 3
		{-d,-d,-d}, // 4
		{ d,-d,-d}, // 5
		{ d,-d, d}, // 6
		{-d,-d, d}, // 7
	};

	static constexpr GLenum kIndexType = OGLUtil::getGlFormat((NeHe_RenderDataTypeUtil::get<VertexIndex>()));
	static constexpr VertexIndex kCubeIndices[12][3] = {
		{0, 2, 1}, // top
		{0, 3, 2},
		{3, 6, 2}, // front
		{3, 7, 6},
		{2, 5, 1}, // rigt
		{2, 6, 5},
		{7, 5, 6}, // bottom
		{7, 4, 5},
		{4, 1, 5}, // back
		{4, 0, 1},
		{4, 3, 0}, // left
		{4, 7, 3},
	};

	static constexpr size_t kCubeIndicesCount = sizeof(kCubeIndices) / sizeof(VertexIndex);

	MyHiResTimer _uptime;

	float _cameraX = 30.f;
	float _cameraY = 30.f;

	GLUquadric* _gluQuad = nullptr;

	NeHeOGL_Vertex_PosColor _cubeVertexs[8];

	NeHeOGL_Mesh	_cubeMesh;
	NeHeOGL_Mesh	_rectMesh;
	GLuint			_texture2d;
};

}