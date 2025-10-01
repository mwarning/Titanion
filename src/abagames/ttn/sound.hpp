/*
 * $Id: sound.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef SOUNDIMPL_H
#define SOUNDIMPL_H

#include <dirent.h>
#include <map>
#include <sys/types.h>
#include <vector>

#include "abagames/util/logger.hpp"
#include "abagames/util/rand.hpp"
#include "abagames/util/sdl/sound.hpp"

/**
 * Manage BGMs and SEs.
 */
class SoundImpl : Sound {
public:
  static Rand rand;

private:
  static std::map<std::string, Music *> bgm;
  static std::map<std::string, Chunk *> se;
  static std::map<std::string, bool> seMark;
  static std::vector<std::string> bgmFileName;
  static std::string currentBgm;
  static int prevBgmIdx;
  static int nextIdxMv;
  static bool _bgmEnabled;
  static bool _seEnabled;

public:
  static void load() {
    loadMusics();
    loadChunks();
  }

private:
  static void loadMusics();

  static void loadChunks();

public:
  static void playBgm(const std::string &name) {
    currentBgm = name;
    if (!_bgmEnabled)
      return;
    Music::halt();
    bgm[name]->play();
  }

  static void playBgm() {
    int bgmIdx = rand.nextInt(bgm.size());
    nextIdxMv = rand.nextInt(2) * 2 - 1;
    prevBgmIdx = bgmIdx;
    playBgm(bgmFileName[bgmIdx]);
  }

  static void nextBgm() {
    int bgmIdx = prevBgmIdx + nextIdxMv;
    if (bgmIdx < 0)
      bgmIdx = bgm.size() - 1;
    else if (bgmIdx >= (int)bgm.size())
      bgmIdx = 0;
    prevBgmIdx = bgmIdx;
    playBgm(bgmFileName[bgmIdx]);
  }

  static void playCurrentBgm() { playBgm(currentBgm); }

  static void fadeBgm() { Music::fade(); }

  static void haltBgm() { Music::halt(); }

  static void playSe(const std::string &name) {
    if (!_seEnabled)
      return;
    seMark[name] = true;
  }

  static void playMarkedSes() {
    for (auto const &e : seMark) {
      auto key = e.first;
      if (seMark[key]) {
        se[key]->play();
        seMark[key] = false;
      }
    }
  }

  static void clearMarkedSes() {
    for (auto &e : seMark) {
      e.second = false;
    }
  }

  static void bgmEnabled(bool v) { _bgmEnabled = v; }

  static void seEnabled(bool v) { _seEnabled = v; }
};

#endif // SOUNDIMPL_H
