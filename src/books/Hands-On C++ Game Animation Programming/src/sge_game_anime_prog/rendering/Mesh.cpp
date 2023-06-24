#include "Mesh.h"
#include <sge_game_anime_prog/opengl/Draw.h>

namespace sge {

Mesh::Mesh() {
	_posAttrib		= eastl::make_unique< Attribute<vec3f> >();
	_normalAttrib	= eastl::make_unique< Attribute<vec3f> >();
	_uvAttrib		= eastl::make_unique< Attribute<vec2f> >();
	_indexBuffer	= eastl::make_unique<IndexBuffer>();

	_jointWeightsAttrib    = eastl::make_unique< Attribute<vec4f> >();
	_jointInfluencesAttrib = eastl::make_unique< Attribute<vec4i> >();
}

Mesh::Mesh(const Mesh& r) {
	_posAttrib		= eastl::make_unique< Attribute<vec3f> >();
	_normalAttrib	= eastl::make_unique< Attribute<vec3f> >();
	_uvAttrib		= eastl::make_unique< Attribute<vec2f> >();
	_indexBuffer	= eastl::make_unique<IndexBuffer>();

	_jointWeightsAttrib	   = eastl::make_unique< Attribute<vec4f> >();
	_jointInfluencesAttrib = eastl::make_unique< Attribute<vec4i> >();
	*this = r;
}

Mesh& Mesh::operator=(const Mesh& r) {
	// copy out the CPU-side members(all of the vectors)
	// and then call the 'uploadToGpu' function to upload the attribute data to the GPU

	if (this == &r) {
		return *this;
	}

	_pos				= r._pos;
	_normal				= r._normal;
	_uv					= r._uv;
	_indices			= r._indices;
	_jointWeights		= r._jointWeights;
	_jointInfluences	= r._jointInfluences;
	uploadToGpu();

	return *this;
}

void Mesh::cpuSkin(const Skeleton& skeleton, const Pose& pose) {
/*
	CPU mesh-skinning to animate a mesh
	CPU skinning is useful if the platform you are developing for has a limited number of uniform registers or a small uniform buffer.
*/

	size_t vertexCount = _pos.size();
	if (vertexCount == 0) return;

	_skinnedPos.resize(vertexCount);
	_skinnedNormal.resize(vertexCount);

#if 1 // skin matrix

	pose.getMatrixPalette(_posePalette);
	const auto& invPosePalette = skeleton.invBindPose();

	for (int i = 0; i < vertexCount; ++i) {
		const vec4i& j = _jointInfluences[i];
		const vec4f& w = _jointWeights[i];

		mat4 m0 = _posePalette[j.x] * invPosePalette[j.x];
		mat4 m1 = _posePalette[j.y] * invPosePalette[j.y];
		mat4 m2 = _posePalette[j.z] * invPosePalette[j.z];
		mat4 m3 = _posePalette[j.w] * invPosePalette[j.w];

		mat4 skin = (m0*w.x) + (m1*w.y) + (m2*w.z) + (m3*w.w);

		_skinnedPos[i]    = skin.transformPoint(_pos[i]);
		_skinnedNormal[i] = skin.transformVector(_normal[i]);
	}

#else // skin tranform
/*
	basic skinning algorithm:
	// 1. calc skin transform: combine current animated pose with inverse bind pose of jointInfluences
	// 2. transform vertex data by using skin transform 
		// skin the mesh 4 times (step1 && step2)
	// 3. blend the 4 results with jointWeights
*/

	const auto& bindPose = skeleton.bindPose();
	for (int i = 0; i < vertexCount; ++i) {
		const vec4i& jointIds = _jointInfluences[i];
		const vec4f& weights  = _jointWeights[i];

		// 1.calc skin transform
		Transform skin0 = Transform::s_combine(
			pose.getGlobalTransform(jointIds.x),
			bindPose.getGlobalTransform(jointIds.x).inverse()
		);
		// 2.transform vertex data by using skin transform 
		vec3 p0 = skin0.transformPoint(_pos[i]);
		vec3 n0 = skin0.transformPoint(_normal[i]);

		Transform skin1 = Transform::s_combine(
			pose.getGlobalTransform(jointIds.y),
			bindPose.getGlobalTransform(jointIds.y).inverse()
		);
		vec3 p1 = skin1.transformPoint(_pos[i]);
		vec3 n1 = skin1.transformPoint(_normal[i]);

		Transform skin2 = Transform::s_combine(
			pose.getGlobalTransform(jointIds.z),
			bindPose.getGlobalTransform(jointIds.z).inverse()
		);
		vec3 p2 = skin2.transformPoint(_pos[i]);
		vec3 n2 = skin2.transformPoint(_normal[i]);

		Transform skin3 = Transform::s_combine(
			pose.getGlobalTransform(jointIds.w),
			bindPose.getGlobalTransform(jointIds.w).inverse()
		);
		vec3 p3 = skin3.transformPoint(_pos[i]);
		vec3 n3 = skin3.transformPoint(_normal[i]);

		// 3. blend the 4 results with jointWeights
		_skinnedPos[i]    = (p0*weights.x) + (p1*weights.y) + (p2*weights.z) + (p3*weights.w);
		_skinnedNormal[i] = (n0*weights.x) + (n1*weights.y) + (n2*weights.z) + (n3*weights.w);
	}
#endif

	_posAttrib->uploadToGpu(_skinnedPos);
	_normalAttrib->uploadToGpu(_skinnedNormal);
}

void Mesh::cpuSkin(const Span<const mat4>& animatedPose) {
	// animatedPose = _posePalette * invPosePalette
	size_t vertexCount = _pos.size();
	if (vertexCount == 0) return;

	_skinnedPos.resize(vertexCount);
	_skinnedNormal.resize(vertexCount);

	for (int i = 0; i < vertexCount; ++i) {
		const vec4i& j = _jointInfluences[i];
		const vec4f& w = _jointWeights[i];

		vec3f p0 = animatedPose[j.x].transformPoint(_pos[i]);
		vec3f p1 = animatedPose[j.y].transformPoint(_pos[i]);
		vec3f p2 = animatedPose[j.z].transformPoint(_pos[i]);
		vec3f p3 = animatedPose[j.w].transformPoint(_pos[i]);
		_skinnedPos[i] = (p0*w.x) + (p1*w.y) + (p2*w.z) + (p3*w.w);

		vec3f n0 = animatedPose[j.x].transformVector(_normal[i]);
		vec3f n1 = animatedPose[j.y].transformVector(_normal[i]);
		vec3f n2 = animatedPose[j.z].transformVector(_normal[i]);
		vec3f n3 = animatedPose[j.w].transformVector(_normal[i]);
		_skinnedNormal[i] = (n0*w.x) + (n1*w.y) + (n2*w.z) + (n3*w.w);
	}

	_posAttrib->uploadToGpu(_skinnedPos);
	_normalAttrib->uploadToGpu(_skinnedNormal);
}

void Mesh::uploadToGpu() {
	// syncs the vectors holding data to the GPU
	// If one of the CPU-side vectors has a size of 0, then there is nothing to set
	if (_pos.size() > 0)				{ _posAttrib->uploadToGpu(_pos); }
	if (_normal.size() > 0)				{ _normalAttrib->uploadToGpu(_normal); }
	if (_uv.size() > 0)					{ _uvAttrib->uploadToGpu(_uv); }
	if (_indices.size() > 0)			{ _indexBuffer->uploadToGpu(_indices); }
	if (_jointWeights.size() > 0)		{ _jointWeightsAttrib->uploadToGpu(_jointWeights); }
	if (_jointInfluences.size() > 0)	{ _jointInfluencesAttrib->uploadToGpu(_jointInfluences); }
}

void Mesh::bind(int pos, int normal, int uv, int jointWeight/*=-1*/, int jointInflucence/*=-1*/) {
	// This takes integers that are bind slot indices.
	// If the bind slot is valid, valid means slot >= 0, call the 'bind' function of attribute/
		// why not pass a slot always >= 0 ???
	if (pos >= 0)				{ _posAttrib->bind(pos); }
	if (normal >= 0)			{ _normalAttrib->bind(normal); }
	if (uv >= 0)				{ _uvAttrib->bind(uv); }
	if (jointWeight >= 0)		{ _jointWeightsAttrib->bind(jointWeight); }
	if (jointInflucence >= 0)	{ _jointInfluencesAttrib->bind(jointInflucence); }
}

void Mesh::unbind(int pos, int normal, int uv, int jointWeight, int jointInflucence) {
	if (pos >= 0)				{ _posAttrib->unbind(pos); }
	if (normal >= 0)			{ _normalAttrib->unbind(normal); }
	if (uv >= 0)				{ _uvAttrib->unbind(uv); }
	if (jointWeight >= 0)		{ _jointWeightsAttrib->unbind(jointWeight); }
	if (jointInflucence >= 0)	{ _jointInfluencesAttrib->unbind(jointInflucence); }
}

void Mesh::draw() {
	if (_indices.size() > 0) {
		DrawUtil::draw(*_indexBuffer.get());
	} else {
		DrawUtil::draw(_pos.size());
	}
}

void Mesh::drawInstanced(u32 instanceCount) {
	if (_indices.size() > 0) {
		DrawUtil::drawInstanced(*_indexBuffer.get(), instanceCount);
	} else {
		DrawUtil::drawInstanced(_pos.size(), instanceCount);
	}
}

}