#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "geometry_object.hpp"
#include "shader.hpp"

class Sphere : public GeometryObject {
  public:
    Sphere(glm::mat4 tModel = glm::mat4(), glm::vec3 tColor = {-1,0,0});
    ~Sphere();
    void render(Shader* tShader = nullptr);
  private:
    glm::mat4 mModel;
    glm::vec3 mColor;
    unsigned int mDiffuseTexture;
    static GLuint mVAO;
    static GLuint mVBO;
    static GLuint mEBO;
    static const int THETAS = 16;
    static const int PHIS = 16;
    static float mVerts[(THETAS+1)*(PHIS+1)*8];
    static std::vector<unsigned int> mIndices;

    void generateVertexes();
};
