set(PROJECT_NAME sge_sita_sqlite3)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

sge_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#	target_compile_definitions(${PROJECT_NAME} PRIVATE /wd4206 /wd4505) # not work
	add_definitions("/wd4206" "/wd4505")
endif()

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/tutorials/sge_sita_db/externals)