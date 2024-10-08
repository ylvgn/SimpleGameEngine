cmake_minimum_required(VERSION 3.18)

set(CMAKE_TOOLCHAIN_FILE "./externals/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_BUILD_TYPE Debug)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(gyh-SimpleGameEngine)

find_package(fmt CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(EABase CONFIG REQUIRED)
find_package(EASTL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(spirv_cross_core)
find_package(spirv_cross_glsl CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

function(sge_source_group src_path src_files)
	foreach(FILE ${src_files}) 
		get_filename_component(PARENT_DIR "${FILE}" PATH)
		file(RELATIVE_PATH PARENT_DIR ${src_path} ${PARENT_DIR})
		string(REPLACE "/" "\\" GROUP "${PARENT_DIR}")
		set(GROUP "${GROUP}")
		source_group("${GROUP}" FILES "${FILE}")	
	endforeach()
endfunction()

function(sge_set_target_warning_level target_name)
	if(MSVC)
	  target_compile_options(${target_name} PRIVATE /WX)     #warning treated as error
	  target_compile_options(${target_name} PRIVATE /W4)     #warning level 4
	  target_compile_options(${target_name} PRIVATE /wd4100) #warning C4100: unreferenced formal parameter in function
	  target_compile_options(${target_name} PRIVATE /wd4201) #warning C4201: nonstandard extension used: nameless struct/union
	  target_compile_options(${target_name} PRIVATE /wd4127) #warning C4127: conditional expression is constant
	  target_compile_options(${target_name} PRIVATE /wd4275) #warning C4275: non dll-interface class 'std::runtime_error' used as base for dll-interface class 'fmt::v10::format_error'
	else()
	  target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -Werror)
	endif()
endfunction()

function(sge_set_target_unity_build_mode target_name)
	set_target_properties(${target_name} PROPERTIES
							UNITY_BUILD ON
							UNITY_BUILD_MODE BATCH
							UNITY_BUILD_BATCH_SIZE 16
							)
endfunction()

function(sge_add_library target_name src_path)
	file(GLOB_RECURSE src_files
		"${src_path}/src/*.*"
	)	
	sge_source_group(${src_path} "${src_files}")
	
	add_library(${target_name} ${src_files})	
	target_precompile_headers(${target_name} PUBLIC src/${target_name}-pch.h)
	target_include_directories(${target_name} PUBLIC src)
	sge_set_target_warning_level(${target_name})
	sge_set_target_unity_build_mode(${target_name})
endfunction()

function(sge_add_dynamic_library target_name src_path)
	file(GLOB_RECURSE src_files
		"${src_path}/src/*.*"
	)	
	sge_source_group(${src_path} "${src_files}")
	
	add_library(${target_name} SHARED ${src_files})	
	target_precompile_headers(${target_name} PUBLIC src/${target_name}-pch.h)
	target_include_directories(${target_name} PUBLIC src)
	sge_set_target_warning_level(${target_name})
	sge_set_target_unity_build_mode(${target_name})
endfunction()

function(sge_add_executable target_name src_path)
	file(GLOB_RECURSE src_files
		"${src_path}/src/*.*"
	)
	sge_source_group(${src_path} "${src_files}")

	add_executable(${target_name} ${src_files})
	target_precompile_headers(${target_name} PUBLIC src/${target_name}-pch.h)
	sge_set_target_warning_level(${target_name})
	sge_set_target_unity_build_mode(${target_name})
endfunction()

#-- core ---
add_subdirectory(src/core)
add_subdirectory(src/core/test)

#-- render ---
add_subdirectory(src/render)
add_subdirectory(src/render/shader_compiler)

#-- engine ---
add_subdirectory(src/engine)

#-- editor ---
add_subdirectory(src/editor)
add_subdirectory(src/editor/app)
add_subdirectory(src/editor/app_opengl) # just for test

set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT sge_editor_app)

#--- inessential ---
#-- Hands-On C++ Game Animation Programming ---
add_subdirectory(src/books/Hands-On\ C++\ Game\ Animation\ Programming)
add_subdirectory(src/books/Hands-On\ C++\ Game\ Animation\ Programming/app)
add_subdirectory(src/books/Hands-On\ C++\ Game\ Animation\ Programming/test)

#-- Programming Windows 5th - Charles Petzold ---
add_subdirectory(src/books/Programming\ Windows\ 5th\ -\ Charles\ Petzold)

#-- NeHe OpenGL ---
add_subdirectory(src/tutorials/NeHe\ OpenGL)

#-- SiTa DataBase ---
add_subdirectory(src/tutorials/SiTa\ DataBase)
add_subdirectory(src/tutorials/SiTa\ DataBase/examples/sge_sita_db_example_001)
add_subdirectory(src/tutorials/SiTa\ DataBase/examples/sge_sita_db_example_002)

#-- SiTa Localization ---
add_subdirectory(src/tutorials/SiTa\ Localization)
add_subdirectory(src/tutorials/SiTa\ Localization/examples/sge_sita_locale_example_001)

#-- SiTa Ray-Tracing ---
add_subdirectory(src/tutorials/SiTa\ Ray-Tracing)
add_subdirectory(src/tutorials/SiTa\ Ray-Tracing/examples/sge_sita_ray_tracer_example_001)
add_subdirectory(src/tutorials/SiTa\ Ray-Tracing/examples/sge_sita_ray_tracer_example_002)
add_subdirectory(src/tutorials/SiTa\ Ray-Tracing/examples/sge_sita_ray_tracer_example_003)

#-- SiTa Advance OpenGL---
add_subdirectory(src/tutorials/SiTa\ Advance\ OpenGL)

#-- Learn OpenGL - Joey de Vries
add_subdirectory(src/books/Learn\ OpenGL\ -\ Joey\ de\ Vries)
add_subdirectory(src/books/Learn\ OpenGL\ -\ Joey\ de\ Vries/examples/sge_learnopengl_example_001)