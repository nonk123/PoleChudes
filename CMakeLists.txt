cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

project(PoleChudes)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5)

set(STEAMWORKS_SDK_ZIP ${CMAKE_SOURCE_DIR}/steamworks_sdk_162.zip)
set(STEAM_APPID ${CMAKE_SOURCE_DIR}/steam_appid.txt)
set(CAULK_BUILD_TEST OFF)

FetchContent_Declare(
    caulk
    GIT_REPOSITORY https://github.com/Schwungus/caulk.git
    GIT_TAG master) # TODO: pin to a specific version

FetchContent_MakeAvailable(raylib caulk)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${SRC_DIR}/*.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib caulk)
caulk_register(${PROJECT_NAME})

add_custom_target(CopyAssets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets
                                     ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(${PROJECT_NAME} CopyAssets)
