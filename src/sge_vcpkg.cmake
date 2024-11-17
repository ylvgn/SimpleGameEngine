include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/sge_common.cmake) # !!<--- Must use fullpath include .cmake file

function(sge_vcpkg_find_python out_var_name require_python_version)
    sge_error_if_not_exist(Z_VCPKG_ROOT_DIR)
    set(SGE_VCPKG_TOOLS_PYTHON "${Z_VCPKG_ROOT_DIR}/downloads/tools/python")

    if (NOT DEFINED require_python_version)
        sge_log_error("please set 'require_python_version'")
    endif()

#   sge_dump_var(SGE_VCPKG_TOOLS_PYTHON require_python_version)

    File(GLOB src_files RELATIVE ${SGE_VCPKG_TOOLS_PYTHON} "${SGE_VCPKG_TOOLS_PYTHON}/python-*")

    foreach(FILE ${src_files})
        set(ABSOLUTE_FILE "${SGE_VCPKG_TOOLS_PYTHON}/${FILE}")
        if(NOT IS_DIRECTORY ${ABSOLUTE_FILE})
            continue()
        endif()
        sge_basename(out_basename ${FILE})
        sge_string_split(out_str_list ${out_basename} "-")

#       sge_dump_var(out_str_list)
        sge_list_at(out_py_version out_str_list 1)

#       sge_log("found python: ${out_py_version}")
        sge_string_split(out_str_list ${out_py_version} ".")
        sge_list_at(out_py_major_version out_str_list 0)

        if (${out_py_major_version} STREQUAL ${require_python_version})
            set(${out_var_name} ${ABSOLUTE_FILE} PARENT_SCOPE)
            sge_basename(out_basename ${CMAKE_CURRENT_FUNCTION_LIST_FILE})
#           sge_log("[${out_basename}]: choosed python ${ABSOLUTE_FILE}")
            return()
        endif()
    endforeach()
    sge_log_error("python${require_python_version} not found: SGE_VCPKG_TOOLS_PYTHON=${SGE_VCPKG_TOOLS_PYTHON}")
endfunction()