/*
 * $Id: replay.d,v 1.4 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef REPLAY_H
#define REPLAY_H

#include <iostream>

#include "abagames/ttn/preference.hpp"
#include "abagames/util/sdl/pad.hpp"
#include "abagames/util/sdl/recordableinput.hpp"

/**
 * Save/Load a replay data.
 */
class ReplayData {
public:
  // static constexpr const char *DIR = "replay";
  static constexpr int VERSION_NUM = 30;
  InputRecord<PadState> *inputRecord;
  long seed;
  int score = 0;
  int mode;
  bool stageRandomized;

public:
  void save(const char *fileName) {
    std::ofstream fd;
    fd.open(PreferenceImpl::pref_dir() + "/" + fileName);
    fd << VERSION_NUM;
    fd << seed;
    fd << score;
    fd << mode;
    fd << (uint8_t)stageRandomized;
    inputRecord->save(fd);
    fd.close();
  }

  void load(const char *fileName) {
    std::ifstream fd;
    fd.open(PreferenceImpl::pref_dir() + "/" + fileName);
    int ver;
    fd >> ver;
    if (ver != VERSION_NUM)
      throw std::runtime_error("Wrong version num");
    fd >> seed;
    fd >> score;
    fd >> mode;
    uint8_t sr;
    fd >> sr;
    stageRandomized = (bool)sr;
    inputRecord = new InputRecord<PadState>();
    inputRecord->load(fd);
    fd.close();
  }
};

#endif // REPLAY_H
