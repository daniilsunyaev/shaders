#include <GL/glew.h>
#include "geometry_object.hpp"
#include "triangle.hpp"
#include "shader.hpp"
#include <iostream>

Triangle::Triangle(float tx, float ty, float tz) :
  GeometryObject(tx,ty,tz) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

Triangle::~Triangle() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}

void Triangle::render(Shader& tShader) {
  tShader.use();
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
}
