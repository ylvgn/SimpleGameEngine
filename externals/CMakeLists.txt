include(../src/sge_vcpkg.cmake)

set(PROJECT_NAME _sge_externals)
message("==== ${PROJECT_NAME} ====")
# project(${PROJECT_NAME}) # !!<--- no need this

# https://cmake.org/cmake/help/latest/command/add_custom_target.html#command:add_custom_target
add_custom_target(${PROJECT_NAME} ALL)
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge)

#-- vcpkg tools ---
if(WIN32)
    set(sge_glslc "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows/tools/shaderc/glslc.exe")
    set(sge_spirv_cross "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows/tools/spirv-cross/spirv-cross.exe")
    set(sge_ninja "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows/tools/ninja/ninja.exe")
else()
    sge_log_error("unsupported platform")
endif()

sge_copy_files_post_build(${PROJECT_NAME}
    ${sge_glslc}
    ${sge_spirv_cross}
    ${sge_ninja}
#----
    ${SGE_RUNTIME_OUTPUT_DIRECTORY_CONFIGURATION}
)

#-- vcpkg python3 ---
sge_vcpkg_find_python(sge_python3 3)
sge_copy_dir_post_build(${PROJECT_NAME} ${sge_python3} ${SGE_RUNTIME_OUTPUT_DIRECTORY_CONFIGURATION}/python3)
