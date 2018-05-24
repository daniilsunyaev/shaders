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
    void swapWindow();

    const int getWindowWidth() const;
    const int getWindowHeight() const;
    const bool windowIsInitialized() const;
  private:
    int initializeSdl();
    int createWindow();
    int initializeGlew();
    int createGlContext();

    SDL_Window* mWindow;
    const char* mWindowName;
    int mWindowWidth;
    int mWindowHeight;
    int mGlMajor;
    int mGlMinor;
    SDL_GLContext mGlContext;
    bool mWindowInitialized = false;
};
