/*
 * $Id: preference.d,v 1.4 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef PREFERENCEIMPL_H
#define PREFERENCEIMPL_H

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sys/stat.h> // mkdir()

#include "abagames/util/preference.hpp"

/**
 * Load/Save/Record a high score table.
 */
class PreferenceImpl : public Preference {
public:
  static constexpr int RANKING_NUM = 10;
  static constexpr int MODE_NUM = 3;

private:
  static constexpr int VERSION_NUM = 30;
  static constexpr const char *PREF_FILE_NAME = "ttn.prf";
  int _highScore[MODE_NUM][RANKING_NUM];
  int _lastMode;

public:
  static std::string pref_dir() {
    char *home = getenv("HOME");
    if (home == NULL)
      throw std::runtime_error("HOME environment variable not defined");
    std::string dir = std::string(home) + "/.titanion";
    try {
      mkdir(dir.c_str(), 0777);
    } catch (const std::exception &e) {
    }
    return dir;
  }

  void load();

  void init() {
    _lastMode = 2;
    for (int j = 0; j < MODE_NUM; j++)
      for (int i = 0; i < RANKING_NUM; i++)
        _highScore[j][i] = (10 - i) * 10000;
  }

  void save() {
    std::ofstream fd;
    fd.open(pref_dir() + "/" + PREF_FILE_NAME);
    fd << VERSION_NUM;
    fd << _lastMode;
    for (int j = 0; j < MODE_NUM; j++)
      for (int i = 0; i < RANKING_NUM; i++)
        fd << _highScore[j][i];
    fd.close();
  }

  void setMode(int mode) { _lastMode = mode; }

  void recordResult(int score, int mode) {
    setMode(mode);
    for (int i = 0; i < RANKING_NUM; i++) {
      if (score > _highScore[mode][i]) {
        for (int j = RANKING_NUM - 1; j >= i + 1; j--) {
          _highScore[mode][j] = _highScore[mode][j - 1];
        }
        _highScore[mode][i] = score;
        return;
      }
    }
  }

  // added
  int getHighScore(int cursorIdx, int i) { return _highScore[cursorIdx][i]; }

  int * /*[RANKING_NUM][]*/ highScore() { return &_highScore[0][0]; }

  int lastMode() { return _lastMode; }
};

#endif // PREFERENCEIMPL_H
