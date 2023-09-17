#pragma once

#include "MathGeometry.h"

namespace sge {

class Mesh;

struct IntersectionsUtil {
	IntersectionsUtil() = delete;

	static bool raycastTriangle(const Ray& ray, const Triangle& triangle, vec3f& outHitPoint);
	static Vector<Triangle> meshToTriangles(const Mesh& mesh);
	static Vector<Triangle> meshesToTriangles(const Span<const Mesh>& meshes);

	static bool barycentricTechnique(const vec3f& p, const Triangle& triangle);
};

}