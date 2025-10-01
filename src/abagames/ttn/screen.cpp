/*
 * $Id: screen.d,v 1.2 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "abagames/ttn/field.hpp"
#include "abagames/util/sdl/screen3d.hpp"

/**
 * OpenGL screen.
 */

void ScreenImpl::setIcon() {
  SDL_WM_SetIcon(SDL_LoadBMP(ICON_FILE_NAME), NULL);
}

void ScreenImpl::init() {
  setCaption(CAPTION);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_BLEND);
  glEnable(GL_LINE_SMOOTH);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  setClearColor(0, 0, 0, 1);
}

void ScreenImpl::setField(Field *field) {
  this->field = field;
  screenResized();
}

void ScreenImpl::close() {}

void ScreenImpl::screenResized() {
  Screen3D::screenResized();
  float lw = ((float)width() / 640 + (float)height() / 480) / 2;
  if (lw < 1)
    lw = 1;
  else if (lw > 4)
    lw = 4;
  glLineWidth(lw);
  if (field)
    field->setLookAt();
}
