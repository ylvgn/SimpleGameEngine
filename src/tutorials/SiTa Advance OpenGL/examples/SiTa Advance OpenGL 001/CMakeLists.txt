set(PROJECT_NAME sge_sita_advance_opengl_001)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

sge_sita_advance_opengl_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/tutorials/sge_sita_advance_opengl/examples)

#add_compile_definitions(${PROJECT_NAME} PUBLIC GLEW_STATIC=1 VCPKG_TARGET_TRIPLET="x64-windows-static")
target_link_libraries(${PROJECT_NAME} PUBLIC sge_core PNG::PNG GLEW::GLEW)