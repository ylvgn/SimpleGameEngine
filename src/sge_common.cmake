include_guard(GLOBAL)

# https://cmake.org/cmake/help/latest/manual/cmake-variables.7.html
#message("CMAKE_SYSTEM_NAME          = ${CMAKE_SYSTEM_NAME}")
#message("CMAKE_GENERATOR_PLATFORM   = ${CMAKE_GENERATOR_PLATFORM}")
#message("CMAKE_GENERATOR_TOOLSET    = ${CMAKE_GENERATOR_TOOLSET}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
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
							UNITY_BUILD_BATCH_SIZE 16)
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

function(sge_dump_var)
    foreach(i RANGE ${ARGC})
        message("ARGV${i} = ${ARGV${i}}")
    endforeach()
endfunction()

function(sge_copy_post_build target_name)
# https://cmake.org/cmake/help/latest/command/add_custom_command.html#build-events
# https://cmake.org/cmake/help/latest/command/macro.html#arguments

#   sge_dump_ARGN(${ARGN})

    if (${ARGC} LESS 2)
        message(FATAL_ERROR
            "'${target_name}' sge_copy_post_build error: without provide <file>... <destination>"
            "\nsyntax:\n\sge_copy_post_build <target> <file>... <destination>\n"
            "\nexample:\n\sge_copy_post_build ${target_name} foo1.txt foo2.txt bar/my_dst\n"
        )
    endif()
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARGN}
    )
endfunction()