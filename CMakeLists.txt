cmake_minimum_required(VERSION 2.6)
project(Fireworks)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(SDL2_image REQUIRED)
include_directories(${SDL2IMAGE_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_PATH})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

add_definitions(-g -pedantic -Wall -Werror -DDATADIR="data")

#set(CMAKE_C_FLAGS "-std=gnu11")
set(CMAKE_CXX_FLAGS "-std=gnu++11")

file(GLOB_RECURSE src1 "src/*" TRUE)
file(GLOB_RECURSE src2 "src/*/*" TRUE)
file(GLOB_RECURSE src3 "src/*/*/*" TRUE)

add_executable(fireworks ${src1} ${src2} ${src3})

target_link_libraries(fireworks ${SDL2_LIBRARY})
target_link_libraries(fireworks ${SDL2IMAGE_LIBRARY})
target_link_libraries(fireworks ${OPENGL_LIBRARY})
target_link_libraries(fireworks ${GLEW_LIBRARY})
target_link_libraries(fireworks ${FREETYPE_LIBRARIES})
target_link_libraries(fireworks ${PNG_LIBRARY})
target_link_libraries(fireworks m)
