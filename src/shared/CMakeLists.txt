cmake_minimum_required(VERSION 3.7)

project(shared)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)


add_library(camera STATIC camera.cpp camera.hpp)
target_include_directories(camera PRIVATE ${OPENGL_INCLUDE_DIR}
  PRIVATE ${GLEW_INCLUDE_DIRS})
target_link_libraries(camera ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})

add_library(shader STATIC shader.cpp shader.hpp)
target_include_directories(shader PRIVATE ${OPENGL_INCLUDE_DIR}
  PRIVATE ${GLEW_INCLUDE_DIRS})
target_link_libraries(shader ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})

add_library(geometry STATIC
  plane.hpp plane.cpp cube.hpp cube.cpp sphere.hpp sphere.cpp)
target_include_directories(geometry PRIVATE ${OPENGL_INCLUDE_DIR}
  PRIVATE ${GLEW_INCLUDE_DIRS})
target_link_libraries(geometry ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
target_link_libraries(geometry shader)
