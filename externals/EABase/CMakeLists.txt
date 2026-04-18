set(PROJECT_NAME EABase)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME} CXX)

#-------------------------------------------------------------------------------------------
# Defines 
#-------------------------------------------------------------------------------------------
add_definitions(-D_CHAR16T)

add_library(${PROJECT_NAME} INTERFACE)
file(GLOB_RECURSE all_files "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")

target_sources(${PROJECT_NAME} PUBLIC ${all_files})
target_include_directories(${PROJECT_NAME} INTERFACE src/EABase)

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "externals")
