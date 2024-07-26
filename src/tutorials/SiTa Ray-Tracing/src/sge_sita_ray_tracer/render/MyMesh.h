#pragma once

#include "MyVertex.h"
#include "../math/MyMathGeometry.h"

namespace sge {

class MyMesh : public NonCopyable {
public:
	using IndexType = uint16_t;

	MyMesh() {
		doubleSided = false;
		wireframe = false;
	}

	void createRect(float w, float h);
	void createCube(float w, float h, float d);

	void loadObjFile(const char* filename);

	void draw();
	void clear();

	void setColor(const Color4b& c);

	void updateAABB();

	Vector<MyVertex_PosColorUv> vertices;
	Vector<IndexType> indices;

	Math::MyAABB3f aabb;

	bool doubleSided	: 1;
	bool wireframe		: 1;
};

}