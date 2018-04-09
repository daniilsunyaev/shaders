#pragma once

#include <iostream>
//#include <GL/glew.h>
#include <SDL2/SDL.h>

// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>

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

    double mDeltaTime = 0.0;
    double mLastFrame = 0.0;
    double mCurrentFrame = 0.0;

    Camera mCamera;
  private:
    void processInput();
    void mouseCallback(SDL_Event &e);
    void scrollCallback(SDL_Event &e);

    bool mFirstMouse = true;
    bool mMainLoop = true;
};
