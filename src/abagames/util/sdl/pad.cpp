/*
 * $Id: pad.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include "pad.hpp"

PadState *Pad::getState() {
  int x = 0, y = 0;
  state->dir = 0;
  if (stick) {
    x = SDL_JoystickGetAxis(stick, 0);
    y = SDL_JoystickGetAxis(stick, 1);
  }
  if (keys[SDLK_RIGHT] == SDL_PRESSED || keys[SDLK_KP6] == SDL_PRESSED ||
      keys[SDLK_d] == SDL_PRESSED || keys[SDLK_l] == SDL_PRESSED ||
      x > JOYSTICK_AXIS)
    state->dir |= PadState::Dir::RIGHT;
  if (keys[SDLK_LEFT] == SDL_PRESSED || keys[SDLK_KP4] == SDL_PRESSED ||
      keys[SDLK_a] == SDL_PRESSED || keys[SDLK_j] == SDL_PRESSED ||
      x < -JOYSTICK_AXIS)
    state->dir |= PadState::Dir::LEFT;
  if (keys[SDLK_DOWN] == SDL_PRESSED || keys[SDLK_KP2] == SDL_PRESSED ||
      keys[SDLK_s] == SDL_PRESSED || keys[SDLK_k] == SDL_PRESSED ||
      y > JOYSTICK_AXIS)
    state->dir |= PadState::Dir::DOWN;
  if (keys[SDLK_UP] == SDL_PRESSED || keys[SDLK_KP8] == SDL_PRESSED ||
      keys[SDLK_w] == SDL_PRESSED || keys[SDLK_i] == SDL_PRESSED ||
      y < -JOYSTICK_AXIS)
    state->dir |= PadState::Dir::UP;
  state->button = 0;
  int btn1 = 0, btn2 = 0;
  float leftTrigger = 0, rightTrigger = 0;
  if (stick) {
    btn1 = SDL_JoystickGetButton(stick, 0) + SDL_JoystickGetButton(stick, 2) +
           SDL_JoystickGetButton(stick, 4) + SDL_JoystickGetButton(stick, 6) +
           SDL_JoystickGetButton(stick, 8) + SDL_JoystickGetButton(stick, 10);
    btn2 = SDL_JoystickGetButton(stick, 1) + SDL_JoystickGetButton(stick, 3) +
           SDL_JoystickGetButton(stick, 5) + SDL_JoystickGetButton(stick, 7) +
           SDL_JoystickGetButton(stick, 9) + SDL_JoystickGetButton(stick, 11);
  }
  if (keys[SDLK_z] == SDL_PRESSED || keys[SDLK_PERIOD] == SDL_PRESSED ||
      keys[SDLK_LCTRL] == SDL_PRESSED || keys[SDLK_RCTRL] == SDL_PRESSED ||
      btn1) {
    if (!buttonsExchanged)
      state->button |= PadState::Button::A;
    else
      state->button |= PadState::Button::B;
  }
  if (keys[SDLK_x] == SDL_PRESSED || keys[SDLK_SLASH] == SDL_PRESSED ||
      keys[SDLK_LALT] == SDL_PRESSED || keys[SDLK_RALT] == SDL_PRESSED ||
      keys[SDLK_LSHIFT] == SDL_PRESSED || keys[SDLK_RSHIFT] == SDL_PRESSED ||
      keys[SDLK_RETURN] == SDL_PRESSED || btn2) {
    if (!buttonsExchanged)
      state->button |= PadState::Button::B;
    else
      state->button |= PadState::Button::A;
  }
  return state;
}
