cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

project(PoleChudes LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git
                     GIT_TAG 5.5)

set(S_TRUCTURES_BUILD_TESTS OFF CACHE INTERNAL "")
FetchContent_Declare(
    S_tructures GIT_REPOSITORY https://github.com/Schwungus/S_tructures.git
    GIT_TAG 9b1aae894a4f592125d1fb50e66787303cbe4a44)

set(NUTPUNCH_BUILD_TESTS OFF CACHE INTERNAL "")
set(NUTPUNCH_BUILD_SERVER OFF CACHE INTERNAL "")
set(NUTPUNCH_BUILD_LOBBYIST OFF CACHE INTERNAL "")
FetchContent_Declare(
    nutpunch GIT_REPOSITORY https://github.com/Schwungus/nutpunch.git
    GIT_TAG a25d9b7bf1dba36130512c5a7eb560622eab9873)

FetchContent_MakeAvailable(raylib S_tructures nutpunch)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${SRC_DIR}/*.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib nutpunch S_tructures)

add_custom_target(
    CopyAssets ALL
    COMMAND
        ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(${PROJECT_NAME} CopyAssets)
