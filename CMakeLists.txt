cmake_minimum_required(VERSION 3.10)
project(f9ay_lib CXX)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(f9ay_lib SHARED ${SOURCES})

target_include_directories(f9ay_lib
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${PROJECT_SOURCE_DIR}/lib/include
)


if (WIN32)
    target_compile_definitions(f9ay_lib PRIVATE F9AY_LIB_EXPORTS)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(f9ay_lib PRIVATE /utf-8)
endif ()

