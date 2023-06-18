#include "GLTFLoader.h"
#include <sge_game_anime_prog/math/Transform.h>
#include <sge_game_anime_prog/animation/Track.h>

namespace sge {

namespace GLTFHelpers {
/*
	These functions are internal to the glTF loaderand should not be exposed in the header file.
*/

Transform getLocalTransform(const cgltf_node& node) {
	// A node can store its transform either as a matrix or as a separate position, rotation, and scale components.

	Transform res;
	if (node.has_matrix) {
		mat4 mat(&node.matrix[0]);
		res = g_mat4ToTransform(mat);
	}

	if (node.has_translation) {
		res.position = vec3(node.translation[0], node.translation[1], node.translation[2]);
	}
	if (node.has_rotation) {
		res.rotation = quat(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
	}
	if (node.has_scale) {
		res.scale = vec3(node.scale[0], node.scale[1], node.scale[2]);
	}
	return res;
}

int getNodeIndex(const cgltf_node* target, cgltf_node* allNodes, size_t nodeCount) {
	// perform a simple linear lookup by looping through all the nodes in a .gltf file
	// let say, you can find the parent of a node using the getNodeIndex helper function, which returns -1 if a node has no parent

	if (target == nullptr) {
		return -1; // invalid index
	}
	for (int i = 0; i < nodeCount; ++i) {
		if (target == &allNodes[i]) {
			return i;
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

	// The number of components per frame (vec3 or quat) is the number of value elements divided by the number of timeline elements.
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

	_loadRestPose();
	_loadJointNames();
	_loadAnimationClips();
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
	auto& o = _outInfo->restPose;
	o.resize(boneCount);

	for (int i = 0; i < boneCount; ++i) {
		const cgltf_node& node = _data->nodes[i];
		Transform transform = GLTFHelpers::getLocalTransform(node);
		o.setLocalTransform(i, transform);
		int parentIndex = GLTFHelpers::getNodeIndex(node.parent, _data->nodes, boneCount);
		o.setParent(i, parentIndex);
	}
}

void GLTFLoader::_loadJointNames() {
	// At some point, you might want to know the name assigned to each joint that is loaded.
	// This can help make debugging or building tools easier.

	size_t boneCount = _data->nodes_count;
	auto& o = _outInfo->jointNames;
	o.clear();
	o.resize(boneCount);

	for (int i = 0; i < boneCount; ++i) {
		const cgltf_node& node = _data->nodes[i];
		if (node.name == 0) {
			o[i] = "EMPTY NODE";
		}
		else {
			o[i] = node.name;
		}
	}
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

			// The[] operator of the Clip class either retrieves the current track or creates a new one.
			// This means the TransformTrack function for the node that you are parsing is always valid
			if (channel.target_path == cgltf_animation_path_type_translation) {
				VectorTrack& track = constCast(o[i][nodeIndex].position());
				GLTFHelpers::getTrackFromChannel<vec3, 3>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_rotation) {
				QuaternionTrack& track = constCast(o[i][nodeIndex].rotation());
				GLTFHelpers::getTrackFromChannel<quat, 4>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_scale) {
				VectorTrack& track = constCast(o[i][nodeIndex].scale());
				GLTFHelpers::getTrackFromChannel<vec3, 3>(track, channel);
			}
		}
		o[i].recalculateDuration();
	}
}

}