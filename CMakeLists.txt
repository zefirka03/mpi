cmake_minimum_required(VERSION 3.15)
project(cuda-opengl C CXX)

add_definitions(-D_USE_MATH_DEFINES)

set(THIRD_PARTY_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw-master/include"
                                    "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw-master/deps")
set(THIRD_PARTY_LIBRARIES glfw)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw-master" glfw)

add_executable(cuda-opengl main.cpp gl_includes.h)

target_link_libraries(cuda-opengl PRIVATE ${THIRD_PARTY_LIBRARIES})
target_include_directories(cuda-opengl PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES} ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})