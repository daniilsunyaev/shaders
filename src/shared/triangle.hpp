#pragma once

#include <GL/glew.h>
#include "shader.hpp"

class Triangle : public GeometryObject {
  public:
    Triangle(float tx, float ty, float tz);
    ~Triangle();
    void render(Shader& tShader);
  private:
    GLuint VAO;
    GLuint VBO;
    const float verts[9] = {
      0.0, 0.0, 0.0,
      -0.5, 1.0, 0.0,
      1.0, 0.0, 0.0
    };
};
