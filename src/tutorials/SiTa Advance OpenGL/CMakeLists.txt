cmake_minimum_required(VERSION 3.18)

set(sge_sita_advance_opengl_root "${CMAKE_CURRENT_SOURCE_DIR}")

function(sge_sita_advance_opengl_add_executable target_name src_path)
#-- common source ---
	file(GLOB_RECURSE common_src_files
		"${sge_sita_advance_opengl_root}/src/*.*"
	)
	sge_source_group(${common_src_files} "${common_src_files}")

#-- current source ---
	file(GLOB_RECURSE src_files
		"${src_path}/src/*.*"
	)
	sge_source_group(${src_path} "${src_files}")

	add_executable(${target_name} ${common_src_files} ${src_files})
	target_precompile_headers(${target_name} PUBLIC src/${target_name}-pch.h)
	target_include_directories(${target_name} PRIVATE "${sge_sita_advance_opengl_root}/src")
    	target_link_libraries(${target_name} PUBLIC sge_core sge_sita_glew)
	sge_set_target_warning_level(${target_name})
	sge_set_target_unity_build_mode(${target_name})
endfunction()

#-- glew ---
add_subdirectory(externals/glew)

#-- examples ---
add_subdirectory(examples/SiTa\ Advance\ OpenGL\ 001)
add_subdirectory(examples/SiTa\ Advance\ OpenGL\ 002)
add_subdirectory(examples/SiTa\ Advance\ OpenGL\ 003)