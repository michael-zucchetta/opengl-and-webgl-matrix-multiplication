#ifndef __APPLE__
#include <GLES3/gl3.h>
#else
#include <OpenGLES/ES3/gl.h>
#endif

#include <iostream>

#include "openglprogram.hpp"

// https://www.ibiblio.org/e-notes/webgl/gpu/mul/mul16_4.htm
const GLchar *fragmentShaderSource = R"glsl(#version 300 es
  precision highp float;
  uniform highp sampler2D sampA;
  uniform highp sampler2D sampB;
  out vec4 fragColor;
  uniform int K4;   // K/4
  void main(void) {
    vec4 acc = vec4(0.f);
    int id = 4*int(gl_FragCoord.x);
    for(int k=0; k< K4; k++){
      vec4 a = texelFetch(sampA, ivec2(k, gl_FragCoord.y), 0);
      acc.x += dot(a, texelFetch(sampB, ivec2(k, id    ), 0));
      acc.y += dot(a, texelFetch(sampB, ivec2(k, id + 1), 0));
      acc.z += dot(a, texelFetch(sampB, ivec2(k, id + 2), 0));
      acc.w += dot(a, texelFetch(sampB, ivec2(k, id + 3), 0));
    }
    fragColor = acc;
  }
)glsl";

const GLchar *vertexShaderSource = R"glsl(#version 300 es
layout (location = 0) in vec2 aPos;
void main(void) {
  gl_Position = vec4(aPos, 0., 1.);
}
)glsl";



class MatrixMultiplication : public OpenGLProgram
{
  private:
    GLuint fbo;
    GLuint aPos;
    int M = 256, K = 128, N = 512;
    
    GLuint createTex(GLenum active, float *data, int i, int j) {
      std::cout<< "create tex " << active << " " <<glGetError() << std::endl;
      GLuint tex;
      glGenTextures(1, &tex);//glCreateTexture();
      glActiveTexture(active);
      glBindTexture(GL_TEXTURE_2D, tex);
      glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, i, j);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, i, j, GL_RGBA, GL_FLOAT, data);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      return tex;
    }
  public:
    MatrixMultiplication(const GLchar *vertexSource, const GLchar *fragmentSource, uint windowHeight, uint windowWidth)
      : OpenGLProgram(vertexSource, fragmentSource, windowHeight, windowHeight) {
        this->aPos = 0;
      }

    void draw() {

      std::cout<< "draw init " << glGetError() << std::endl;
      glUseProgram(this->program);
      GLuint bufferId = 0;
      glClear(GL_COLOR_BUFFER_BIT);
      std::cout<< "GL clear " << glGetError() << std::endl;
      glEnableVertexAttribArray(this->aPos);
      std::cout<< "Enable vertex " << glGetError() << std::endl;
      glGenBuffers(1, &bufferId);
      glBindBuffer(GL_ARRAY_BUFFER, bufferId);
      std::cout<< "bind buffer " << glGetError() << std::endl;
      float buffer [] = {-1, -1, 1, -1, -1, 1, 1, 1 };
      glBufferData(GL_ARRAY_BUFFER,
            sizeof(buffer),
            buffer, GL_STATIC_DRAW);
      std::cout<< "Buffer data " << glGetError() << std::endl;
      glVertexAttribPointer(this->aPos, 2, GL_FLOAT, GL_FALSE, 0, 0); 
      GLuint sampLoc1 = glGetUniformLocation(this->program, "sampA");
      glUniform1i(sampLoc1, 0);
      GLuint sampLoc2 = glGetUniformLocation(this->program, "sampB");
      glUniform1i(sampLoc2, 1);
      glUniform1i(glGetUniformLocation(this->program, "K4"), K/4);
      //glClear();
      float *_texture0Data = new float[K * M];
      float *_texture1Data = new float[K * N];
      float *_texture2Data = new float[M * N];
      for (int i = 0; i < M * K; ++i) {
        _texture0Data[i] = 2;
      }

      for (int i = 0; i < K * N; ++i) {
        _texture1Data[i] = 3;
      }
      
      for (int i = 0; i < M * N; ++i) {
        _texture2Data[i] = 0;
      }
      
      GLuint tex0 = this->createTex(GL_TEXTURE0, _texture0Data, K/4, M);
      GLuint tex1 = this->createTex(GL_TEXTURE1, _texture1Data, K/4,N);
      GLuint tex2 = this->createTex(GL_TEXTURE2, _texture2Data, N/4,M);


      GLuint FBO = 0;// glCreateFramebuffer();
      glGenFramebuffers(1, &FBO);
      glBindFramebuffer(GL_FRAMEBUFFER, FBO);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex2, 0);
      //Test if everything failed    
      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if(status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout<<"failed to make complete framebuffer object" << status<<std::endl;
      }
      glViewport(0,0, N/4,M);
      std::cout<< "Check values " << glGetError() << std::endl;
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      std::cout<< glGetError() << std::endl;
      glReadPixels(0,0, N/4, M, GL_RGBA, GL_FLOAT, _texture2Data);
      std::cout<< glGetError() << std::endl;
      for (int i = 0; i < M * N; ++i) {
        std::cout<<i<< " : " << _texture2Data[i] << std::endl;
      }
      std::cout<<"TEX " << tex0 << " " << tex1 << " " << tex2 << std::endl;
    }

    void update() {

    }
};


int main() {
  uint height = 1, width = 1;
  MatrixMultiplication program(vertexShaderSource, fragmentShaderSource, height, width);
  program.runProgram();

}
