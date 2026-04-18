# glad src dl: https://glad.dav1d.de/#language=c&specification=gl&api=gl%3D3.3&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&profile=compatibility&loader=on
set(PROJECT_NAME glad)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME} LANGUAGES C)

file(GLOB_RECURSE all_files "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")
add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} PUBLIC ${all_files})
target_include_directories(${PROJECT_NAME} PUBLIC src/include)

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "externals")