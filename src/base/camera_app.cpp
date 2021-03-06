#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "app.hpp"
#include "camera_app.hpp"
#include "camera.hpp"


CameraApp::CameraApp(const char* tWindowName,
    const int tWindowWidth, const int tWindowHeight,
    const int tGlMajor, const int tGlMinor) :
  App(tWindowName, tWindowWidth, tWindowHeight, tGlMajor, tGlMinor),
  mCamera(Camera(glm::vec3(0.0f, 0.0f, 1.0f))) {}

CameraApp::~CameraApp() {}

void CameraApp::run() {
  if(windowIsInitialized()) {
    setUpScene();

    while(mMainLoop) {
      mLastFrameSeconds = mCurrentFrameSeconds;
      mCurrentFrameSeconds = (float)SDL_GetTicks() * 0.001f;
      mDeltaTime = mCurrentFrameSeconds - mLastFrameSeconds;

      processInput();
      mainLoopBody();

      swapWindow();
    }
  }
}

void CameraApp::setUpScene() {
  std::cout << "setting scene up..." << std::endl;
}

void CameraApp::mainLoopBody() {
  // if last seconds digit changed during current frame, print coords
  if(floor(mLastFrameSeconds) - floor(mCurrentFrameSeconds)!=0) {
    std::cout << "Camera pos: " << mCamera.getPosition().x
      << " " << mCamera.getPosition().y
      << " " << mCamera.getPosition().z << std::endl;
    std::cout << "pitch: " << mCamera.getPitch()
      << ", yaw: " << mCamera.getYaw() << std::endl;
  }
}

void CameraApp::processInput() {
  SDL_Event e;
  while(SDL_PollEvent(&e) != 0) {
    if(e.type == SDL_QUIT) {
      mMainLoop = false;
    } else if(e.type == SDL_MOUSEMOTION) {
      mouseCallback(e);

    } else if(e.type == SDL_MOUSEBUTTONDOWN) {
      mMouseLook = !mMouseLook;
      SDL_SetRelativeMouseMode((SDL_bool)mMouseLook);
    } else if(e.type == SDL_MOUSEWHEEL) {
      scrollCallback(e);
    } else if(e.type == SDL_KEYDOWN) {
      switch(e.key.keysym.sym) {
        case SDLK_w:
          mCamera.processKeyboard(FORWARD, mDeltaTime);
          break;
        case SDLK_s:
          mCamera.processKeyboard(BACKWARD, mDeltaTime);
          break;
        case SDLK_a:
          mCamera.processKeyboard(LEFT, mDeltaTime);
          break;
        case SDLK_d:
          mCamera.processKeyboard(RIGHT, mDeltaTime);
          break;
        default:
          break;
      }
    }
  }
}

void CameraApp::mouseCallback(SDL_Event &e) {
  if(mMouseLook) {
    mCamera.processMouseMovement((float)e.motion.xrel, (float)-e.motion.yrel);
  }
}

void CameraApp::scrollCallback(SDL_Event &e) {
  mCamera.processMouseScroll((float)e.wheel.y);
}

const Camera& CameraApp::getCamera() const {
  return mCamera;
}

const float CameraApp::getCurrentFrameSeconds() const {
  return mCurrentFrameSeconds;
}
