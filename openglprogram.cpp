#include <iostream>
#include <vector>

#include "openglprogram.hpp"

#include <SDL2/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif


OpenGLProgram::OpenGLProgram(const GLchar *vertexSource, const GLchar *fragmentSource, uint windowHeight, uint windowWidth) {
  this->vertexSource = vertexSource;
  this->fragmentSource = fragmentSource;
  this->windowHeight = windowHeight;
  this->windowWidth = windowWidth;
  this->running = true;
  this->fullscreen  = false;
  std::cout<<"Initializing opengl program"<<std::endl;
  // important to initialize before shaders
  this->window = SDL_CreateWindow("OpenGL Test", 0, 0, this->windowWidth, this->windowHeight, SDL_WINDOW_OPENGL);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
#ifdef __EMSCRIPTEN__
  EmscriptenWebGLContextAttributes attrs;
  attrs.explicitSwapControl = 0;
  attrs.depth = 1;
  attrs.stencil = 1;
  attrs.antialias = 1;
  attrs.majorVersion = 3;
  attrs.minorVersion = 0;

  this->webglContext = emscripten_webgl_create_context(0, &attrs);
  emscripten_webgl_enable_extension(this->webglContext, "GL_EXT_color_buffer_float");
  emscripten_webgl_make_context_current(this->webglContext);
#endif
  // important to initialize after emscripten context above 
  SDL_GLContext Context = SDL_GL_CreateContext(this->window);
  std::cout<<this->vertexSource<<std::endl;
  std::cout<<this->fragmentSource;
  this->vertexShader = this->loadShader(GL_VERTEX_SHADER, this->vertexSource);
  this->fragmentShader = this->loadShader(GL_FRAGMENT_SHADER, this->fragmentSource);
}

void OpenGLProgram::initProgram() {
  // create the program object
  this->program = glCreateProgram();
  std::cout<<" Program is " << this->program << std::endl;
  glAttachShader(this->program, this->vertexShader);
  glAttachShader(this->program, this->fragmentShader);

  glLinkProgram(this->program);
  GLint linked;
  glGetProgramiv(this->program, GL_LINK_STATUS, &linked);
  if ( !linked ) {
    std::cout<<"Program not linked " << std::endl;
    GLint infoLen = 0;

    glGetProgramiv (this->program, GL_INFO_LOG_LENGTH, &infoLen );

    if ( infoLen > 1 ) {
      std::vector<GLchar> infoLog(infoLen);

      glGetProgramInfoLog(this->program, infoLen, &infoLen, &infoLog[0] );
      std::cout << "Error linking program:\n%s\n";
      for (auto i = infoLog.begin(); i != infoLog.end(); ++i) {
        std::cout<<*i;
      }

      std::cout << std::endl;

    }
    glDeleteProgram(this->program);
  } else {
    std::cout<<"program initialization is fine " << std::endl;
  }
}


GLuint OpenGLProgram::loadShader(const GLenum type, const GLchar *shaderSource) {
  GLuint shader;
  GLint compiled = 0;
  // Create the shader object
  shader = glCreateShader(type);
  std::cout << " SHADER IS " << shader << " " << type << std::endl;
  if (shader == 0) {
    return 0;
  }
  // Load the shader source
  glShaderSource(shader, 1, &shaderSource, NULL);
  // Compile the shader
  glCompileShader(shader);

  // Check the compile status
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  std::cout<<"Compiled status " << compiled << " " << " for shader " << shader << std::endl;
  if (!compiled){
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    std::cout<<"InfoLen " << infoLen << std::endl;
    if(infoLen > 1) {
      GLint maxLength = 0;
      std::vector<GLchar> infoLog(infoLen);

      glGetShaderInfoLog(shader, infoLen, &infoLen, &infoLog[0] );
      std::cout << "Error compiling shader of type " << type <<": \n\n";
      for (auto i = infoLog.begin(); i != infoLog.end(); ++i) {
        std::cout<<*i;
      }
      std::cout<<std::endl;
    }
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

void OpenGLProgram::loop() {
  uint32_t WindowFlags = SDL_WINDOW_OPENGL;
  //this->draw();
#ifndef __EMSCRIPTEN__
  while (running) {
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
      if (Event.type == SDL_KEYDOWN)
      {
        switch (Event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            running = false;
            break;
          case 'f':
            fullscreen = !fullscreen;
            if (fullscreen) {
              SDL_SetWindowFullscreen(this->window, WindowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
            else
            {
              SDL_SetWindowFullscreen(this->window, WindowFlags);
            }
            break;
          default:
            break;
        }
      }
      else if (Event.type == SDL_QUIT)
      {
        running = 0;
      }
    }
#endif
    //usleep(1000);
    // std::this_thread::sleep_for(10);
    this->update();
    this->draw();
    SDL_GL_SwapWindow(this->window);
#ifndef __EMSCRIPTEN__
    //glFlush();
    break; 
  }
#endif
}


void main_loop(void* arg)
{
    static_cast<OpenGLProgram*>(arg)->loop();
}

void OpenGLProgram::runProgram() {
  this->initProgram();
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(main_loop, this, -1, true);
#else
  this->loop();
#endif
}
