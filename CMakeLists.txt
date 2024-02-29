cmake_minimum_required(VERSION 3.15)
project(cuda-opengl C CXX)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-D_USE_MATH_DEFINES)

set(THIRD_PARTY_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw-master/include"
                                    "${CMAKE_CURRENT_SOURCE_DIR}/deps/glew/include"
                                    "${CMAKE_CURRENT_SOURCE_DIR}/deps/entt"
                                    "${CMAKE_CURRENT_SOURCE_DIR}/deps/glm")
set(THIRD_PARTY_LIBRARIES glfw libglew_static)

add_definitions(-DGLEW_STATIC)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw-master" glfw)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/glew" glew)

file(GLOB air_core_src
     "src/core/*.cpp"
)

file(GLOB air_core_head
     "src/core/*.h"
)

file(GLOB air_renderer_src
     "src/renderer/*.cpp"
)

file(GLOB air_renderer_head
     "src/renderer/*.h"
)

add_executable(cuda-opengl src/main.cpp ${air_core_src} ${air_core_head} ${air_renderer_src} ${air_renderer_head})

target_link_libraries(cuda-opengl PRIVATE ${THIRD_PARTY_LIBRARIES})
target_include_directories(cuda-opengl PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES} ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})