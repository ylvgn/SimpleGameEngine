include_guard(GLOBAL)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(sge_log)
	message(${ARGN})
endfunction()

function(sge_log_warning)
	message(WARNING ${ARGN}"warning: file ${CMAKE_CURRENT_LIST_FILE}, line ${CMAKE_CURRENT_LIST_LINE}")
endfunction()

function(sge_log_error)
	message(FATAL_ERROR ${ARGN}"error: file ${CMAKE_CURRENT_LIST_FILE}, line ${CMAKE_CURRENT_LIST_LINE}")
endfunction()

function(sge_error_if_not_exist)
	if (NOT ${ARGC} EQUAL 1)
		sge_log_warning("only allow 1 parameter")
	endif()

	set(KEY ${ARGV0})
	set(VALUE ${${KEY}})
	if (NOT EXISTS ${VALUE})
		sge_log_error("path not found: ${KEY}=[${VALUE}]")
	endif()
endfunction()

function(sge_dump_var)
	foreach(i RANGE ${ARGC})
		if (i EQUAL ${ARGC})
			break()
		endif()
		set(KEY ${ARGV${i}})
		set(VALUE ${${KEY}})
		sge_log(STATUS "DUMP_VAR: ${KEY} = [${VALUE}]") # 'STATUS' is compatible for list variable!!
	endforeach()
endfunction()

function(sge_dump_cmake_variables)
# https://cmake.org/cmake/help/latest/manual/cmake-variables.7.html
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        if (ARGV0)
            unset(MATCHED)
            string(REGEX MATCH ${ARGV0} MATCHED ${_variableName})
            if (NOT MATCHED)
                continue()
            endif()
        endif()
        sge_log(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endfunction()

function(sge_write_to_file filename text)
#	sge_log("sge_write_to_file(\"${filename}\")")
	file(GENERATE OUTPUT ${filename} CONTENT "${text}" NEWLINE_STYLE UNIX)	
endfunction()

function(sge_basename out_var_name in_path)
	get_filename_component(basename ${in_path} NAME)
	set(${out_var_name} ${basename} PARENT_SCOPE)
endfunction()

function(sge_string_split out_var_name in_str in_delimiter)
	string(REPLACE ${in_delimiter} ";" out_list ${in_str})
	set(${out_var_name} ${out_list} PARENT_SCOPE)
endfunction()

function(sge_list_at out_var_name in_list index)
# e.g. sge_list_at(out_result my_str_list 1) --> Aware NOT Use This: sge_list_at(out_result ${my_str_list} 1)
	list(GET ${in_list} ${index} out_element)
	set(${out_var_name} ${out_element} PARENT_SCOPE)
endfunction()

function(sge_dirname out_var_name in_path)
	get_filename_component(out_dirname ${in_path} PATH)
	set(${out_var_name} ${out_dirname} PARENT_SCOPE)
endfunction()

function(sge_realpath out_var_name in_path)
	get_filename_component(out_dirname ${in_path} ABSOLUTE)
	set(${out_var_name} ${out_dirname} PARENT_SCOPE)
endfunction()

function(sge_output_copy_dir in_output_name in_src_dir in_dst_dir)
	add_custom_command(
		OUTPUT ${in_output_name}
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${in_src_dir} ${in_dst_dir}
    )
endfunction()

function(sge_output_copy_files in_output_name)
	if (${ARGC} LESS 2)
		sge_log_error(
			"'${in_output_name}' sge_output_copy_files error: without provide <file>... <destination>"
			"\nsyntax:\n\sge_output_copy_files(<output> <file>... <destination>)\n"
			"\nexample:\n\sge_output_copy_files(${in_output_name} foo1.txt foo2.txt bar/my_dst)\n"
		)
	endif()
	add_custom_command(
		OUTPUT ${in_output_name}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARGN}
    )
endfunction()

function(sge_output_mkdir in_output_name in_dir)
	add_custom_target(
		OUTPUT ${in_output_name}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${in_dir}
	)
endfunction()

function(sge_add_cmd_post_build target_name)
	add_custom_command(
		TARGET ${target_name} POST_BUILD
		COMMAND ${ARGN}
	)
endfunction()

function(sge_copy_files_post_build target_name)
#	sge_dump_ARGN(${ARGN})
    if (${ARGC} LESS 2)
		sge_log_error(
            "'${target_name}' sge_copy_files_post_build error: without provide <file>... <destination>"
            "\nsyntax:\n\sge_copy_files_post_build(<target> <file>... <destination>)\n"
            "\nexample:\n\sge_copy_files_post_build(${target_name} foo1.txt foo2.txt bar/my_dst)\n"
        )
    endif()
    # add_custom_command(TARGET ${target_name} POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARGN}
    # )
	sge_add_cmd_post_build(${target_name} COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARGN})
endfunction()

function(sge_copy_dir_post_build target_name src_dir dst_dir)
	add_custom_command(
		TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${src_dir} ${dst_dir}
    )
endfunction()

function(sge_source_group src_path src_files)
	foreach(FILE ${src_files})
		#get_filename_component(PARENT_DIR "${FILE}" PATH)
		sge_dirname(PARENT_DIR ${FILE})
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
