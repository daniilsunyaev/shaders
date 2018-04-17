#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "sphere.hpp"
#include "shader.hpp"
#include <vector>
#include <iostream>

Sphere::Sphere(glm::mat4 tModel, glm::vec3 tColor) : mModel(tModel), mColor(tColor) {
  if(mVAO == 0) {
    generateVertexes();

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(mVerts), mVerts, GL_STATIC_DRAW);
    //  glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float),
     //     &data[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int),
        &mIndices[0], GL_STATIC_DRAW);
    float stride = (3+2+3)*sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  glGenTextures(1, &mDiffuseTexture);
  glBindTexture(GL_TEXTURE_2D, mDiffuseTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1, 1, 0, GL_RGB, GL_FLOAT, &tColor);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Sphere::~Sphere() {
  if(mVAO > 0) {
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    mVAO = 0;
    mVBO = 0;
  }
}

void Sphere::generateVertexes() {
  const float PI = 3.14159265359;
  std::vector<glm::vec3> positions;
  std::vector<glm::vec2> uv;
  std::vector<glm::vec3> normals;

  for(int t=0; t<=THETAS; t++) {
    float vSegment = (float)t / (float)THETAS;
    for(int p=0; p<=PHIS; p++) {
      float uSegment = (float)p / (float)PHIS;

      float xPos = std::cos(uSegment * 2.0f * PI) * std::sin(vSegment * PI);
      float yPos = std::cos(vSegment * PI);
      float zPos = std::sin(uSegment * 2.0f * PI) * std::sin(vSegment * PI);

      positions.push_back(glm::vec3(xPos, yPos, zPos));
      uv.push_back(glm::vec2(uSegment, vSegment));
      normals.push_back(glm::vec3(xPos, yPos, zPos));
    }
  }

  bool oddRow = false;
  for (int t = 0; t <= THETAS; t++) {
    if (!oddRow) {
      for (int p = 0; p <= PHIS; p++) {
        mIndices.push_back(t * (PHIS + 1) + p);
        mIndices.push_back((t + 1) * (PHIS + 1) + p);
      }
    } else {
      for (int p = PHIS; p >= 0; p--) {
        mIndices.push_back((t + 1) * (PHIS + 1) + p);
        mIndices.push_back(t * (PHIS + 1) + p);
      }
    }
    oddRow = !oddRow;
  }

  std::vector<float> data;
  for(int i = 0; i < positions.size(); ++i) {
    mVerts[i*8] = positions[i].x;
    mVerts[i*8+1] = positions[i].y;
    mVerts[i*8+2] = positions[i].z;

    mVerts[i*8+3] = normals[i].x;
    mVerts[i*8+4] = normals[i].y;
    mVerts[i*8+5] = normals[i].z;

    mVerts[i*8+6] = uv[i].x;
    mVerts[i*8+7] = uv[i].y;

    ////if (normals.size() > 0) {
    //  data.push_back(normals[i].x);
    //  data.push_back(normals[i].y);
    //  data.push_back(normals[i].z);
    ////}
    //if (uv.size() > 0) {
    //  data.push_back(uv[i].u);
    //  data.push_back(uv[i].v);
    //}
  }
}

void Sphere::render(Shader* tShader) {
  if(tShader!=nullptr) {
    tShader->use();
    tShader->setMat4("model", mModel);

    if(mColor.r >= 0) {
      tShader->setInt("diffuse", 0);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mDiffuseTexture);
    }
  }

  auto sphereIndexCount = mIndices.size();
  glBindVertexArray(mVAO);
  glDrawElements(GL_TRIANGLE_STRIP, sphereIndexCount, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
}

GLuint Sphere::mVAO = 0;
GLuint Sphere::mVBO = 0;
GLuint Sphere::mEBO = 0;
float Sphere::mVerts[(THETAS+1)*(PHIS+1)*8] = {};
std::vector<unsigned int> Sphere::mIndices = {};
