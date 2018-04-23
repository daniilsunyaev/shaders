#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "pbr.hpp"
#include "plane.hpp"
#include "cube.hpp"
#include "textures.hpp"

PBRApp::PBRApp(
    const char* tWindowName,
    const int tWindowWidth,
    const int tWindowHeight
    ) : CameraApp(tWindowName, tWindowWidth, tWindowHeight) {}

void PBRApp::setUpScene() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  glDepthFunc(GL_LEQUAL);

  unsigned int captureFBO, captureRBO;
  glGenFramebuffers(1, &captureFBO);
  glGenRenderbuffers(1, &captureRBO);

  //glGenFramebuffers(1, &mCaptureFBO);
  //glGenRenderbuffers(1, &mCaptureRBO);
  std::cout << "building BRDF lookup texture" << std::endl;

  glGenTextures(1, &mBRDFLUTTexture);
  glBindTexture(GL_TEXTURE_2D, mBRDFLUTTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBRDFLUTTexture, 0);

  glViewport(0, 0, 512, 512);
  mBRDFShader.use();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mPostProcessingPlane.render();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PBRApp::~PBRApp() {
  for(auto& figureP : mScene) {
    delete figureP;
  }
}

void PBRApp::mainLoopBody() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);  
  glClearColor(0.05f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mProjection = glm::perspective(glm::radians(mCamera.Zoom),
      (float)mWindowWidth / (float)mWindowHeight, 0.1f, 100.0f);
  mView = mCamera.GetViewMatrix();

  glViewport(0, 0, mWindowWidth, mWindowHeight);

  mPostShader.use();
  //mPostShader.setMat4("projection", mProjection);
  //mPostShader.setMat4("view", mView);
  mPostShader.setInt("map", 5);
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, mBRDFLUTTexture);
  mPostProcessingPlane.render();
}
