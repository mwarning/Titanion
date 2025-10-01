/*
 * $Id: pad.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef PAD_H
#define PAD_H

#include <fstream>
#include <iostream>
#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_keyboard.h>

#include "abagames/util/sdl/input.hpp"
#include "abagames/util/sdl/recordableinput.hpp"

/**
 * Inputs from a joystick and a keyboard.
 */

class PadState {
public:
  enum Dir {
    UP = 1,
    DOWN = 2,
    LEFT = 4,
    RIGHT = 8,
  };
  enum Button {
    A = 16,
    B = 32,
    ANY = 48,
  };
  int dir, button;

  PadState() : dir(0), button(0) {}

  PadState(const PadState &p) : dir(p.dir), button(p.button) {}

  static PadState *newInstance() { return new PadState(); }

  static PadState *newInstance(PadState *s) { return new PadState(*s); }

  PadState(PadState &s) { set(s); }

  void set(const PadState &s) {
    dir = s.dir;
    button = s.button;
  }

  void clear() {
    dir = 0;
    button = 0;
  }

  void read(std::ifstream &fd) {
    int s;
    fd >> s;
    dir = s & (Dir::UP | Dir::DOWN | Dir::LEFT | Dir::RIGHT);
    button = s & Button::ANY;
  }

  void write(std::ofstream &fd) {
    int s = dir | button;
    fd << s;
  }

  bool equals(PadState *s) {
    if (dir == s->dir && button == s->button)
      return true;
    else
      return false;
  }
};

class Pad : public Input {
public:
  const Uint8 *keys;
  bool buttonsExchanged = false;

private:
  SDL_Joystick *stick = NULL;
  const int JOYSTICK_AXIS = 16384;
  PadState *state;

public:
  Pad() { this->state = new PadState(); }

  SDL_Joystick *openJoystick(SDL_Joystick *st = NULL) {
    if (st == NULL) {
      if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
        return NULL;
      stick = SDL_JoystickOpen(0);
    } else {
      stick = st;
    }
    return stick;
  }

  void handleEvent(SDL_Event *event) { keys = SDL_GetKeyState(NULL); }

  PadState *getState();

  PadState *getNullState() {
    state->clear();
    return state;
  }
};

class RecordablePad : public RecordableInput<PadState>, public Pad {
public:
  PadState *getState(bool doRecord = true) {
    auto s = Pad::getState();
    if (doRecord)
      record(s);
    return s;
  }
};

#endif // PAD_H
