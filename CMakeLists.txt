cmake_minimum_required(VERSION 3.16)
project(GuessGameCoroutines LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(guess_game main.cpp GuessGame.cpp Players.cpp)

if(MSVC)
    target_compile_options(guess_game PRIVATE /W4 /utf-8)
else()
    target_compile_options(guess_game PRIVATE -Wall -Wextra -pedantic)
endif()

add_custom_command(TARGET guess_game POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/games.txt" "${CMAKE_SOURCE_DIR}/games_with_errors.txt" "$<TARGET_FILE_DIR:guess_game>" VERBATIM)

set_property(TARGET guess_game PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:guess_game>)