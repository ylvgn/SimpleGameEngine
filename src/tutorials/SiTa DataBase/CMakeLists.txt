set(PROJECT_NAME sge_sita_db)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/externals/MySQL Connector C 6.1/${CMAKE_VS_PLATFORM_NAME}")

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/externals/MySQL Connector C 6.1/include"
)

sge_add_dynamic_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/tutorials/sge_sita_db)
target_link_libraries(${PROJECT_NAME} PUBLIC sge_core)