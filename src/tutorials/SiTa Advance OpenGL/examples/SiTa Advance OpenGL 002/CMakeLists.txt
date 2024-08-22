set(PROJECT_NAME sge_sita_advance_opengl_002)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

sge_sita_advance_opengl_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/tutorials/sge_sita_advance_opengl/examples)

target_link_libraries(${PROJECT_NAME} PUBLIC sge_core PNG::PNG GLEW::GLEW)