/*
 * $Id: sound.d,v 1.2 2006/12/04 16:04:27 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef SOUND_H
#define SOUND_H

/**
 * Initialize and close SDL_mixer.
 */

#include <iostream>
#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "abagames/ttn/files.hpp"
#include "abagames/util/sdl/sdlexception.hpp"

/**
 * Initialize and close SDL_mixer.
 */

struct Sound {
public:
  static bool noSound;
  static int bgmVol;
  static int seVol;

  static void init() {
    if (noSound)
      return;

    int audio_rate;
    Uint16 audio_format;
    int audio_channels;
    int audio_buffers;
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
      noSound = true;
      throw SDLInitFailedException("Unable to initialize SDL_AUDIO: " +
                                   std::string(SDL_GetError()));
    }
    audio_rate = 44100;
    audio_format = AUDIO_S16;
    audio_channels = 1;
    audio_buffers = 4096;
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) <
        0) {
      noSound = true;
      throw SDLInitFailedException("Couldn't open audio: " +
                                   std::string(SDL_GetError()));
    }
    Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
    Mix_VolumeMusic(bgmVol);
    Mix_Volume(-1, seVol);
  }

  static void close() {
    if (noSound)
      return;
    if (Mix_PlayingMusic())
      Mix_HaltMusic();
    Mix_CloseAudio();
  }
};

/**
 * Music.
 */
class Music {
public:
  static constexpr int fadeOutSpeed = 1280;
  static constexpr const char *music_dir = "sounds/musics";

private:
  Mix_Music *music;

public:
  void load(const std::string &name) {
    if (Sound::noSound)
      return;
    std::string fileName = std::string(music_dir) + "/" + name;

#ifdef USE_INTERNAL_FILES
    music = Mix_LoadMUS_RW(SDL_file_wrapper(fileName));
#else
    music = Mix_LoadMUS(fileName.c_str());
#endif

    if (!music) {
      Sound::noSound = true;
      throw SDLException("Couldn't load: " + fileName + " (" +
                         std::string(Mix_GetError()) + ")");
    }
  }

  void free() {
    if (music) {
      halt();
      Mix_FreeMusic(music);
    }
  }

  void play() {
    if (Sound::noSound)
      return;
    Mix_PlayMusic(music, -1);
  }

  void playOnce() {
    if (Sound::noSound)
      return;
    Mix_PlayMusic(music, 1);
  }

  static void fade() {
    if (Sound::noSound)
      return;
    Mix_FadeOutMusic(fadeOutSpeed);
  }

  static void halt() {
    if (Sound::noSound)
      return;
    if (Mix_PlayingMusic())
      Mix_HaltMusic();
  }
};

/**
 * Sound chunk.
 */
class Chunk {
public:
  static constexpr const char *chunk_dir = "sounds/chunks";

private:
  Mix_Chunk *chunk;
  int chunkChannel;

public:
  void load(const char *name, int ch) {
    if (Sound::noSound)
      return;
    std::string fileName = std::string(chunk_dir) + "/" + name;

#ifdef USE_INTERNAL_FILES
    chunk = Mix_LoadWAV_RW(SDL_file_wrapper(fileName), 0);
#else
    chunk = Mix_LoadWAV(fileName.c_str());
#endif

    if (!chunk) {
      Sound::noSound = true;
      throw SDLException("Couldn't load: " + fileName + " (" +
                         std::string(Mix_GetError()) + ")");
    }
    chunkChannel = ch;
  }

  void free() {
    if (chunk) {
      halt();
      Mix_FreeChunk(chunk);
    }
  }

  void play() {
    if (Sound::noSound)
      return;
    Mix_PlayChannel(chunkChannel, chunk, 0);
  }

  void halt() {
    if (Sound::noSound)
      return;
    Mix_HaltChannel(chunkChannel);
  }
};

#endif // SOUND_H
