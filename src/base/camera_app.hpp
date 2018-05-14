#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include "app.hpp"
#include "camera.hpp"

class CameraApp : public App {
  public:
    CameraApp(const char* tWindowName = "SDL OpenGL Camera Base",
        const int tWindowWidth = 800,
        const int tWindowHeight = 600,
        const int tGlMajor = 3,
        const int tGlMinor = 3);
    void run();
    virtual ~CameraApp();
  protected:
    virtual void setUpScene();
    virtual void mainLoopBody();

    float mDeltaTime = 0.0;
    float mLastFrame = 0.0;
    float mCurrentFrame = 0.0;

    Camera mCamera;
  private:
    void processInput();
    void mouseCallback(SDL_Event &e);
    void scrollCallback(SDL_Event &e);

    bool mMouseLook = false;
    bool mMainLoop = true;
};
