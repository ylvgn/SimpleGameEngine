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

int getNodeIndex(const cgltf_node* target, cgltf_node* allNodes, size_t numNodes) {
	// perform a simple linear lookup by looping through all the nodes in a.gltf file
	// let say, you can find the parent of a node using the getNodeIndex helper function, which returns -1 if a node has no parent

	if (target == nullptr) {
		return -1; // invalid index
	}

	for (int i = 0; i < numNodes; ++i) {
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
	out.setInterpolation(interpolation);

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

cgltf_data* g_loadGLTFFile(StrView path) {
	// To load a file, you need to create an instance of cgltf_options.
	// You won't need to set any option flags; just instantiate the cgltf_options struct with 0 for all member values.
	cgltf_options options;
	memset(&options, 0, sizeof(cgltf_options));

	TempString s = path;
	cgltf_data* data = NULL;

	// load the glTF data from the file
	cgltf_result result = cgltf_parse_file(&options, s.c_str(), &data);
	if (result != cgltf_result_success) {
		SGE_LOG("Could not load input file: {}\n", s);
		return 0;
	}
	// cgltf_load_buffers is used to load any external buffer data
	result = cgltf_load_buffers(&options, data, s.c_str());
	if (result != cgltf_result_success) {
		cgltf_free(data);
		SGE_LOG("Could not load buffers for: {}\n", s);
		return 0;
	}
	// cgltf_validate makes sure that the glTF file that was just loaded was valid
	result = cgltf_validate(data);
	if (result != cgltf_result_success) {
		cgltf_free(data);
		SGE_LOG("Invalid gltf file: {}\n", s);
		return 0;
	}
	return data;
}

void g_freeGLTFFile(cgltf_data* data) {
	if (data == 0) {
		SGE_LOG("WARNING: Can't free null data");
	} else {
		cgltf_free(data);
	}
}

Pose g_loadRestPose(cgltf_data* data) {
	// The animation system you will build in this book does not support joint lookup by name, only index.

	cgltf_size boneCount = data->nodes_count;
	Pose res(boneCount);

	for (int i = 0; i < boneCount; ++i) {
		const cgltf_node& node = data->nodes[i];

		Transform transform = GLTFHelpers::getLocalTransform(data->nodes[i]);
		res.setLocalTransform(i, transform);

		int parentIndex = GLTFHelpers::getNodeIndex(node.parent, data->nodes, boneCount);
		res.setParent(i, parentIndex);
	}

	return res;
}

Vector<String> g_loadJointNames(cgltf_data* data) {
	// At some point, you might want to know the name assigned to each joint that is loaded.This can help make debugging or building tools easier.

	size_t boneCount = data->nodes_count;
	Vector<String> res;
	res.resize(boneCount);

	for (int i = 0; i < boneCount; ++i) {
		cgltf_node* node = &data->nodes[i];
		if (node->name == 0) {
			res[i] = "EMPTY NODE";
		} else {
			res[i] = node->name;
		}
	}
	return res;
}

Vector<Clip> g_loadAnimationClips(cgltf_data* data) {

	/*
	loop through all the clips in the provided gltf_data its "animation" node
	let say a .gltf file (json format), like

		{
			"nodes" : [
				...
			],
			...
			"animation" : [
				{
					"name" : "Walking",
					"channels" : [
						{
							"sampler" : 0,
							"target" : {
								"node" : 40,
								"path" : "rotation"
							},
						},
						...
					],
					"samplers" : [
						...
					]
				}
			],
			...
		}
	*/
	cgltf_size clipCount = data->animations_count;
	cgltf_size nodeCount = data->nodes_count;

	Vector<Clip> res;
	res.resize(clipCount);
	for (int i = 0; i < clipCount; ++i) {
		const cgltf_animation& animation = data->animations[i];
		// For every clip, set its name. 
		res[i].setName(animation.name);

		// loop through all of the channels in the clip, each channel of a glTF file is an animation track.
		cgltf_size channelCount = animation.channels_count;
		for (int j = 0; j < channelCount; ++j) {
			const cgltf_animation_channel& channel = animation.channels[j];

			// find the index of the node that the current channel affects
			const cgltf_node* target = channel.target_node;
			int nodeIndex = GLTFHelpers::getNodeIndex(target, data->nodes, nodeCount);

			// The[] operator of the Clip class either retrieves the current track or creates a new one.This means the TransformTrack function for the node that you are parsing is always valid 
			if (channel.target_path == cgltf_animation_path_type_translation) {
				VectorTrack& track = constCast(res[i][nodeIndex].position());
				GLTFHelpers::getTrackFromChannel<vec3, 3>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_rotation) {
				QuaternionTrack& track = constCast(res[i][nodeIndex].rotation());
				GLTFHelpers::getTrackFromChannel<quat, 4>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_scale) {
				VectorTrack& track = constCast(res[i][nodeIndex].scale());
				GLTFHelpers::getTrackFromChannel<vec3, 3>(track, channel);
			}
		}
		res[i].recalculateDuration();
	}

	return res;
}

}