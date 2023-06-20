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

	This Mesh class is not production-ready, but it's easy to work with
*/

class Mesh {
public:
	Mesh();
	Mesh(const Mesh& r);
	Mesh& operator=(const Mesh& r);

	// The vector references are not read-only; The references are expected to be edited
	// you use these when loading meshes to populate the mesh data
	inline Vector<vec3f>& pos()				{ return _pos; }
	inline Vector<vec3f>& normal()			{ return _normal; }
	inline Vector<vec2f>& uv()				{ return _uv; }
	inline Vector<u32>&   indices()			{ return _indices; }
	inline Vector<vec4f>& jointWeights()	{ return _jointWeights; }
	inline Vector<vec4i>& jointInfluences()	{ return _jointInfluences; }

	// static vertex pipeline : vertex->MVP
	// rigid skinned vertex pipeline : vertex->skinned vertex->MVP
	void cpuSkin(const Skeleton& skeleton, const Pose& pose);
	void uploadToGpu();

	void bind(  int pos, int normal, int uv, int jointWeight, int jointInflucence);
	void unbind(int pos, int normal, int uv, int jointWeight, int jointInflucence);

	void draw();
	void drawInstanced(u32 instanceCount);

private:
	Vector<vec3f>	_pos;
	Vector<vec3f>	_normal;
	Vector<vec2f>	_uv;
	Vector<u32>		_indices; // optional

	// smooth skinned mesh
	Vector<vec4f> _jointWeights;    // values from [0, 1], expected added together is equal 1.
	Vector<vec4i> _jointInfluences; // contains jointId that affect the vertex

	// Each of the vectors listed in the preceding code also needs to be set appropriate attributes.
	UPtr< Attribute<vec3f> > _posAttrib;
	UPtr< Attribute<vec3f> > _normalAttrib;
	UPtr< Attribute<vec2f> > _uvAttrib;
	UPtr<IndexBuffer>		 _indexBuffer; // optional

	// smooth skinned mesh
	UPtr< Attribute<vec4f> > _jointWeightsAttrib;
	UPtr< Attribute<vec4i> > _jointInfluencesAttrib;

	// Add an additional copy of (skinned vectors)
		// -the position
		// -and normal,
		// -as well as a matrix palette to use for CPU skinning
	Vector<vec3f> _skinnedPos;
	Vector<vec3f> _skinnedNormal;

	Vector<mat4>  _posePalette;
};

}