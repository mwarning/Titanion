/*
 * $Id: mainloop.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <SDL/SDL.h>

#include "abagames/util/logger.hpp"
#include "abagames/util/preference.hpp"
#include "abagames/util/rand.hpp"
#include "abagames/util/sdl/frame.hpp"
#include "abagames/util/sdl/input.hpp"
#include "abagames/util/sdl/screen.hpp"
#include "abagames/util/sdl/sdlexception.hpp"
#include "abagames/util/sdl/sound.hpp"

class Frame;

/**
 * SDL main loop.
 */
class MainLoop {
public:
  const int INTERVAL_BASE = 16;
  const int MAX_SKIP_FRAME = 5;
  bool noSlowdown = false;
  SDL_Event event;

private:
  Screen *screen;
  Input *input;
  Frame *frame;
  Preference *preference;
  float slowdownRatio;
  float interval = INTERVAL_BASE;
  float _slowdownStartRatio = 1;
  float _slowdownMaxRatio = 1.5f;

public:
  MainLoop(Screen *screen, Input *input, Frame *frame, Preference *preference);

private:
  // Initialize and load a preference.
  void initFirst();

  // Quit and save a preference.
  void quitLast();

  bool done;

public:
  void breakLoop() { done = true; }

  void loop();

  // Intentional slowdown.

  void initInterval() { interval = INTERVAL_BASE; }

  void addSlowdownRatio(float sr) { slowdownRatio += sr; }

  void calcInterval();

  float slowdownStartRatio(float v) { return _slowdownStartRatio = v; }

  float slowdownMaxRatio(float v) { return _slowdownMaxRatio = v; }
};

#endif
