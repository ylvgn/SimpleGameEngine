#include "Intersections.h"

#include "MathGeometry.h"
#include <sge_game_anime_prog/rendering/Mesh.h>

namespace sge {

bool IntersectionsUtil::raycastTriangle(const Ray& ray, const Triangle& triangle, vec3f& outHitPoint) {
#if 1
	constexpr static const float kEpsilon = 0.0000001f;

	vec3f vertex0		= triangle.v0;
	vec3f vertex1		= triangle.v1;
	vec3f vertex2		= triangle.v2;
	vec3f rayVector		= ray.direction;
	vec3f rayOrigin		= ray.origin;
	vec3f edge1, edge2, h, s, q;
	float a, f, u, v;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;
	h = rayVector.cross(edge2);
	a = edge1.dot(h);
	if (Math::equals0(a, kEpsilon)) {
		return false;
	}
	f = 1.0f / a;
	s = rayOrigin - vertex0;
	u = f * s.dot(h);
	if (u < 0.0f || u > 1.0f) {
		return false;
	}
	q = s.cross(edge1);
	v = f * rayVector.dot(q);
	if (v < 0.0f || u + v > 1.0f) {
		return false;
	}
	float t = f * edge2.dot(q);
	if (t > kEpsilon) {
		outHitPoint = rayOrigin + rayVector * t;
		return true;
	}
	return false;
#else
	const vec3f& v0			= triangle.v0;
	const vec3f& v1			= triangle.v1;
	const vec3f& v2			= triangle.v2;
	const vec3f& rayVector	= ray.direction;

	// lineAB
	const auto& A = ray.origin;
	const auto& B = A + rayVector;

	// plane v0,v1,v2
	Plane plane;
	plane.setByTriangle(v0, v1, v2);

	// lineCD (lineAB project to plane)
	float distAToPlane = plane.dot(A);
	float distBToPlane = plane.dot(B);

	// line line intersect
	vec3f C = A - (plane.normal * distAToPlane);
	vec3f D = B - (plane.normal * distBToPlane);

	vec3f BA = B - A;
	vec3f nBA = BA.normalize();
	float cosTheta = nBA.dot((C-A).normalize());
	if (Math::equals0(cosTheta)) {
		return false;
	}
	float t = distAToPlane / cosTheta;
	vec3f intersetPt = nBA * t + A;

	if (IntersectionsUtil::barycentricTechnique(intersetPt, triangle)) {
		outHitPoint = intersetPt;
		return true;
	}
	return false;
#endif
}

Vector<Triangle> IntersectionsUtil::meshToTriangles(const Mesh& mesh) {
	Vector<Triangle> res;
	
	const auto& indices = mesh.indices;
	const auto& pos		= mesh.pos;

	if (indices.empty()) {
		res.resize(pos.size() / 3);
		for (int i = 0, j = 0; i < pos.size(); i+=3, ++j) {
			res[j].set(	pos[i],
						pos[i+1],
						pos[i+2]);
		}
	}
	else
	{
		res.resize(indices.size() / 3);
		for (int i = 0, j = 0; i < indices.size(); i += 3, ++j) {
			res[j].set(	pos[indices[i]],
						pos[indices[i+1]],
						pos[indices[i+2]]);
		}
	}
	return res;
}

Vector<Triangle> IntersectionsUtil::meshesToTriangles(const Span<const Mesh>& meshes) {
	Vector<Triangle> res;
	for (auto& mesh : meshes) {
		res.appendRange(IntersectionsUtil::meshToTriangles(mesh));
	}
	return res;
}

bool IntersectionsUtil::barycentricTechnique(const vec3f& pt, const Triangle& triangle) {
	// https://blackpawn.com/texts/pointinpoly/
	
	const vec3f& A = triangle.v0;
	const vec3f& B = triangle.v1;
	const vec3f& C = triangle.v2;
	const vec3f& P = pt;

	// Compute vectors        
	vec3f v0 = C - A;
	vec3f v1 = B - A;
	vec3f v2 = P - A;

	// Compute dot products
	float dot00 = v0.dot(v0);
	float dot01 = v0.dot(v1);
	float dot02 = v0.dot(v2);
	float dot11 = v1.dot(v1);
	float dot12 = v1.dot(v2);

	// Compute barycentric coordinates
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);
}

}
