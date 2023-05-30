set(PROJECT_NAME sge_game_anime_prog_test)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

sge_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/books/game_anime_prog)
target_link_libraries(${PROJECT_NAME} PUBLIC sge_game_anime_prog)