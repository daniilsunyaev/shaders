#include <iostream>
#include <vector>
#include "camera_app.hpp"
#include "shader.hpp"
#include "geometry_object.hpp"
#include "plane.hpp"
#include "sphere.hpp"
#include "parallax_mapping.hpp"
#include "textures.hpp"

ParallaxApp::ParallaxApp(
    const char* tWindowName,
    const int tWindowWidth,
    const int tWindowHeight
    ) : CameraApp(tWindowName, tWindowWidth, tWindowHeight) {}

ParallaxApp::~ParallaxApp() {}

void ParallaxApp::setUpScene() {
  glEnable(GL_DEPTH_TEST);

  mDiffuseTexture = loadTexture(DIFFUSE_TEXTURE_PATH);
  mReliefTexture = loadTexture(RELIEF_TEXTURE_PATH);

  buildGeometry();
}

void ParallaxApp::buildGeometry() {
  buildPolygon();
  buildLight(glm::vec3(0.0f, 0.0f, -2.0f));
}

void ParallaxApp::buildPolygon() {
  auto model = glm::mat4();
  model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.3f));
  mParallaxMappedPolygon = Plane(model);
}

void ParallaxApp::buildLight(glm::vec3 tPosition) {
  auto model = glm::mat4();
  model = glm::translate(model, tPosition);
  model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
  mLight = Sphere(model);
}

void ParallaxApp::mainLoopBody() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glViewport(0, 0, getWindowWidth(), getWindowHeight());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mProjection = glm::perspective(glm::radians(getCamera().getZoom()),
      (float)getWindowWidth() / (float)getWindowHeight(), 0.1f, 100.0f);
  mView = getCamera().getViewMatrix();

  glm::vec3 lightPosition = glm::vec3(0.0f, 0.2f + 0.5f*sin(0.5f*getCurrentFrameSeconds()), sin(3 * getCurrentFrameSeconds()));
  buildLight(lightPosition);

  mParallaxMappingShader.use();
  mParallaxMappingShader.setMat4("view", mView);
  mParallaxMappingShader.setMat4("projection", mProjection);
  mParallaxMappingShader.setInt("diffuse", 0);
  mParallaxMappingShader.setVec3("cameraPosition", getCamera().getPosition());
  mParallaxMappingShader.setVec3("lightPosition", lightPosition);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mDiffuseTexture);
  mParallaxMappingShader.setInt("relief", 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, mReliefTexture);
  mParallaxMappedPolygon.render(&mParallaxMappingShader);

  mLightShader.use();
  mLightShader.setMat4("view", mView);
  mLightShader.setMat4("projection", mProjection);
  mLight.render(&mLightShader);
}
