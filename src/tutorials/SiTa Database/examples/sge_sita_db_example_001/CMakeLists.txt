set(PROJECT_NAME sge_sita_db_example_001)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

sge_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/tutorials/sge_sita_db/examples)
target_link_libraries(${PROJECT_NAME} PUBLIC sge_sita_db)

if(WIN32)
	sge_copy_files_post_build(${PROJECT_NAME}
#		"${CMAKE_CURRENT_BINARY_DIR}/../../$<CONFIG>/sge_sita_db.dll" no need now
		"${CMAKE_CURRENT_SOURCE_DIR}/../../externals/MySQL Connector C 6.1/${CMAKE_VS_PLATFORM_NAME}/libmysql.dll"
	#-------
		$<TARGET_FILE_DIR:${PROJECT_NAME}>
	)
else()
	message(FATAL_ERROR "unsupported platform")
endif()