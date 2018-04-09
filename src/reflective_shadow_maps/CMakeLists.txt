cmake_minimum_required(VERSION 3.7)

project(reflective_shadow_maps)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}${PROJECT_NAME}")

add_library(reflective_shadow_maps_lib STATIC reflective_shadow_maps.cpp)

add_executable(reflective_shadow_maps main.cpp)

target_include_directories(reflective_shadow_maps PRIVATE
  ${reflective_shadow_maps_SOURCE_DIR})
target_link_libraries(reflective_shadow_maps base)
target_link_libraries(reflective_shadow_maps reflective_shadow_maps_lib)
