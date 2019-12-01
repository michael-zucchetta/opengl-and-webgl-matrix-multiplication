#ifndef OPENGLPROGRAM_HPP
#define OPENGLPROGRAM_HPP

#ifndef __APPLE__
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#else
#include <OpenGLES/ES3/gl.h>
#endif
#include <SDL2/SDL.h>

#include <functional>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif


class OpenGLProgram {

  private:
    GLuint loadShader(const GLenum type, const GLchar *shaderSource);
    void initProgram();
    SDL_Window *window;
    SDL_GLContext *context;
    bool running;
    bool fullscreen;
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webglContext;
#endif
  public:
    GLuint program;
    GLuint vertexShader;
    GLuint fragmentShader;
    const GLchar *vertexSource;
    const GLchar *fragmentSource;
    uint windowWidth;
    uint windowHeight;

    void loop();
    OpenGLProgram(const GLchar *vertexSource, const GLchar *fragmentSource, uint windowHeight, uint windowWidth);
    void runProgram();
    virtual void draw() = 0;
    virtual void update() = 0;
};

#endif
