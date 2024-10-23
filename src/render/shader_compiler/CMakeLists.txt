set(PROJECT_NAME sge_shader_compiler)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

sge_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/render)
target_link_libraries(${PROJECT_NAME} PUBLIC sge_render spirv-cross-core spirv-cross-glsl)

if(WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E
			make_directory "${CMAKE_CURRENT_SOURCE_DIR}/../../../externals/tools" # TODO

		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${CMAKE_CURRENT_SOURCE_DIR}/../../../externals/vcpkg/installed/x64-windows/tools/shaderc/glslc.exe"
			"${CMAKE_CURRENT_SOURCE_DIR}/../../../externals/vcpkg/installed/x64-windows/tools/spirv-cross/spirv-cross.exe"
			"${CMAKE_CURRENT_SOURCE_DIR}/../../../externals/vcpkg/installed/x64-windows/tools/ninja/ninja.exe"
			"${CMAKE_CURRENT_SOURCE_DIR}/../../../externals/tools"
	)
endif()