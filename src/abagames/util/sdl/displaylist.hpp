/*
 * $Id: displaylist.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2005 Kenta Cho. Some rights reserved.
 */

#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include <iostream>

#include <SDL/SDL_opengl.h>

#include "abagames/util/sdl/sdlexception.hpp"

/**
 * Display list of OpenGL.
 */
class DisplayList {
private:
  bool registered;
  int num;
  int idx;
  int enumIdx;

public:
  DisplayList(int num) {
    this->num = num;
    idx = glGenLists(num);
  }

  void beginNewList() {
    resetList();
    newList();
  }

  void nextNewList() {
    glEndList();
    enumIdx++;
    if (enumIdx >= (idx + num) || enumIdx < idx)
      throw SDLException("Can't create new list. Index out of bound.");
    glNewList(enumIdx, GL_COMPILE);
  }

  void endNewList() {
    glEndList();
    registered = true;
  }

  void resetList() { enumIdx = idx; }

  void newList() { glNewList(enumIdx, GL_COMPILE); }

  void endList() {
    glEndList();
    enumIdx++;
    registered = true;
  }

  void call(int i = 0) { glCallList(idx + i); }

  void close() {
    if (!registered)
      return;
    glDeleteLists(idx, num);
  }
};

#endif // DISPLAYLIST_H
