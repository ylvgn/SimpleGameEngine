cmake_minimum_required(VERSION 3.18)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/externals/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_BUILD_TYPE Debug)

include(src/sge_common.cmake)

set(PROJECT_NAME gyh-SimpleGameEngine)
project(${PROJECT_NAME})
message("==== ${PROJECT_NAME} ====")
#sge_dump_var(VCPKG_TARGET_TRIPLET)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/examples/_bin/${VCPKG_TARGET_TRIPLET}")
#sge_dump_var(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

if (CMAKE_CONFIGURATION_TYPES)
    set(SGE_RUNTIME_OUTPUT_DIRECTORY_CONFIGURATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
else()
    set(SGE_RUNTIME_OUTPUT_DIRECTORY_CONFIGURATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

#sge_dump_cmake_variables()

find_package(fmt CONFIG REQUIRED)
find_package(EABase CONFIG REQUIRED)
find_package(EASTL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(spirv_cross_core CONFIG REQUIRED)
find_package(spirv_cross_glsl CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

#-- core ---
add_subdirectory(src/core)
add_subdirectory(src/core/test)

#-- render ---
add_subdirectory(src/render)
add_subdirectory(src/render/shader_compiler)

#-- engine ---
add_subdirectory(src/engine)

#-- editor ---
add_subdirectory(src/editor)
add_subdirectory(src/editor/app)

#-- tools ---
add_subdirectory(src/tools/file_cmd)

#-- externals ---
add_subdirectory(externals)

set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT sge_editor_app) # Set as Startup Project


#-- inessential ---
#-- Hands-On C++ Game Animation Programming ---
add_subdirectory(src/books/Hands-On\ C++\ Game\ Animation\ Programming)
add_subdirectory(src/books/Hands-On\ C++\ Game\ Animation\ Programming/app)
add_subdirectory(src/books/Hands-On\ C++\ Game\ Animation\ Programming/test)

#-- Programming Windows 5th - Charles Petzold ---
add_subdirectory(src/books/Programming\ Windows\ 5th\ -\ Charles\ Petzold)

#-- NeHe OpenGL ---
add_subdirectory(src/tutorials/NeHe\ OpenGL)

#-- SiTa DataBase ---
add_subdirectory(src/tutorials/SiTa\ DataBase)
add_subdirectory(src/tutorials/SiTa\ DataBase/examples/sge_sita_db_example_001)
add_subdirectory(src/tutorials/SiTa\ DataBase/examples/sge_sita_db_example_002)

#-- SiTa Localization ---
add_subdirectory(src/tutorials/SiTa\ Localization)
add_subdirectory(src/tutorials/SiTa\ Localization/examples/sge_sita_locale_example_001)

#-- SiTa Ray-Tracing ---
add_subdirectory(src/tutorials/SiTa\ Ray-Tracing)
add_subdirectory(src/tutorials/SiTa\ Ray-Tracing/examples/sge_sita_ray_tracer_example_001)
add_subdirectory(src/tutorials/SiTa\ Ray-Tracing/examples/sge_sita_ray_tracer_example_002)
add_subdirectory(src/tutorials/SiTa\ Ray-Tracing/examples/sge_sita_ray_tracer_example_003)

#-- SiTa Advance OpenGL ---
add_subdirectory(src/tutorials/SiTa\ Advance\ OpenGL)

#-- Learn OpenGL - Joey de Vries ---
add_subdirectory(src/books/Learn\ OpenGL\ -\ Joey\ de\ Vries)
add_subdirectory(src/books/Learn\ OpenGL\ -\ Joey\ de\ Vries/examples/sge_learnopengl_example_001)
add_subdirectory(src/books/Learn\ OpenGL\ -\ Joey\ de\ Vries/examples/sge_learnopengl_example_002)