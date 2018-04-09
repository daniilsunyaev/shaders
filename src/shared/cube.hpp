#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "geometry_object.hpp"
#include "shader.hpp"

class Cube : public GeometryObject {
  public:
    Cube(glm::mat4 tModel = glm::mat4(), glm::vec3 tColor = {-1,0,0});
    ~Cube();
    void render(Shader* tShader = nullptr);
  private:
    glm::mat4 mModel;
    glm::vec3 mColor;
    unsigned int mDiffuseTexture;
    static GLuint mVAO;
    static GLuint mVBO;
    static const float mVerts[36*8];
};
