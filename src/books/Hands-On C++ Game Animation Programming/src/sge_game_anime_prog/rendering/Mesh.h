#pragma once

#include <sge_game_anime_prog/math/vec3.h>
#include <sge_game_anime_prog/math/vec4.h>
#include <sge_game_anime_prog/math/mat4.h>

#include <sge_game_anime_prog/opengl/Attribute.h>
#include <sge_game_anime_prog/opengl/IndexBuffer.h>

#include <sge_game_anime_prog/animation/Skeleton.h>

namespace sge {
/*
	It should maintain a copy of the mesh data on the CPU as well as on the GPU.
	The Mesh class contains two copies of the same data.
	It keeps all the vertex data on the CPU side in vectors
	and on the GPU side in vertex buffer objects.

	The intended use of this class is to edit the CPU-side vertices,
	then sync the changes to the GPU with the 'uploadToGpu' functions.

	1. static vertex pipeline: vertex->MVP
	2. rigid skinned vertex pipeline: vertex->skinned vertex->MVP
	3. smooth skinned vertex pipeline: vertex->skinned vertex with weights->MVP

	ps: This Mesh class is not production-ready, but it's easy to work with
*/

class Mesh {
public:

	static const int kInvalidSlotIndex = -1;

	Mesh();
	Mesh(const Mesh& r);
	Mesh& operator=(const Mesh& r);

	void cpuSkin(const Skeleton& skeleton, const Pose& pose);
	void cpuSkin(const Span<const mat4f>& animatedPose);
	void uploadToGpu();

	void bind(  int pos, int normal, int uv, int jointWeight = kInvalidSlotIndex, int jointInflucence = kInvalidSlotIndex);
	void unbind(int pos, int normal, int uv, int jointWeight = kInvalidSlotIndex, int jointInflucence = kInvalidSlotIndex);

	void draw();
	void drawInstanced(size_t instanceCount);

	inline size_t getVertexCount()	const { return pos.size(); };
	inline size_t getIndexCount()	const { return indices.size(); };

	Vector<u32>		indices;

	Vector<vec3f>	pos;
	Vector<vec3f>	normal;
	Vector<vec2f>	uv;

	Vector<vec4f>	color;

	// smooth skinned mesh
	Vector<vec4f>	jointWeights;    // values from [0, 1], expected added together is equal 1.
	Vector<vec4i>	jointInfluences; // contains jointId that affect the vertex

private:
	// Each of the vectors listed in the preceding code also needs to be set appropriate attributes.
	UPtr<IndexBuffer>		 _indexBuffer;

	UPtr< Attribute<vec3f> > _posAttrib;
	UPtr< Attribute<vec3f> > _normalAttrib;
	UPtr< Attribute<vec2f> > _uvAttrib;

	// smooth skinned mesh
	UPtr< Attribute<vec4f> > _jointWeightsAttrib;
	UPtr< Attribute<vec4i> > _jointInfluencesAttrib;

	// Add an additional copy of (skinned vectors)
		// -the position
		// -and normal,
		// -as well as a matrix palette to use for CPU skinning
	Vector<vec3f>			_skinnedPos;
	Vector<vec3f>			_skinnedNormal;

	Vector<mat4f>			_posePalette;
};

}