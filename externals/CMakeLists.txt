set(PROJECT_NAME sge_externals)
message("==== ${PROJECT_NAME} ====")
# project(${PROJECT_NAME}) # no need this

message(SGE_CMAKE_EXTERNALS_TOOLS=${SGE_CMAKE_EXTERNALS_TOOLS})

# https://cmake.org/cmake/help/latest/command/add_custom_target.html#command:add_custom_target
add_custom_target(${PROJECT_NAME} ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SGE_CMAKE_EXTERNALS_TOOLS}
)

if(WIN32)
    set(glslc "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows/tools/shaderc/glslc.exe")
    set(spirv_cross "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows/tools/spirv-cross/spirv-cross.exe")
    set(ninja "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows/tools/ninja/ninja.exe")
else()
    message(FATAL_ERROR "unsupported platform")
endif()

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge)

sge_copy_post_build(${PROJECT_NAME} ${glslc} ${spirv_cross} ${ninja} ${SGE_CMAKE_EXTERNALS_TOOLS})