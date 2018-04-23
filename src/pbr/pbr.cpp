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

  glGenFramebuffers(1, &mCaptureFBO);
  glGenRenderbuffers(1, &mCaptureRBO);

  buildEnvCubemap();
  buildIrradianceCubemap();
  buildPrefilteredEnvMap();
  buildBRDFLUT();

  glDeleteFramebuffers(1, &mCaptureFBO);
  glDeleteRenderbuffers(1, &mCaptureRBO);


  //buildGeometry();

  mPBRShader.use();
  //mAlbedo = loadTexture("./materials/streaked_metal/albedo.png");
  //mAlbedo = loadTexture("./materials/rock/albedo.png");
  //mAlbedo = loadTexture("./materials/rusted_iron/albedo.png");
  //mAlbedo = loadTexture("./materials/marble/albedo.png");
  mAlbedo = loadTexture("./materials/plastic/albedo.png");
  mPBRShader.setInt("albedoTex", 3);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, mAlbedo);

  //mMetallic = loadTexture("./materials/streaked_metal/metallic.png");
  //mMetallic = loadTexture("./materials/rock/metallic.png");
  //mMetallic = loadTexture("./materials/rusted_iron/metallic.png");
  //mMetallic = loadTexture("./materials/marble/metallic.png");
  mMetallic = loadTexture("./materials/plastic/metallic.png");
  mPBRShader.setInt("metallicTex", 4);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, mMetallic);

  //mRoughness = loadTexture("./materials/streaked_metal/roughness.png");
  //mRoughness = loadTexture("./materials/rock/roughness.png");
  //mRoughness = loadTexture("./materials/rusted_iron/roughness.png");
  //mRoughness = loadTexture("./materials/marble/roughness.png");
  mRoughness = loadTexture("./materials/plastic/roughness.png");
  mPBRShader.setInt("roughnessTex", 5);
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, mRoughness);

  //mAo = loadTexture("./materials/streaked_metal/ao.png");
  //mAo = loadTexture("./materials/rock/ao.png");
  //mAo = loadTexture("./materials/rusted_iron/ao.png");
  //mAo = loadTexture("./materials/marble/ao.png");
  mAo = loadTexture("./materials/plastic/ao.png");
  mPBRShader.setInt("aoTex", 6);
  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, mAo);

  //mNormal = loadTexture("./materials/streaked_metal/normal.png");
  //mNormal = loadTexture("./materials/rock/normal.png");
  //mNormal = loadTexture("./materials/rusted_iron/normal.png");
  //mNormal = loadTexture("./materials/marble/normal.png");
  mNormal = loadTexture("./materials/plastic/normal.png");
  mPBRShader.setInt("normalTex", 7);
  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, mNormal);

}

PBRApp::~PBRApp() {
  for(auto& figureP : mScene) {
    delete figureP;
  }
}

void PBRApp::buildEnvCubemap() {
  std::cout << "initializing environment cubemap..." << std::endl;
  glGenTextures(1, &mEnvCubemapTexture);
  buildCubemapFromEquirectmap(mEnvCubemapTexture, 1024, ENV_MAP_PATH);
}

void PBRApp::buildIrradianceCubemap() {
  std::cout << "initializing irradiance cubemap..." << std::endl;
  glGenTextures(1, &mIrradianceCubemapTexture);
  buildCubemapFromEquirectmap(mIrradianceCubemapTexture, 32, IRRADIANCE_MAP_PATH);
}

void PBRApp::buildCubemapFromEquirectmap(unsigned int tCubemapTextureId, int tSize, char const* tEquirectmapPath) {
  glBindFramebuffer(GL_FRAMEBUFFER, mCaptureFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, mCaptureRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, tSize, tSize);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mCaptureRBO); 
  glBindTexture(GL_TEXTURE_CUBE_MAP, tCubemapTextureId);
  for (unsigned int i=0; i<6; i++) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 
        tSize, tSize, 0, GL_RGB, GL_FLOAT, nullptr);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  mCubemapSetupShader.use();
  mCubemapSetupShader.setInt("equirect_map", 0);
  mCubemapSetupShader.setMat4("projection", mCaptureProjection);
  glActiveTexture(GL_TEXTURE0);
  auto equirectTexture = loadTextureF(tEquirectmapPath);
  glBindTexture(GL_TEXTURE_2D, equirectTexture);

  glViewport(0, 0, tSize, tSize);
  glBindFramebuffer(GL_FRAMEBUFFER, mCaptureFBO);
  for (unsigned int i = 0; i < 6; ++i) {
    mCubemapSetupShader.setMat4("view", mCaptureViews[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, tCubemapTextureId, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mSkyboxCube.render();
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);  
  glBindTexture(GL_TEXTURE_CUBE_MAP, tCubemapTextureId);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void PBRApp::buildPrefilteredEnvMap() {
  std::cout << "initializing prefiltered env map..." << std::endl;
  glGenTextures(1, &mPrefilteredEnvMapTexture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, mPrefilteredEnvMapTexture);
  for (unsigned int i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 
        128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  mPrefilterShader.use();
  mPrefilterShader.setInt("environmentMap", 0);
  mPrefilterShader.setMat4("projection", mCaptureProjection);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvCubemapTexture);

  std::cout << "prefiltering" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, mCaptureFBO);
  unsigned int maxMipLevels = 4;
  for (unsigned int mip = 0; mip < maxMipLevels; mip++) {
    std::cout << "mip " << mip << std::endl;
    unsigned int mipWidth  = 128 * std::pow(0.5, mip);
    unsigned int mipHeight = 128 * std::pow(0.5, mip);
    glBindRenderbuffer(GL_RENDERBUFFER, mCaptureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
    glViewport(0, 0, mipWidth, mipHeight);

    float roughness = (float)mip / (float)(maxMipLevels - 1);
    mPrefilterShader.setFloat("roughness", roughness);
    for (unsigned int i=0; i<6; i++) {
      mPrefilterShader.setMat4("view", mCaptureViews[i]);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mPrefilteredEnvMapTexture, mip);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      mSkyboxCube.render();
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);  
}

void PBRApp::buildBRDFLUT() {
  std::cout << "building BRDF lookup texture" << std::endl;
  glGenTextures(1, &mBRDFLUTTexture);
  glBindTexture(GL_TEXTURE_2D, mBRDFLUTTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

  glBindFramebuffer(GL_FRAMEBUFFER, mCaptureFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, mCaptureRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBRDFLUTTexture, 0);

  glViewport(0, 0, 512, 512);
  mBRDFShader.use();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mPostProcessingPlane.render();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PBRApp::mainLoopBody() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);  
  glClearColor(0.05f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mProjection = glm::perspective(glm::radians(mCamera.Zoom),
      (float)mWindowWidth / (float)mWindowHeight, 0.1f, 100.0f);
  mView = mCamera.GetViewMatrix();

  glViewport(0, 0, mWindowWidth, mWindowHeight);

  mPBRShader.use();
  mPBRShader.setMat4("view", mView);
  mPBRShader.setMat4("projection", mProjection);
  mPBRShader.setVec3("camPos", mCamera.Position);

  mPBRShader.setInt("irradianceMap", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, mIrradianceCubemapTexture);
  mPBRShader.setInt("prefilterMap", 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, mPrefilteredEnvMapTexture);
  mPBRShader.setInt("brdfLUT", 2);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, mBRDFLUTTexture);

  mPBRShader.setInt("albedoTex", 3);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, mAlbedo);

  mPBRShader.setInt("metallicTex", 4);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, mMetallic);

  mPBRShader.setInt("roughnessTex", 5);
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, mRoughness);

  mPBRShader.setInt("aoTex", 6);
  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, mAo);

  mPBRShader.setInt("normalTex", 7);
  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, mNormal);

  mBall.render(&mPBRShader);

  renderEnvCubemap();
}

void PBRApp::renderEnvCubemap() {
  mSkyboxShader.use();
  mSkyboxShader.setMat4("projection", mProjection);
  mSkyboxShader.setMat4("view", mView);
  mSkyboxShader.setInt("skybox", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvCubemapTexture);
  mSkyboxCube.render();
}

void PBRApp::renderIrradianceCubemap() {
  mSkyboxShader.use();
  mSkyboxShader.setMat4("projection", mProjection);
  mSkyboxShader.setMat4("view", mView);
  mSkyboxShader.setInt("skybox", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, mIrradianceCubemapTexture);
  mSkyboxCube.render();
}

void PBRApp::buildGeometry() {
  // streaked metal
  auto model = glm::mat4();
  model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
  auto sphere = new Sphere(model);
  mScene.push_back(sphere);

  // ceiling
  //model = glm::mat4();
  //model = glm::scale(model, glm::vec3(2.0f, 0.1f, 3.0f));
  //model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
  //cube = new Cube(model, {0.6f, 0.6f, 0.6f});
  //mScene.push_back(cube);

  //// left wall
  //model = glm::mat4();
  //model = glm::scale(model, glm::vec3(0.1f, 1.1f, 3.0f));
  //model = glm::translate(model, glm::vec3(-21.0f, 0.0f, 0.0f));
  //cube = new Cube(model, {0.5f, 0.5f, 0.5f});
  //mScene.push_back(cube);

  //// front wall
  //model = glm::mat4();
  //model = glm::scale(model, glm::vec3(2.0f, 1.1f, 0.1f));
  //model = glm::translate(model, glm::vec3(0.0f, 0.0f, -31.0f));
  //cube = new Cube(model, {0.5f, 0.5f, 0.5f});
  //mScene.push_back(cube);

  //// back wall
  //model = glm::mat4();
  //model = glm::scale(model, glm::vec3(2.0f, 1.1f, 0.1f));
  //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 31.0f));
  //cube = new Cube(model, {0.5f, 0.5f, 0.5f});
  //mScene.push_back(cube);

  //// figure
  //model = glm::mat4();
  //model = glm::translate(model, glm::vec3(-0.5f, -0.4f, -2.0f));
  //model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.3f));
  //model = glm::rotate(model, glm::radians(-25.0f), glm::vec3(0.0f, 0.5f, 0.0f));
  //cube = new Cube(model, {0.0f, 0.4f, 1.0f});
  //mScene.push_back(cube);

  //// another figure
  //model = glm::mat4();
  //model = glm::translate(model, glm::vec3(-1.5f, -0.4f, -2.0f));
  //model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.3f));
  //model = glm::rotate(model, glm::radians(35.0f), glm::vec3(0.0f, 0.5f, 0.0f));
  //cube = new Cube(model, {1.0f, 0.4f, 0.1f});
  //mScene.push_back(cube);
}
