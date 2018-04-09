#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "cube.hpp"
#include "shader.hpp"
#include <iostream>

Cube::Cube(glm::mat4 tModel, glm::vec3 tColor) : mModel(tModel), mColor(tColor) {
  if(mVAO == 0) {
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mVerts), mVerts, GL_STATIC_DRAW);
    glBindVertexArray(mVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  glGenTextures(1, &mDiffuseTexture);
  glBindTexture(GL_TEXTURE_2D, mDiffuseTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1, 1, 0, GL_RGB, GL_FLOAT, &tColor);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
}

Cube::~Cube() {
  if(mVAO > 0) {
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    mVAO = 0;
    mVBO = 0;
  }
}

void Cube::render(Shader* tShader) {
  if(tShader!=nullptr) {
    tShader->use();
    tShader->setMat4("model", mModel);

    if(mColor.r >= 0) {
      tShader->setInt("diffuse", 0);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mDiffuseTexture);
    }
  }

  glBindVertexArray(mVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

GLuint Cube::mVAO = 0;
GLuint Cube::mVBO = 0;
const float Cube::mVerts[36*8] = {
  // positions         // normals          // texture Coords
  -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
   1.0f,  1.0f, -1.0f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
   1.0f, -1.0f, -1.0f, 0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
   1.0f,  1.0f, -1.0f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
  -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
  -1.0f,  1.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
  // front face
  -1.0f, -1.0f, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
   1.0f, -1.0f, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
   1.0f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
   1.0f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
  -1.0f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
  -1.0f, -1.0f, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
  // left face
  -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
  -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
  -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
  -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
  -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
  -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
  // right face
  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
  1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
  1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
  1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
  1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
  // bottom face
  -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
   1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
   1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
   1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
  -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
  -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
  // top face
  -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
   1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
   1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
   1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
  -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
  -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
};
