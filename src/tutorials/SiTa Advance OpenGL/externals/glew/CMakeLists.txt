set(PROJECT_NAME sge_sita_glew)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

#add_compile_definitions(GLEW_USE_STATIC_LIBS GLEW_STATIC) # no need this

sge_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
	add_definitions("/wd4456")
endif(WIN32)

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/tutorials/sge_sita_advance_opengl/externals)