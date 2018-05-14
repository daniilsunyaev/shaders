#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include "reflective_shadow_maps.hpp"
#include "plane.hpp"
#include "cube.hpp"

ReflectiveShadowMapsApp::ReflectiveShadowMapsApp(
    const char* tWindowName,
    const int tWindowWidth,
    const int tWindowHeight
    ) : CameraApp(tWindowName, tWindowWidth, tWindowHeight) {}

void ReflectiveShadowMapsApp::setUpScene() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);

  buildGeometry();
  initRSM(2048, 2048);
  initRSMOffsets(300);
  initGIMap(32, 32);
  initHDR();
}

ReflectiveShadowMapsApp::~ReflectiveShadowMapsApp() {
  for(auto& figureP : mScene) {
    delete figureP;
  }
}

void ReflectiveShadowMapsApp::initRSM(int tWidth, int tHeight) {
  std::cout << "initializing shadowmap framebuffer..." << std::endl;

  mShadowmapWidth = tWidth;
  mShadowmapHeight = tHeight;
  glGenFramebuffers(1, &mRSMFBO);

  glGenTextures(1, &mLightDepthMapTexture);
  glBindTexture(GL_TEXTURE_2D, mLightDepthMapTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, tWidth, tHeight,
      0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glBindFramebuffer(GL_FRAMEBUFFER, mRSMFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
      GL_TEXTURE_2D, mLightDepthMapTexture, 0);

  glGenTextures(3, mRsmTextures);
  for(int i=0; i<3; i++) {
    glBindTexture(GL_TEXTURE_2D, mRsmTextures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, tWidth, tHeight,
        0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i,
        GL_TEXTURE_2D, mRsmTextures[i], 0);
  }
  unsigned int attachemts[3] = {
    GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2
  };
  glDrawBuffers(3, attachemts);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ReflectiveShadowMapsApp::initRSMOffsets(int tSamples) {
  std::cout << "generating RSM lookup samples..." << std::endl;

  std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
  std::default_random_engine generator;
  mRSMOffsets.reserve(tSamples);

  for(int i=0; i<tSamples; i++) {
    float phi = glm::radians(randomFloats(generator)*360);
    float ro = randomFloats(generator);
    glm::vec2 sample(
        MAX_ILLUMINATION_RADIUS*ro*ro*cos(phi),
        MAX_ILLUMINATION_RADIUS*ro*ro*sin(phi)
        );
    mRSMOffsets.push_back(sample);
  }

  mFirstBounceIlluminationShader.use();
  for(int i=0; i<tSamples; i++) {
    mFirstBounceIlluminationShader.setVec2("offsetSamples[" + std::to_string(i) + "]", 
        mRSMOffsets[i]);
  }
  float illuminationArea = 
    MAX_ILLUMINATION_RADIUS * MAX_ILLUMINATION_RADIUS * 3.1415926f
    * (float)mShadowmapHeight * (float)mShadowmapWidth;
  mFirstBounceIlluminationShader.setFloat("illuminationArea", illuminationArea);

  mRenderShader.use();
  for(int i=0; i<tSamples; i++) {
    mRenderShader.setVec2("offsetSamples[" + std::to_string(i) + "]", 
        mRSMOffsets[i]);
  }
  mRenderShader.setFloat("illuminationArea", illuminationArea);
}

void ReflectiveShadowMapsApp::initGIMap(int tWidth, int tHeight) {
  std::cout << "initializing global illumination framebuffer..." << std::endl;

  mFirstBounceTextureWidth = tWidth;
  mFirstBounceTextureHeight = tHeight;
  glGenFramebuffers(1, &mIlluminationFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, mIlluminationFBO);

  unsigned int rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, tWidth, tHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
     GL_RENDERBUFFER, rboDepth);

  glGenTextures(1, &mIlluminationMapTexture);
  glGenTextures(1, &mLowResPositions);
  glGenTextures(1, &mLowResNormals);

  glBindTexture(GL_TEXTURE_2D, mIlluminationMapTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, tWidth, tHeight,
      0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_2D, mIlluminationMapTexture, 0);

  glBindTexture(GL_TEXTURE_2D, mLowResPositions);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, tWidth, tHeight,
      0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
      GL_TEXTURE_2D, mLowResPositions, 0);
  glBindTexture(GL_TEXTURE_2D, mLowResNormals);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, tWidth, tHeight,
      0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
      GL_TEXTURE_2D, mLowResNormals, 0);
  unsigned int attachemts[3] = {
    GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2
  };
  glDrawBuffers(3, attachemts);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ReflectiveShadowMapsApp::initHDR() {
  std::cout << "initializing gamma cor FBO" << std::endl;
  glGenFramebuffers(1, &mHdrFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, mHdrFBO);

  glGenTextures(1, &mScreenTexture);
  glBindTexture(GL_TEXTURE_2D, mScreenTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWindowWidth, mWindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mScreenTexture, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWindowWidth, mWindowHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);  
}

void ReflectiveShadowMapsApp::mainLoopBody() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mLightPosition.z = 1.0f + 3.0f*sin(0.3f*mCurrentFrame);

  glm::mat4 lightProjection, lightView;
  glm::mat4 lightSpaceMatrix;
  float near_plane = 1.0f, far_plane = 20.0f;
  lightProjection = glm::ortho(-8.0f, 8.0f, -8.0f, 8.0f,
      near_plane, far_plane);  
  lightView = glm::lookAt(mLightPosition, 
      glm::vec3(0.0f, 0.0f, 0.0f), 
      glm::vec3(0.0f, 1.0f, 0.0f));  
  mLightSpaceMatrix = lightProjection * lightView;

  mProjection = glm::perspective(glm::radians(mCamera.mZoom),
      (float)mWindowWidth / (float)mWindowHeight, 0.1f, 100.0f);
  mView = mCamera.GetViewMatrix();

  renderShadowMaps();
  renderGIMaps();
  renderScreenScene();
  performPostProcessing();
}

void ReflectiveShadowMapsApp::renderShadowMaps() {
  mRSMShader.use();
  mRSMShader.setMat4("lightSpaceMatrix", mLightSpaceMatrix);
  mRSMShader.setVec3("L", mLightPosition);
  glViewport(0, 0, mShadowmapWidth, mShadowmapHeight);
  glBindFramebuffer(GL_FRAMEBUFFER, mRSMFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for(auto& figureP : mScene) {
    figureP->render(&mRSMShader);
  }
}

void ReflectiveShadowMapsApp::renderGIMaps() {
  glBindFramebuffer(GL_FRAMEBUFFER, mIlluminationFBO);
  glViewport(0, 0, mFirstBounceTextureWidth, mFirstBounceTextureHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mFirstBounceIlluminationShader.use();
  mFirstBounceIlluminationShader.setMat4("view", mView);
  mFirstBounceIlluminationShader.setMat4("projection", mProjection);
  mFirstBounceIlluminationShader.setMat4("lightSpaceMatrix", mLightSpaceMatrix);

  mFirstBounceIlluminationShader.setInt("rPosition", 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, mRsmTextures[0]);

  mFirstBounceIlluminationShader.setInt("rNormal", 2);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, mRsmTextures[1]);

  mFirstBounceIlluminationShader.setInt("rFlux", 3);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, mRsmTextures[2]);

  for(auto& figureP : mScene) {
    figureP->render(&mFirstBounceIlluminationShader);
  }
}

void ReflectiveShadowMapsApp::renderScreenScene() {
  glBindFramebuffer(GL_FRAMEBUFFER, mHdrFBO);
  glViewport(0, 0, mWindowWidth, mWindowHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  mRenderShader.use();
  mRenderShader.setVec3("lightDir", mLightPosition);

  mRenderShader.setInt("windowWidth", mWindowWidth);
  mRenderShader.setInt("windowHeight", mWindowHeight);
  mRenderShader.setMat4("view", mView);
  mRenderShader.setMat4("projection", mProjection);
  mRenderShader.setMat4("lightSpaceMatrix", mLightSpaceMatrix);
  mRenderShader.setInt("shadowMap", 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, mLightDepthMapTexture);
  mRenderShader.setInt("illuminationMap", 2);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, mIlluminationMapTexture);
  mRenderShader.setInt("lowResPosition", 3);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, mLowResPositions);

  mRenderShader.setInt("lowResNormal", 4);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, mLowResNormals);

  for(auto& figureP : mScene) {
    figureP->render(&mRenderShader);
  }
}

void ReflectiveShadowMapsApp::performPostProcessing() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mPostProcessingShader.use();
  mPostProcessingShader.setInt("map", 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, mScreenTexture);
  mPostProcessingPlane.render();
}

void ReflectiveShadowMapsApp::buildGeometry() {
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
}
