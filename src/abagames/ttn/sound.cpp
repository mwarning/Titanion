/*
 * $Id: sound.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include "sound.hpp"

static const std::string seFileName[] = {
    "shot.wav",    "explosion1.wav",  "explosion2.wav",       "explosion3.wav",
    "tractor.wav", "flying_down.wav", "player_explosion.wav", "flick.wav",
    "extend.wav"};
static const int seChannel[] = {0, 1, 2, 3, 4, 5, 6, 6, 7};

Rand SoundImpl::rand;
std::map<std::string, Music *> SoundImpl::bgm;
std::map<std::string, Chunk *> SoundImpl::se;
std::map<std::string, bool> SoundImpl::seMark;
std::vector<std::string> SoundImpl::bgmFileName;
std::string SoundImpl::currentBgm;
int SoundImpl::prevBgmIdx;
int SoundImpl::nextIdxMv;
bool SoundImpl::_bgmEnabled = true;
bool SoundImpl::_seEnabled = true;

static std::vector<std::string> listdir(const char *base) {
  std::vector<std::string> list;
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(base)) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      list.push_back(ent->d_name);
    }
    closedir(dir);
  } else {
    Logger::error(std::string("Failed to open ") + base);
  }
  return list;
}

static std::string getExt(const std::string &base) {
  auto idx = base.rfind('.');
  if (idx != std::string::npos) {
    return base.substr(idx + 1);
  }

  return "";
}

void SoundImpl::loadMusics() {
  std::map<const char *, Music *> musics;
  auto files = listdir(Music::music_dir);
  for (auto fileName : files) {
    auto ext = getExt(fileName);
    if (ext != "ogg" && ext != "wav")
      continue;
    Music *music = new Music();
    music->load(fileName);
    bgm[fileName] = music;
    bgmFileName.push_back(fileName);
    Logger::info("Load bgm: " + fileName);
  }
}

void SoundImpl::loadChunks() {
  int i = 0;
  for (auto fileName : seFileName) {
    Chunk *chunk = new Chunk();
    chunk->load(fileName.c_str(), seChannel[i]);
    se[fileName] = chunk;
    seMark[fileName] = false;
    Logger::info("Load SE: " + fileName);
    i++;
  }
}
