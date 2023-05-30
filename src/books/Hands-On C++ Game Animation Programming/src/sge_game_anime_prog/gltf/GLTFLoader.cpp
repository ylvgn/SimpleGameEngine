#include "GLTFLoader.h"

namespace sge {

cgltf_data* g_LoadGLTFFile(StrView path) {
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

void g_FreeGLTFFile(cgltf_data* data) {
	if (data == 0) {
		SGE_LOG("WARNING: Can't free null data");
	}
	else {
		cgltf_free(data);
	}
}

}