cmake_minimum_required(VERSION 3.7)

project(ssao)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}${PROJECT_NAME}")

add_library(ssao_lib STATIC ssao.cpp)

add_executable(ssao main.cpp)

target_include_directories(ssao PRIVATE
  ${ssao_SOURCE_DIR})
target_link_libraries(ssao base)
target_link_libraries(ssao ssao_lib)
