/*
 * $Id: mainloop.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include "mainloop.hpp"

/**
 * SDL main loop.
 */

MainLoop::MainLoop(Screen *screen, Input *input, Frame *frame,
                   Preference *preference) {
  this->screen = screen;
  this->input = input;
  this->frame = frame;
  this->preference = preference;
  frame->setMainLoop(this);
  frame->setUIs(screen, input);
  frame->setPreference(preference);
}

// Initialize and load a preference.
void MainLoop::initFirst() {
  preference->load();
  try {
    Sound::init();
  } catch (SDLInitFailedException &e) {
    Logger::error(e);
  }
  frame->init();
  initInterval();
}

// Quit and save a preference.
void MainLoop::quitLast() {
  frame->quit();
  Sound::close();
  preference->save();
  screen->closeSDL();
  SDL_Quit();
}

void MainLoop::loop() {
  done = false;
  long prvTickCount = 0;
  int i;
  long nowTick;
  int frameNum;
  screen->initSDL();
  initFirst();
  frame->start();
  while (!done) {
    if (SDL_PollEvent(&event) == 0)
      event.type = SDL_USEREVENT;
    input->handleEvent(&event);
    if (event.type == SDL_QUIT) {
      breakLoop();
      break;
    }
    nowTick = SDL_GetTicks();
    int itv = (int)interval;
    frameNum = (int)(nowTick - prvTickCount) / itv;
    if (frameNum <= 0) {
      frameNum = 1;
      SDL_Delay(prvTickCount + itv - nowTick);
      prvTickCount += interval;
    } else if (frameNum > MAX_SKIP_FRAME) {
      frameNum = MAX_SKIP_FRAME;
      prvTickCount = nowTick;
    } else {
      // prvTickCount += frame * interval;
      prvTickCount = nowTick;
    }
    slowdownRatio = 0;
    for (i = 0; i < frameNum; i++)
      frame->move();
    slowdownRatio /= frameNum;
    screen->clear();
    frame->draw();
    screen->flip();
    if (!noSlowdown)
      calcInterval();
  }
  quitLast();
}

void MainLoop::calcInterval() {
  if (slowdownRatio > _slowdownStartRatio) {
    float sr = slowdownRatio / _slowdownStartRatio;
    if (sr > _slowdownMaxRatio)
      sr = _slowdownMaxRatio;
    interval += (sr * INTERVAL_BASE - interval) * 0.1;
  } else {
    interval += (INTERVAL_BASE - interval) * 0.08;
  }
}
