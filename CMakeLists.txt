cmake_minimum_required(VERSION 3.15)
project(Lab C CXX)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-D_USE_MATH_DEFINES)

set(THIRD_PARTY_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw-master/include"
                                    "${CMAKE_CURRENT_SOURCE_DIR}/deps/glew/include"
                                    "${CMAKE_CURRENT_SOURCE_DIR}/deps/glm"
                                    "${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui"
                                    "${CMAKE_CURRENT_SOURCE_DIR}/deps/mpi/Include")
set(THIRD_PARTY_LIBRARIES glfw libglew_static msmpi)

add_definitions(-DGLEW_STATIC)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw-master" glfw)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/glew" glew)

file(GLOB air_core_src
     "src/core/*.cpp"
)

file(GLOB air_core_head
     "src/core/*.h"
)


file(GLOB imgui
     "deps/imgui/*.cpp"
)

add_executable(Lab src/main.cpp src/GridTexture.h src/Solution2.h src/Application.h ${air_core_src} ${air_core_head} ${imgui})

target_link_directories(Lab PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/deps/mpi/Lib/x64")
target_link_libraries(Lab PRIVATE ${THIRD_PARTY_LIBRARIES})
target_include_directories(Lab PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES} ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})