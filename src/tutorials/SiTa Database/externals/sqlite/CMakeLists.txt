set(PROJECT_NAME sge_sita_sqlite3)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

sge_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
#	target_compile_definitions(${PROJECT_NAME} PRIVATE /wd4206) # not work
	add_definitions("/wd4206" "/wd4505")
endif(WIN32)

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/tutorials/sge_sita_db/externals)