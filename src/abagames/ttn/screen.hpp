/*
 * $Id: screen.d,v 1.2 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef SCREENIMPL_H
#define SCREENIMPL_H

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "abagames/util/sdl/screen3d.hpp"

class Field;

/**
 * OpenGL screen.
 */
class ScreenImpl : public Screen3D {
private:
  static constexpr const char *CAPTION = "Titanion";
  static constexpr const char *ICON_FILE_NAME = "images/ttn_icon32.bmp";
  Field *field;

protected:
  void setIcon();

  void init();

public:
  void setField(Field *field);

protected:
  void close();

public:
  void screenResized();
};

#endif // SCREENIMPL_H
