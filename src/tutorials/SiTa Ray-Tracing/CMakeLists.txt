set(PROJECT_NAME sge_sita_ray_tracer)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

sge_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/tutorials/sge_sita_ray_tracing)
target_link_libraries(${PROJECT_NAME} PUBLIC sge_core)
