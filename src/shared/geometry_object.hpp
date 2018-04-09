#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"

class GeometryObject {
  public:
    GeometryObject() {}
    virtual void render(Shader* tShader = nullptr) = 0;
    virtual ~GeometryObject() {}
};
