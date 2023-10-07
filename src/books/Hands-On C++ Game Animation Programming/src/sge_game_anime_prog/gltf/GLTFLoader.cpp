#include "GLTFLoader.h"
#include <sge_game_anime_prog/math/Transform.h>
#include <sge_game_anime_prog/animation/Track.h>

namespace sge {

namespace GLTFHelpers {
/*
	These functions are internal to the glTF loader and should not be exposed in the header file.
*/

static const float kEpsilon = 0.00001f;

Transform getLocalTransform(const cgltf_node& node) {
	// A node can store its transform either as a matrix or as a separate position, rotation, and scale components.

	Transform res;
	if (node.has_matrix) {
		mat4f m(&node.matrix[0]);
		res = Transform::s_mat(m);
	}

	if (node.has_translation) {
		res.position = vec3f(node.translation[0], node.translation[1], node.translation[2]);
	}
	if (node.has_rotation) {
		res.rotation = quat4f(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
	}
	if (node.has_scale) {
		res.scale = vec3f(node.scale[0], node.scale[1], node.scale[2]);
	}
	return res;
}

int getNodeIndex(const cgltf_node* target, const cgltf_node* allNodes, cgltf_size nodeCount) {
	// perform a simple linear lookup by looping through all the nodes in a .gltf file
	// let say, you can find the parent of a node using the getNodeIndex helper function, which returns -1 if a node has no parent

	if (target == nullptr) {
		return -1; // invalid index
	}
	for (int i = 0; i < nodeCount; ++i) {
		if (target == &allNodes[i]) {
			return i; // it will be a unit index in allNodes, so it is jointId
		}
	}
	return -1; // invalid index
}

void getScalarValues(Vector<float>& out, size_t componentCount, const cgltf_accessor& accessor) {
	// reads the floating-point values of a gltf accessor.
	// This can be done with the cgltf_accessor_read_float helper function.

	out.resize(accessor.count * componentCount);
	for (cgltf_size i = 0; i < accessor.count; ++i) {
		cgltf_accessor_read_float(&accessor, i, &out[i * componentCount], componentCount);
	}
}

template<typename T, size_t N>
void getTrackFromChannel(Track<T, N>& out, const cgltf_animation_channel& channel) {
	// It converts a glTF animation channel into a VectorTrack or a QuaternionTrack.
	// animation channels: https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_007_Animations.md

	const cgltf_animation_sampler* sampler = channel.sampler;
	Interpolation interpolation = Interpolation::Constant;
	if (sampler->interpolation == cgltf_interpolation_type_linear) {
		interpolation = Interpolation::Linear;
	}
	else if (sampler->interpolation == cgltf_interpolation_type_cubic_spline) {
		interpolation = Interpolation::Cubic;
	}
	out.setType(interpolation);

	// The sampler input is an accessor to the animation timeline.
	Vector<float> timelineFloats;
	getScalarValues(timelineFloats, 1, *sampler->input);

	// The sampler output is an accessor to the animation values.
	Vector<float> valueFloats;
	getScalarValues(valueFloats, N, *sampler->output);

	// The number of frames the number of elements in the sampler input.
	cgltf_size frameCount = sampler->input->count;

	// The number of components per frame (vec3f or quat4f) is the number of value elements divided by the number of timeline elements.
	size_t numberOfValuesPerFrame = valueFloats.size() / timelineFloats.size();

	bool isSamplerCubic = interpolation == Interpolation::Cubic;
	out.resize(frameCount);
	for (int i = 0; i < frameCount; ++i) {

		// loop through every frame in the sampler.
		// For each frame, set the time, then read the input tangent, the value, then the output tangent.
		// Input and output tangents are only available if the sampler is cubic;
		// if it is not, these should default to 0.

		size_t baseIndex = i * numberOfValuesPerFrame;
		int offset = 0;

		Frame<N>& frame = out[i];
		frame.time = timelineFloats[i];

		for (int component = 0; component < N; ++component) {
			frame.in[component] = isSamplerCubic 
				? valueFloats[baseIndex + offset++]
				: 0.0f;
		}

		for (int component = 0; component < N; ++component) {
			frame.value[component] = valueFloats[baseIndex + offset++];
		}

		for (int component = 0; component < N; ++component) {
			frame.out[component] = isSamplerCubic
				? valueFloats[baseIndex + offset++]
				: 0.0f;
		}
	}
}

size_t getComponentCount(cgltf_type dataType) {
	switch (dataType)
	{
		case cgltf_type_scalar: return 1;
		case cgltf_type_vec2:   return 2;
		case cgltf_type_vec3:   return 3;
		case cgltf_type_vec4:   return 4;
		case cgltf_type_mat2:   return 4;
		case cgltf_type_mat3:   return 9;
		case cgltf_type_mat4:   return 16;
		default:				throw SGE_ERROR("not support"); break;
	}
}

void skinMeshFromAttribute(Mesh& outMesh, const cgltf_attribute& attribute, const cgltf_skin* skin, const cgltf_node* nodes, cgltf_size nodeCount) {

	// 1. figuring out how many attributes the current component has
	const cgltf_accessor& accessor = *attribute.data;
	cgltf_type dataType = accessor.type;
	size_t componentCount = getComponentCount(dataType);

	// 2. parse the data out of the provided accessor
	Vector<float> values;
	getScalarValues(values, componentCount, accessor);

	// 3.write data to Mesh by loop through all the values in the current accessor
	// and assign them to the appropriate vector based on the accessor type.
	auto& pos			= outMesh.pos;
	auto& normal		= outMesh.normal;
	auto& uv			= outMesh.uv;
	auto& influences	= outMesh.jointInfluences;
	auto& weights		= outMesh.jointWeights;
	auto& color			= outMesh.color;

	cgltf_attribute_type attribType = attribute.type;
	cgltf_size accessorCount		= accessor.count;
	for (size_t i = 0; i < accessorCount; ++i) {
		size_t index = i * componentCount;
		switch (attribType)
		{
			case cgltf_attribute_type_position:
				SGE_ASSERT(dataType == cgltf_type_vec3);
				pos.push_back({values[index+0], values[index+1], values[index+2]});
				break;
			case cgltf_attribute_type_normal:
			{
				SGE_ASSERT(dataType == cgltf_type_vec3);
				vec3f norm (values[index+0],
							values[index+1],
							values[index+2]
				);
				if (norm.lenSq() < kEpsilon) {
					norm = vec3f::s_up();
				}
				normal.push_back(norm.normalize());
			} break;
			case cgltf_attribute_type_texcoord:
				SGE_ASSERT(dataType == cgltf_type_vec2);
				uv.push_back({values[index+0], values[index+1]});
				break;
			case cgltf_attribute_type_joints:
			{
				// These joints are stored as floating-point numbers. Convert them into integers
				// These indices are skin relative. 
				// This function has no information about the skin that is being parsed.
				// Add +0.5f to round, since we can not read integers -> same as Math::roundToInt

				SGE_ASSERT(dataType == cgltf_type_vec4);

				vec4i joints(
					Math::roundToInt(values[index + 0]),
					Math::roundToInt(values[index + 1]),
					Math::roundToInt(values[index + 2]),
					Math::roundToInt(values[index + 3])
				);

				// Make sure that even the invalid nodes have a value of 0.
				// Any negative joint indices will break the skinning implementation.
				joints.x = Math::max(0, getNodeIndex(skin->joints[joints.x], nodes, nodeCount));
				joints.y = Math::max(0, getNodeIndex(skin->joints[joints.y], nodes, nodeCount));
				joints.z = Math::max(0, getNodeIndex(skin->joints[joints.z], nodes, nodeCount));
				joints.w = Math::max(0, getNodeIndex(skin->joints[joints.w], nodes, nodeCount));

				influences.push_back(joints);
			} break;
			case cgltf_attribute_type_weights:
				SGE_ASSERT(dataType == cgltf_type_vec4);
				weights.push_back({values[index+0], values[index+1], values[index+2], values[index+3]});
				break;
			case cgltf_attribute_type_tangent:
				SGE_ASSERT(dataType == cgltf_type_vec4); // x, y, z + sign
				break;
			case cgltf_attribute_type_color:
				SGE_ASSERT(dataType == cgltf_type_vec4);
				color.push_back({values[index+ 0], values[index+1], values[index+2], values[index+3]});
				break;
			default: throw SGE_ERROR("not support cgltf_attribute_type"); break;
		}
	}
}

void staticMeshFromAttribute(Mesh& outMesh, const cgltf_attribute& attribute) {
	// same as skinMeshFromAttribute, but no need skin

	const cgltf_accessor& accessor	= *attribute.data;
	cgltf_type dataType				= accessor.type;
	size_t componentCount			= getComponentCount(dataType);

	Vector<f32> values;
	getScalarValues(values, componentCount, accessor);

	auto& pos						= outMesh.pos;
	auto& normal					= outMesh.normal;
	auto& uv						= outMesh.uv;
	auto& color						= outMesh.color;

	cgltf_attribute_type attribType = attribute.type;
	cgltf_size accessorCount		= accessor.count;
	for (size_t i = 0; i < accessorCount; ++i) {
		size_t index = i * componentCount;
		switch (attribType)
		{
			case cgltf_attribute_type_position:
				SGE_ASSERT(dataType == cgltf_type_vec3);
				pos.push_back({values[index+0], values[index+1], values[index+2]});
				break;
			case cgltf_attribute_type_normal:
			{
				SGE_ASSERT(dataType == cgltf_type_vec3);
				vec3f norm (values[index+0],
							values[index+1],
							values[index+2]
				);
				if (norm.lenSq() < kEpsilon) {
					norm = vec3f::s_up();
				}
				normal.push_back(norm.normalize());
			} break;
			case cgltf_attribute_type_texcoord:
				SGE_ASSERT(dataType == cgltf_type_vec2);
				uv.push_back({values[index+0], values[index+1]});
				break;
			case cgltf_attribute_type_color:
				SGE_ASSERT(dataType == cgltf_type_vec4);
				color.push_back({values[index+ 0], values[index+1], values[index+2], values[index+3] });
				break;
		}
	}
}

} // GLTFHelpers namespace

void GLTFLoader::s_readFile(Info& outInfo, StrView filename) {
	MemMapFile mm;
	mm.open(filename);
	s_readMem(outInfo, mm, filename);
}

void GLTFLoader::s_readMem(Info& outInfo, ByteSpan data, StrView filename) {
	GLTFLoader inst;
	inst._readMem(outInfo, data, filename);
}

void GLTFLoader::_readMem(Info& outInfo, ByteSpan data, StrView filename) {
	outInfo.clear();
	_outInfo = &outInfo;

	TempString s = filename;

	// To load a file, you need to create an instance of cgltf_options.
	// You won't need to set any option flags; just instantiate the cgltf_options struct with 0 for all member values.
	cgltf_options options;
	memset(&options, 0, sizeof(cgltf_options));
	
	SGE_ASSERT(_data == nullptr);
	// load the glTF data
	cgltf_result result = cgltf_parse(&options, data.data(), data.size(), &_data);
	if (result != cgltf_result_success) {
		throw SGE_ERROR("Could not load input file: {}\n", s);
	}
	
	// cgltf_load_buffers is used to load any external buffer data
	result = cgltf_load_buffers(&options, _data, s.c_str());
	if (result != cgltf_result_success) {
		throw SGE_ERROR ("Could not load buffers for: {}\n", s);
	}
	
	// cgltf_validate makes sure that the glTF file that was just loaded was valid
	result = cgltf_validate(_data);
	if (result != cgltf_result_success) {
		throw SGE_ERROR ("Invalid gltf file: {}\n", s);
	}

	_loadSkeleton();
	_loadAnimationClips();
	_loadAnimationClipNames();
	_loadSkinMeshes();
	_loadStaticMeshes();
}

GLTFLoader::~GLTFLoader() {
	if (_data) {
		cgltf_free(_data);
		_data = nullptr;
	}
}

void GLTFLoader::_loadRestPose() {
	// The animation system you will build in this book does not support joint lookup by name, only index.
	cgltf_size boneCount = _data->nodes_count;
	Pose o;
	o.resize(boneCount);

	for (int i = 0; i < boneCount; ++i) {
		const cgltf_node& node = _data->nodes[i];
		Transform transform = GLTFHelpers::getLocalTransform(node);
		o.setLocalTransform(i, transform);

		int parent = GLTFHelpers::getNodeIndex(node.parent, _data->nodes, boneCount);
		o.setParent(i, parent); // when parent == -1, means no parent
	}
	_outInfo->skeleton.setRestPose(o);
}

void GLTFLoader::_loadJointNames() {
	// At some point, you might want to know the name assigned to each joint that is loaded.
	// This can help make debugging or building tools easier.

	size_t boneCount = _data->nodes_count;
	Vector<String> o;
	o.resize(boneCount);
	for (int i = 0; i < boneCount; ++i) {
		const cgltf_node& node = _data->nodes[i];
		o[i] = node.name == 0
			? "EMPTY NODE"
			: node.name;
	}
	_outInfo->skeleton.setJointNames(o);
}

void GLTFLoader::_loadAnimationClips() {
	cgltf_size clipCount = _data->animations_count;
	cgltf_size nodeCount = _data->nodes_count;
	auto& o = _outInfo->animationClips;

	o.clear();
	o.resize(clipCount);

	for (int i = 0; i < clipCount; ++i) {
		const cgltf_animation& animation = _data->animations[i];
		o[i].setName(animation.name);

		// loop through all of the channels in the clip, each channel of a glTF file is an animation track.
		cgltf_size channelCount = animation.channels_count;
		for (int j = 0; j < channelCount; ++j) {
			const cgltf_animation_channel& channel = animation.channels[j];

			// find the index of the node that the current channel affects
			const cgltf_node* target = channel.target_node;
			int nodeIndex = GLTFHelpers::getNodeIndex(target, _data->nodes, nodeCount);
			SGE_ASSERT(nodeIndex != -1);

			// The[] operator of the Clip class either retrieves the current track or creates a new one.
			// This means the TransformTrack function for the node that you are parsing is always valid
			if (channel.target_path == cgltf_animation_path_type_translation) {
				VectorTrack& track = constCast(o[i][nodeIndex].position());
				GLTFHelpers::getTrackFromChannel<vec3f, 3>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_rotation) {
				QuaternionTrack& track = constCast(o[i][nodeIndex].rotation());
				GLTFHelpers::getTrackFromChannel<quat4f, 4>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_scale) {
				VectorTrack& track = constCast(o[i][nodeIndex].scale());
				GLTFHelpers::getTrackFromChannel<vec3f, 3>(track, channel);
			}
		}
		o[i].recalculateDuration();
	}
}

void GLTFLoader::_loadAnimationClipNames() {
	auto& src = _outInfo->animationClips;
	if (src.size() == 0) {
		_loadAnimationClips();
	}
	auto& o = _outInfo->animationClipNames;
	o.resize(src.size());
	int i = 0;
	for (auto& clip : src) {
		const auto& name = clip.name();
		o[i++].assign(name.data(), name.size());
	}
}

void GLTFLoader::_loadBindPose() {
/*
	glTF files don't store the bind pose.
	Reconstructing the bind pose is not ideal.
*/

	const auto& restPose = _outInfo->restPose();
	size_t boneCount = restPose.getJointCount();
	SGE_ASSERT(boneCount > 0);

	Vector<Transform> worldBindPose;
	worldBindPose.resize(boneCount);

	for (int i = 0; i < boneCount; ++i) {
		// This makes sure that if a skin didn't provide an inverse bind pose matrix for a joint, a good default value is available.
		// By using the rest pose as the default joint values,
		// any joint that does not have an inverse bind pose matrix still has a valid default orientation and size.
		worldBindPose[i] = restPose.getGlobalTransform(i); // default value
	}

	// for each skin that a glTF file contains,
	// it stores a matrix array that holds the inverse bind pose matrix for each joint that affects the skin.
	cgltf_size skinCount = _data->skins_count;
	for (int i = 0; i < skinCount; ++i) {
		const cgltf_skin& skin = _data->skins[i];

		Vector<float> invBindPoseMat4s;
		GLTFHelpers::getScalarValues(invBindPoseMat4s, 16, *skin.inverse_bind_matrices);

		for (int j = 0; j < skin.joints_count; ++j) {
			const cgltf_node* joint = skin.joints[j];

			// Read the ivnerse bind matrix of the joint
			const float* matrix = &(invBindPoseMat4s[j * 16]);
			mat4f invBindPoseMat4(matrix);

			// Invert the inverse bind pose matrix to get the bind pose matrix.
			mat4f bindPoseMat4 = invBindPoseMat4.inverse();
			Transform bindPoseTrans = Transform::s_mat(bindPoseMat4);

			// Set that transform in the worldBindPose.
			int nodeIndex = GLTFHelpers::getNodeIndex(joint, _data->nodes, boneCount);
			SGE_ASSERT(nodeIndex != -1);
			worldBindPose[nodeIndex] = bindPoseTrans;
		}
	}

	// Convert each joint so that it is relative to its parent.
	Pose o(restPose);
	for (int i = 0; i < boneCount; ++i) {
		Transform world = worldBindPose[i];
		int parent = restPose.getParent(i);

		// we assump call _loadRestPose first, so parent maybe come up -1
		if (parent >= 0) { // Bring into parent space, world/global space -> local space
			Transform parentTrans = worldBindPose[parent];
			world = Transform::s_combine(parentTrans.inverse(), world);
		}
		o.setLocalTransform(i, world); // when no parent, worldTrans==localTrans
	}
	_outInfo->skeleton.setBindPose(o);
}

void GLTFLoader::_loadSkeleton() {
//	This is a convenience function that loads a skeleton
//	without having to call three separate functions.
	_loadRestPose();
	_loadBindPose();
	_loadJointNames();
}

void GLTFLoader::_loadSkinMeshes() {
	auto& o = _outInfo->skinMeshes;
	o.clear();

	cgltf_node* nodes		= _data->nodes;
	cgltf_size  nodeCount	= _data->nodes_count;

	for (int i = 0; i < nodeCount; ++i) {
		const cgltf_node& node = nodes[i];

		if (node.mesh == nullptr || node.skin == nullptr) {
			// Only process nodes that have both a mesh and a skin;
			// any other nodes should be skipped
			continue;
		}

		cgltf_size primitivesCount = node.mesh->primitives_count;
		for (int j = 0; j < primitivesCount; ++j) {
			const cgltf_primitive& primitive = node.mesh->primitives[j];
			o.push_back(Mesh());
			auto& mesh = o.back();

			cgltf_size attributeCount = primitive.attributes_count;
			for (cgltf_size k = 0; k < attributeCount; ++k) {
				const cgltf_attribute& attribute = primitive.attributes[k];
				GLTFHelpers::skinMeshFromAttribute(mesh, attribute, node.skin, nodes, nodeCount);
			}

			// Check whether the primitive contains indices.If it does, the index buffer of the mesh needs to be filled out as well
			if (primitive.indices != 0) {
				cgltf_size indiceCount = primitive.indices->count;
				auto& indices = mesh.indices;
				indices.resize(indiceCount);

				for (cgltf_size k = 0; k < indiceCount; ++k) {
					indices[k] = static_cast<u32>(cgltf_accessor_read_index(primitive.indices, k));
				}
			}
			mesh.uploadToGpu();
		}
	}
}

void GLTFLoader::_loadStaticMeshes() {
	auto& o = _outInfo->staticMeshes;
	o.clear();

	cgltf_node* nodes		= _data->nodes;
	cgltf_size  nodeCount	= _data->nodes_count;

	for (int i = 0; i < nodeCount; ++i) {
		const cgltf_node& node = nodes[i];

		if (node.mesh == nullptr) {
			continue;
		}

		cgltf_size primitivesCount = node.mesh->primitives_count;
		for (int j = 0; j < primitivesCount; ++j) {
			const cgltf_primitive& primitive = node.mesh->primitives[j];
			o.push_back(Mesh());
			auto& mesh = o.back();

			cgltf_size attributeCount = primitive.attributes_count;
			for (cgltf_size k = 0; k < attributeCount; ++k) {
				const cgltf_attribute& attribute = primitive.attributes[k];
				GLTFHelpers::staticMeshFromAttribute(mesh, attribute);
			}

			if (primitive.indices != 0) {
				cgltf_size indiceCount = primitive.indices->count;
				auto& indices = mesh.indices;
				indices.resize(indiceCount);

				for (cgltf_size k = 0; k < indiceCount; ++k) {
					indices[k] = static_cast<u32>(cgltf_accessor_read_index(primitive.indices, k));
				}
			}
			mesh.uploadToGpu();
		}
	}
}

}