#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include "ssao.hpp"
#include "plane.hpp"
#include "cube.hpp"

SSAOApp::SSAOApp(
    const char* tWindowName,
    const int tWindowWidth,
    const int tWindowHeight
    ) : CameraApp(tWindowName, tWindowWidth, tWindowHeight) {}

void SSAOApp::setUpScene() {
  glEnable(GL_DEPTH_TEST);
  buildGeometry();
  initGbuffer();
  initRawSSAO();
  initBluredSSAO();
  initRandomizedLookupHemisphere();
}

SSAOApp::~SSAOApp() {
  for(auto& figureP : mScene) {
    delete figureP;
  }
}

void SSAOApp::initGbuffer() {
  std::cout << "initializing Gbuffer..." << std::endl;

  glGenFramebuffers(1, &mGbufferFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, mGbufferFBO);
  glGenTextures(3, mGtextures);

  for(int i=0; i<3; i++) {
    glBindTexture(GL_TEXTURE_2D, mGtextures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWindowWidth, mWindowHeight,
        0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i,
        GL_TEXTURE_2D, mGtextures[i], 0);
  }
  unsigned int attachemts[3] = {
    GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2
  };
  glDrawBuffers(3, attachemts);
  unsigned int rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWindowWidth, mWindowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
     GL_RENDERBUFFER, rboDepth);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOApp::initRawSSAO() {
  std::cout << "initializing SSAO framebuffers..." << std::endl;

  glGenFramebuffers(1, &mRawSSAOFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, mRawSSAOFBO);
  glGenTextures(1, &mRawSSAOtexture);
  glBindTexture(GL_TEXTURE_2D, mRawSSAOtexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mWindowWidth, mWindowHeight,
      0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_2D, mRawSSAOtexture, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOApp::initBluredSSAO() {
  glGenFramebuffers(1, &mBluredSSAOFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, mBluredSSAOFBO);
  glGenTextures(1, &mBluredSSAOtexture);
  glBindTexture(GL_TEXTURE_2D, mBluredSSAOtexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mWindowWidth, mWindowHeight,
      0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_2D, mBluredSSAOtexture, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOApp::initRandomizedLookupHemisphere() {
  std::cout << "initializing SSAO lookup samples..." << std::endl;

  std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
  std::default_random_engine generator;
  mSSAOShader.use();
  for(int i=0; i<AO_SAMPLES; i++) {
    float phi = glm::radians(randomFloats(generator)*360);
    float theta = glm::radians(1+randomFloats(generator)*89);
    float ro = randomFloats(generator)*0.95f+0.05f;
    glm::vec3 sample(
        ro*sin(theta)*cos(phi),
        ro*sin(theta)*sin(phi),
        ro*cos(theta)
        );
    mSSAOShader.setVec3("ssaoKernel[" + std::to_string(i) + "]", sample);
  }

  std::vector<glm::vec3> rotationVectors;
  for(int i=0; i<ROTATION_TEXTURE_SIZE*ROTATION_TEXTURE_SIZE; i++) {
    float phi = glm::radians(randomFloats(generator)*360);
    float theta = glm::radians(randomFloats(generator)*180 - 90.0f);
    glm::vec3 noise(
        sin(theta)*cos(phi),
        sin(theta)*sin(phi),
        cos(theta)
        );
    rotationVectors.push_back(noise);
  }
  glGenTextures(1, &mSSAORotationVectorsTexture);
  glBindTexture(GL_TEXTURE_2D, mSSAORotationVectorsTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ROTATION_TEXTURE_SIZE,
      ROTATION_TEXTURE_SIZE, 0, GL_RGB, GL_FLOAT, &rotationVectors[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

  mSSAOShader.setInt("rotationVectors", 0);
}

void SSAOApp::mainLoopBody() {
  glClearColor(0.05f, 0.1f, 0.1f, 1.0f);
  mProjection = glm::perspective(glm::radians(mCamera.Zoom),
      (float)mWindowWidth / (float)mWindowHeight, 0.1f, 100.0f);
  mView = mCamera.GetViewMatrix();

  mLightDirection.x = 4.0f*cos(0.5f*mCurrentFrame);
  mLightDirection.z = 2.0f*sin(0.5f*mCurrentFrame);

  glViewport(0, 0, mWindowWidth, mWindowHeight);

  renderGbuffer();
  renderRawSSAO();
  blurRawSSAO();
  performAOShading();
}

void SSAOApp::renderGbuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, mGbufferFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mGbufferShader.use();
  mGbufferShader.setMat4("view", mView);
  mGbufferShader.setMat4("projection", mProjection);
  for(auto& figureP : mScene) {
    figureP->render(&mGbufferShader);
  }
}

void SSAOApp::renderRawSSAO() {
  glBindFramebuffer(GL_FRAMEBUFFER, mRawSSAOFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mSSAOShader.use();
  mSSAOShader.setMat4("projection", mProjection);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mSSAORotationVectorsTexture);
  mSSAOShader.setInt("gPosition", 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, mGtextures[0]);
  mSSAOShader.setInt("gNormal", 2);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, mGtextures[1]);
  mSSAOShader.setInt("gAlbedo", 3);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, mGtextures[2]);
  mPostProcessingPlane.render();
}


void SSAOApp::blurRawSSAO() {
  glBindFramebuffer(GL_FRAMEBUFFER, mBluredSSAOFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mBlurShader.use();
  mBlurShader.setInt("map", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mRawSSAOtexture);
  mPostProcessingPlane.render();
}

void SSAOApp::performAOShading() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mIlluminationShader.use();
  mIlluminationShader.setVec3("viewLightDirection", glm::mat3(mView)*mLightDirection);
  mIlluminationShader.setInt("map", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mBluredSSAOtexture);
  mIlluminationShader.setInt("gNormal", 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, mGtextures[1]);
  mIlluminationShader.setInt("gAlbedo", 2);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, mGtextures[2]);
  mPostProcessingPlane.render();
}

void SSAOApp::buildGeometry() {
  // floor
  auto model = glm::mat4();
  model = glm::scale(model, glm::vec3(2.0f, 0.1f, 3.0f));
  model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));
  auto cube = new Cube(model, {1.0f, 0.5f, 0.05f});
  mScene.push_back(cube);

  // ceiling
  model = glm::mat4();
  model = glm::scale(model, glm::vec3(2.0f, 0.1f, 3.0f));
  model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
  cube = new Cube(model, {0.6f, 0.6f, 0.6f});
  mScene.push_back(cube);

  // left wall
  model = glm::mat4();
  model = glm::scale(model, glm::vec3(0.1f, 1.1f, 3.0f));
  model = glm::translate(model, glm::vec3(-21.0f, 0.0f, 0.0f));
  cube = new Cube(model, {0.5f, 0.5f, 0.5f});
  mScene.push_back(cube);

  // front wall
  model = glm::mat4();
  model = glm::scale(model, glm::vec3(2.0f, 1.1f, 0.1f));
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, -31.0f));
  cube = new Cube(model, {0.5f, 0.5f, 0.5f});
  mScene.push_back(cube);

  // back wall
  model = glm::mat4();
  model = glm::scale(model, glm::vec3(2.0f, 1.1f, 0.1f));
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, 31.0f));
  cube = new Cube(model, {0.5f, 0.5f, 0.5f});
  mScene.push_back(cube);

  // figure
  model = glm::mat4();
  model = glm::translate(model, glm::vec3(-0.5f, -0.4f, -2.0f));
  model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.3f));
  model = glm::rotate(model, glm::radians(-25.0f), glm::vec3(0.0f, 0.5f, 0.0f));
  cube = new Cube(model, {0.0f, 0.4f, 1.0f});
  mScene.push_back(cube);

  // another figure
  model = glm::mat4();
  model = glm::translate(model, glm::vec3(-1.5f, -0.4f, -2.0f));
  model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.3f));
  model = glm::rotate(model, glm::radians(35.0f), glm::vec3(0.0f, 0.5f, 0.0f));
  cube = new Cube(model, {1.0f, 0.4f, 0.1f});
  mScene.push_back(cube);
}
