/*
 * $Id: input.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2004 Kenta Cho. Some rights reserved.
 */

#ifndef INPUT_H
#define INPUT_H

#include <SDL/SDL.h>

#include <vector>

/**
 * Input device interface.
 */
struct Input {
  virtual void handleEvent(SDL_Event *event) = 0;
};

class MultipleInputDevice : Input {
public:
  std::vector<Input> inputs;

  void handleEvent(SDL_Event *event) {
    for (auto &i : inputs) {
      i.handleEvent(event);
    }
  }
};

#endif // INPUT_H
