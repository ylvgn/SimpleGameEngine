#include "Mesh.h"

#include <sge_game_anime_prog/opengl/Draw.h>

namespace sge {

Mesh::Mesh() {
	_indexBuffer			= eastl::make_unique< IndexBuffer >();
	_posAttrib				= eastl::make_unique< Attribute<vec3f> >();
	_normalAttrib			= eastl::make_unique< Attribute<vec3f> >();
	_uvAttrib				= eastl::make_unique< Attribute<vec2f> >();
	_jointWeightsAttrib		= eastl::make_unique< Attribute<vec4f> >();
	_jointInfluencesAttrib	= eastl::make_unique< Attribute<vec4i> >();
}

Mesh::Mesh(const Mesh& r) {
	_indexBuffer			= eastl::make_unique< IndexBuffer >();
	_posAttrib				= eastl::make_unique< Attribute<vec3f> >();
	_normalAttrib			= eastl::make_unique< Attribute<vec3f> >();
	_uvAttrib				= eastl::make_unique< Attribute<vec2f> >();
	_jointWeightsAttrib		= eastl::make_unique< Attribute<vec4f> >();
	_jointInfluencesAttrib	= eastl::make_unique< Attribute<vec4i> >();

	*this = r;
}

Mesh& Mesh::operator=(const Mesh& r) {
	// copy out the CPU-side members (all of the vectors)
	// and then call the 'uploadToGpu' function to upload the attribute data to the GPU

	if (this == &r) {
		return *this;
	}

	pos				= r.pos;
	normal			= r.normal;
	uv				= r.uv;
	indices			= r.indices;
	jointWeights	= r.jointWeights;
	jointInfluences	= r.jointInfluences;

	uploadToGpu();

	return *this;
}

void Mesh::cpuSkin(const Skeleton& skeleton, const Pose& pose) {
/*
	CPU mesh-skinning to animate a mesh
	CPU skinning is useful
		if the platform you are developing for has a limited number of uniform registers or a small uniform buffer.
*/

	size_t vertexCount = getVertexCount();
	if (vertexCount == 0) return;

	_skinnedPos.resize(vertexCount);
	_skinnedNormal.resize(vertexCount);

#if 1 // skin matrix

	pose.getMatrixPalette(_posePalette);
	const auto& invPosePalette = skeleton.invBindPose();

	for (int i = 0; i < vertexCount; ++i) {
		const vec4i& j = jointInfluences[i];
		const vec4f& w = jointWeights[i];

		mat4f m0 = _posePalette[j.x] * invPosePalette[j.x];
		mat4f m1 = _posePalette[j.y] * invPosePalette[j.y];
		mat4f m2 = _posePalette[j.z] * invPosePalette[j.z];
		mat4f m3 = _posePalette[j.w] * invPosePalette[j.w];

		mat4f skin = (m0*w.x) + (m1*w.y) + (m2*w.z) + (m3*w.w);

		_skinnedPos[i]    = skin.transformPoint(pos[i]);
		_skinnedNormal[i] = skin.transformVector(normal[i]);
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
		const vec4i& jointIds = jointInfluences[i];
		const vec4f& weights  = jointWeights[i];

		// 1.calc skin transform
		Transform skin0 = Transform::s_combine(
			pose.getGlobalTransform(jointIds.x),
			bindPose.getGlobalTransform(jointIds.x).inverse()
		);
		// 2.transform vertex data by using skin transform 
		vec3f p0 = skin0.transformPoint(pos[i]);
		vec3f n0 = skin0.transformPoint(normal[i]);

		Transform skin1 = Transform::s_combine(
			pose.getGlobalTransform(jointIds.y),
			bindPose.getGlobalTransform(jointIds.y).inverse()
		);
		vec3f p1 = skin1.transformPoint(pos[i]);
		vec3f n1 = skin1.transformPoint(normal[i]);

		Transform skin2 = Transform::s_combine(
			pose.getGlobalTransform(jointIds.z),
			bindPose.getGlobalTransform(jointIds.z).inverse()
		);
		vec3f p2 = skin2.transformPoint(pos[i]);
		vec3f n2 = skin2.transformPoint(normal[i]);

		Transform skin3 = Transform::s_combine(
			pose.getGlobalTransform(jointIds.w),
			bindPose.getGlobalTransform(jointIds.w).inverse()
		);
		vec3f p3 = skin3.transformPoint(pos[i]);
		vec3f n3 = skin3.transformPoint(normal[i]);

		// 3. blend the 4 results with jointWeights
		_skinnedPos[i]    = (p0*weights.x) + (p1*weights.y) + (p2*weights.z) + (p3*weights.w);
		_skinnedNormal[i] = (n0*weights.x) + (n1*weights.y) + (n2*weights.z) + (n3*weights.w);
	}
#endif

	_posAttrib->uploadToGpu(ByteSpan_make(_skinnedPos.span()));
	_normalAttrib->uploadToGpu(ByteSpan_make(_skinnedNormal.span()));
}

void Mesh::cpuSkin(const Span<const mat4f>& animatedPose) {
	// animatedPose = _posePalette * invPosePalette

	size_t vertexCount = getVertexCount();
	if (vertexCount == 0) return;

	_skinnedPos.resize(vertexCount);
	_skinnedNormal.resize(vertexCount);

	for (int i = 0; i < vertexCount; ++i) {
		const vec4i& j = jointInfluences[i];
		const vec4f& w = jointWeights[i];

		vec3f p0 = animatedPose[j.x].transformPoint(pos[i]);
		vec3f p1 = animatedPose[j.y].transformPoint(pos[i]);
		vec3f p2 = animatedPose[j.z].transformPoint(pos[i]);
		vec3f p3 = animatedPose[j.w].transformPoint(pos[i]);
		_skinnedPos[i] = (p0*w.x) + (p1*w.y) + (p2*w.z) + (p3*w.w);

		vec3f n0 = animatedPose[j.x].transformVector(normal[i]);
		vec3f n1 = animatedPose[j.y].transformVector(normal[i]);
		vec3f n2 = animatedPose[j.z].transformVector(normal[i]);
		vec3f n3 = animatedPose[j.w].transformVector(normal[i]);
		_skinnedNormal[i] = (n0*w.x) + (n1*w.y) + (n2*w.z) + (n3*w.w);
	}

	_posAttrib->uploadToGpu(ByteSpan_make(_skinnedPos.span()));
	_normalAttrib->uploadToGpu(ByteSpan_make(_skinnedNormal.span()));
}

void Mesh::bind(int pos_,
				int normal_,
				int uv_,
				int jointWeight_/*=kInvalidSlotIndex*/,
				int jointInflucence_/*=kInvalidSlotIndex*/)
{
	// This takes integers that are bind slot indices.
	// If the bind slot is valid, valid means slot >= 0, call the 'bind' function of attribute
	if (pos_ >= 0)				{ _posAttrib->bind(pos_); }
	if (normal_ >= 0)			{ _normalAttrib->bind(normal_); }
	if (uv_ >= 0)				{ _uvAttrib->bind(uv_); }
	if (jointWeight_ >= 0)		{ _jointWeightsAttrib->bind(jointWeight_); }
	if (jointInflucence_ >= 0)	{ _jointInfluencesAttrib->bind(jointInflucence_); }
}

void Mesh::unbind(	int pos_,
					int normal_,
					int uv_,
					int jointWeight_/*= kInvalidSlotIndex*/,
					int jointInflucence_/*= kInvalidSlotIndex*/)
{
	if (pos_ >= 0)				{ _posAttrib->unbind(pos_); }
	if (normal_ >= 0)			{ _normalAttrib->unbind(normal_); }
	if (uv_ >= 0)				{ _uvAttrib->unbind(uv_); }
	if (jointWeight_ >= 0)		{ _jointWeightsAttrib->unbind(jointWeight_); }
	if (jointInflucence_ >= 0)	{ _jointInfluencesAttrib->unbind(jointInflucence_); }
}

void Mesh::uploadToGpu() {
	// syncs the vectors holding data to the GPU
	// If one of the CPU-side vectors has a size of 0, then there is nothing to set

	if (!pos.empty())				{ _posAttrib->uploadToGpu(ByteSpan_make(pos.span())); }
	if (!normal.empty())			{ _normalAttrib->uploadToGpu(ByteSpan_make(normal.span())); }
	if (!uv.empty())				{ _uvAttrib->uploadToGpu(ByteSpan_make(uv.span())); }
	if (!indices.empty())			{ _indexBuffer->uploadToGpu(ByteSpan_make(indices.span())); }
	if (!jointWeights.empty())		{ _jointWeightsAttrib->uploadToGpu(ByteSpan_make(jointWeights.span())); }
	if (!jointInfluences.empty())	{ _jointInfluencesAttrib->uploadToGpu(ByteSpan_make(jointInfluences.span())); }
}

void Mesh::draw() {
	size_t indexCount = getIndexCount();
	if (indexCount > 0) {
		DrawUtil::draw(*_indexBuffer.get());
	} else {
		DrawUtil::draw(getVertexCount());
	}
}

void Mesh::drawInstanced(size_t instanceCount) {
	size_t indexCount = getIndexCount();
	if (indexCount > 0) {
		DrawUtil::drawInstanced(*_indexBuffer.get(), instanceCount);
	} else {
		DrawUtil::drawInstanced(getVertexCount(), instanceCount);
	}
}

}