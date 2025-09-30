cmake_minimum_required(VERSION 3.11.0)
project(FFTbench VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
cmake_policy(SET CMP0072 NEW)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MAKE_PROGRAM FFTbench)

file(GLOB_RECURSE APP_SOURCES "${CMAKE_SOURCE_DIR}/src/*.c*")

add_executable(FFTbench ${APP_SOURCES})
target_include_directories(FFTbench PRIVATE ${CMAKE_SOURCE_DIR}/include)
