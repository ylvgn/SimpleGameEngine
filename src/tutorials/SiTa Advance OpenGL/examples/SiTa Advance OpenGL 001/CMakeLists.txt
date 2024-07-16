set(PROJECT_NAME sge_sita_advance_opengl_001)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

sge_sita_advance_opengl_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/tutorials/sge_sita_advance_opengl)

target_link_libraries(${PROJECT_NAME} PUBLIC sge_core png)

# ----------- not work: link error when use vcpkg glew ------------------------------- why ???
## Solution 1
#target_link_libraries(${PROJECT_NAME} PUBLIC sge_core png GLEW::GLEW)

## Solution 2
#target_link_libraries(${PROJECT_NAME} PUBLIC sge_core png)
#target_link_libraries(${PROJECT_NAME} PUBLIC "$<LINK_LIBRARY:WHOLE_ARCHIVE,GLEW::GLEW>") # whole archive static lib
# ----------- not work, just manually dl glew source and compile it myself -------------------------------