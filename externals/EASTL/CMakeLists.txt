set(PROJECT_NAME EASTL)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME} CXX)

#-------------------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------------------
option(EASTL_BUILD_BENCHMARK "Enable generation of build files for benchmark" OFF)
option(EASTL_BUILD_TESTS "Enable generation of build files for tests" OFF)
option(EASTL_STD_ITERATOR_CATEGORY_ENABLED "Enable compatibility with std:: iterator categories" OFF)
option(EASTL_DISABLE_APRIL_2024_DEPRECATIONS "Enable use of API marked for removal in April 2024." OFF)
option(EASTL_DISABLE_SEPT_2024_DEPRECATIONS "Enable use of API marked for removal in September 2024." OFF)
option(EASTL_DISABLE_APRIL_2025_DEPRECATIONS "Enable use of API marked for removal in April 2025." OFF)

file(GLOB_RECURSE all_files "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")
add_library(${PROJECT_NAME} STATIC)
target_compile_features(EASTL PUBLIC cxx_std_14)

target_include_directories(${PROJECT_NAME} PUBLIC src/EASTL)

target_sources(${PROJECT_NAME} PUBLIC ${all_files})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "externals")

target_link_libraries(${PROJECT_NAME} PRIVATE EABase)

#-------------------------------------------------------------------------------------------
# Defines
#-------------------------------------------------------------------------------------------
add_definitions(-D_CHAR16T)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-DEASTL_OPENSOURCE=1)
if (EASTL_STD_ITERATOR_CATEGORY_ENABLED)
  add_definitions(-DEASTL_STD_ITERATOR_CATEGORY_ENABLED=1)
endif()

#-------------------------------------------------------------------------------------------
# Deprecations
#-------------------------------------------------------------------------------------------
if(EASTL_DISABLE_APRIL_2024_DEPRECATIONS)
  target_compile_definitions(EASTL PUBLIC EA_DEPRECATIONS_FOR_2024_APRIL=EA_DISABLED)
endif()
if(EASTL_DISABLE_SEPT_2024_DEPRECATIONS)
  target_compile_definitions(EASTL PUBLIC EA_DEPRECATIONS_FOR_2024_SEPT=EA_DISABLED)
endif()
if(EASTL_DISABLE_APRIL_2025_DEPRECATIONS)
  target_compile_definitions(EASTL PUBLIC EA_DEPRECATIONS_FOR_2025_APRIL=EA_DISABLED)
endif()