/*
 * $Id: frame.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef FRAME_H
#define FRAME_H

#include "abagames/util/preference.hpp"
#include "abagames/util/sdl/input.hpp"
#include "abagames/util/sdl/mainloop.hpp"
#include "abagames/util/sdl/screen.hpp"

class MainLoop;

/**
 * Lifecycle of the game.
 */
class Frame {
protected:
  MainLoop *mainLoop;
  Screen *abstractScreen;
  Input *abstractInput;
  Preference *abstractPreference;

public:
  void setMainLoop(MainLoop *mainLoop) { this->mainLoop = mainLoop; }

  void setUIs(Screen *screen, Input *input) {
    abstractScreen = screen;
    abstractInput = input;
  }

  void setPreference(Preference *preference) {
    abstractPreference = preference;
  }

  virtual void addSlowdownRatio(float sr) = 0;

  virtual void init() = 0;
  virtual void start() = 0;
  virtual void quit() = 0;
  virtual void move() = 0;
  virtual void draw() = 0;
};

#endif // FRAME_H
