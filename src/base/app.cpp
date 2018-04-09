// This file handles window / gl context initializing,
// extensions loading, errors logging routine

#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "app.hpp"

App::App(const char* tWindowName,
    const int tWindowWidth, const int tWindowHeight,
    const int tGlMajor, const int tGlMinor) :
  mWindowName(tWindowName),
  mWindowWidth(tWindowWidth), mWindowHeight(tWindowHeight),
  mGlMajor(tGlMajor), mGlMinor(tGlMinor) {
    int sdlInitError = initializeSdl();
    int windowError = createWindow();
    int glContextError = createGlContext();

    SDL_GL_SetSwapInterval(1);

    int glewInitError = initializeGlew();

  if(sdlInitError ==0
      && windowError == 0
      && glContextError == 0
      && glewInitError == 0) {
    mWindowInitialized = true;
  }
}

App::~App() {
  SDL_GL_DeleteContext(mGlContext);
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
}

int App::initializeSdl() {
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
    SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
    return 1;
  }
  return 0;
}

int App::initializeGlew() {
  glewExperimental = GL_TRUE;
  GLenum error = glewInit();
  if(error != GLEW_OK) {
    SDL_Log("Unable to initialize Glew: %s\n", glewGetErrorString(error));
    return 1;
  }
  return 0;
}

int App::createGlContext() {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, mGlMajor);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, mGlMinor);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  mGlContext = SDL_GL_CreateContext(mWindow);

  if(mGlContext == NULL) {
    SDL_Log("Unable to initialize GL context: %s\n", SDL_GetError());
    return 1;
  }
  return 0;
}

int App::createWindow() {
  mWindow = SDL_CreateWindow(
      mWindowName,
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      mWindowWidth,
      mWindowHeight,
      SDL_WINDOW_OPENGL
      );

  if(mWindow == NULL) {
    SDL_Log("Unable to create window: %s\n", SDL_GetError());
    return 1;
  }

  return 0;
}
