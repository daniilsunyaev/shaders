// This object handles window, gl context initializing,
// extensions loading, errors logging routine

#pragma once

#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>

class App {
  public:
    App(const char* tWindowName = "SDL OpenGL Base",
        const int tWindowWidth = 800,
        const int tWindowHeight = 600,
        const int tGlMajor = 3,
        const int tGlMinor = 3);
    virtual ~App();

    SDL_Window* mWindow;
  protected:
    const char* mWindowName;
    int mWindowWidth;
    int mWindowHeight;
    bool mWindowInitialized = false;
  private:
    int initializeSdl();
    int createWindow();
    int initializeGlew();
    int createGlContext();

    int mGlMajor;
    int mGlMinor;
    SDL_GLContext mGlContext;
};
