/*
 * $Id: screen3d.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2005 Kenta Cho. Some rights reserved.
 */

#ifndef SCREEN3D_H
#define SCREEN3D_H

#include <cmath>
#include <stdexcept>
#include <string>

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "abagames/util/math.hpp"
#include "abagames/util/sdl/screen.hpp"
#include "abagames/util/sdl/sdlexception.hpp"
#include "abagames/util/vector.hpp"

/**
 * SDL screen handler (3D, OpenGL).
 */

static float _brightness = 1;

class Screen3D : public Screen, public SizableScreen {
private:
  float _farPlane = 1000;
  float _nearPlane = 0.1;
  int _width = 640;
  int _height = 480;
  bool _windowMode = true;
  uint32_t _videoFlags = 0;

protected:
  virtual void init() = 0;
  virtual void close() = 0;
  virtual void setIcon() {}

public:
  void initSDL() {
    // Initialize SDL.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      throw SDLInitFailedException("Unable to initialize SDL: " +
                                   std::string(SDL_GetError()));
    }
    setIcon();
    // Create an OpenGL screen.
    if (_windowMode) {
      _videoFlags = SDL_OPENGL | SDL_RESIZABLE;
    } else {
      _videoFlags = SDL_OPENGL | SDL_FULLSCREEN;
    }
    if (SDL_SetVideoMode(_width, _height, 0, _videoFlags) == NULL) {
      throw SDLInitFailedException("Unable to create SDL screen: " +
                                   std::string(SDL_GetError()));
    }
    glViewport(0, 0, width(), height());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    resized(_width, _height);
    SDL_ShowCursor(SDL_DISABLE);
    init();
  }

  // Reset a viewport when the screen is resized.
  void screenResized() {
    int screen_width = _width;
    int screen_height = _height;
    if (SDL_SetVideoMode(screen_width, screen_height, 0, _videoFlags) == NULL) {
      throw SDLInitFailedException("Unable to resize SDL screen: " +
                                   std::string(SDL_GetError()));
    }

    // adjust width and height to maintain correct aspect ratio
    if (screen_width * 480 > screen_height * 640)
      _width = screen_height * 640 / 480;
    else if (screen_width * 480 < screen_height * 640)
      _height = screen_width * 480 / 640;

    glViewport((screen_width - _width) / 2, screen_height - _height, _width,
               _height);
    glMatrixMode(GL_PROJECTION);
    setPerspective();
    glMatrixMode(GL_MODELVIEW);
  }

  void setPerspective() {
    glLoadIdentity();
    // gluPerspective(45.0f, cast(GLfloat) width / cast(GLfloat) height,
    // nearPlane, farPlane);
    glFrustum(-_nearPlane, _nearPlane,
              -_nearPlane * (GLfloat)_height / (GLfloat)_width,
              _nearPlane * (GLfloat)_height / (GLfloat)_width, 0.1f, _farPlane);
  }

  void resized(int w, int h) {
    _width = w;
    _height = h;
    screenResized();
  }

  void closeSDL() {
    close();
    SDL_ShowCursor(SDL_ENABLE);
  }

  void flip() {
    handleError();
    SDL_GL_SwapBuffers();
  }

  void clear() { glClear(GL_COLOR_BUFFER_BIT); }

  void handleError() {
    GLenum error = glGetError();
    if (error == GL_NO_ERROR)
      return;
    closeSDL();
    throw std::runtime_error("OpenGL error(" + std::to_string(error) + ")");
  }

protected:
  void setCaption(const char *name) { SDL_WM_SetCaption(name, NULL); }

public:
  bool windowMode(bool v) { return _windowMode = v; }

  bool windowMode() { return _windowMode; }

  int width(int v) { return _width = v; }

  int width() { return _width; }

  int height(int v) { return _height = v; }

  int height() { return _height; }

  static void glVertex(const Vector &v) { glVertex3f(v.x, v.y, 0); }

  static void glVertex(const Vector3 &v) { glVertex3f(v.x, v.y, v.z); }

  static void glTranslate(const Vector &v) { glTranslatef(v.x, v.y, 0); }

  static void glTranslate(const Vector3 &v) { glTranslatef(v.x, v.y, v.z); }

  static void glRotate(float d, float x = 0, float y = 0, float z = 1) {
    glRotatef(d * 180 / PI, x, y, z);
  }

  static void setColor(float r, float g, float b, float a = 1) {
    glColor4f(r * _brightness, g * _brightness, b * _brightness, a);
  }

  static void setClearColor(float r, float g, float b, float a = 1) {
    glClearColor(r * _brightness, g * _brightness, b * _brightness, a);
  }

  static float brightness(float v) { return _brightness = v; }
};

#endif // SCREEN3D_H
