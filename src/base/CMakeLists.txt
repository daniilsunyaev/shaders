cmake_minimum_required(VERSION 3.7)

project(base)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_library(base STATIC app.cpp app.hpp camera_app.cpp camera_app.hpp)

target_include_directories(base PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(base PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(base PUBLIC ${GLEW_INCLUDE_DIRS})
target_link_libraries(base ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES})

target_link_libraries(base camera shader geometry)
