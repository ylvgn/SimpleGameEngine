set(PROJECT_NAME sge_prog_win5)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

add_compile_definitions(UNICODE _UNICODE)

sge_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/books/sge_prog_win5)
target_link_libraries(${PROJECT_NAME} PUBLIC sge_core)