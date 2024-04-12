set(PROJECT_NAME sge_nehe_ogl)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

add_compile_definitions(UNICODE _UNICODE)

sge_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/tutorials/sge_nehe_ogl)
target_link_libraries(${PROJECT_NAME} PUBLIC sge_core png)