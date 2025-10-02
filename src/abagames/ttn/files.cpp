
#include "files.hpp"

#include <cstdint>
#include <iostream>
#include <stdexcept>

#ifdef USE_INTERNAL_FILES

struct InternalFile {
  const char *fileName;
  const uint8_t *start;
  const uint8_t *end;
};

extern const uint8_t _binary_sounds_musics_ttn1_ogg_start[];
extern const uint8_t _binary_sounds_musics_ttn1_ogg_end[];
extern const uint8_t _binary_sounds_musics_ttn2_ogg_start[];
extern const uint8_t _binary_sounds_musics_ttn2_ogg_end[];
extern const uint8_t _binary_sounds_musics_ttn3_ogg_start[];
extern const uint8_t _binary_sounds_musics_ttn3_ogg_end[];
extern const uint8_t _binary_sounds_chunks_explosion1_wav_start[];
extern const uint8_t _binary_sounds_chunks_explosion1_wav_end[];
extern const uint8_t _binary_sounds_chunks_explosion2_wav_start[];
extern const uint8_t _binary_sounds_chunks_explosion2_wav_end[];
extern const uint8_t _binary_sounds_chunks_explosion3_wav_start[];
extern const uint8_t _binary_sounds_chunks_explosion3_wav_end[];
extern const uint8_t _binary_sounds_chunks_extend_wav_start[];
extern const uint8_t _binary_sounds_chunks_extend_wav_end[];
extern const uint8_t _binary_sounds_chunks_flick_wav_start[];
extern const uint8_t _binary_sounds_chunks_flick_wav_end[];
extern const uint8_t _binary_sounds_chunks_flying_down_wav_start[];
extern const uint8_t _binary_sounds_chunks_flying_down_wav_end[];
extern const uint8_t _binary_sounds_chunks_player_explosion_wav_start[];
extern const uint8_t _binary_sounds_chunks_player_explosion_wav_end[];
extern const uint8_t _binary_sounds_chunks_shot_wav_start[];
extern const uint8_t _binary_sounds_chunks_shot_wav_end[];
extern const uint8_t _binary_sounds_chunks_tractor_wav_start[];
extern const uint8_t _binary_sounds_chunks_tractor_wav_end[];
extern const uint8_t _binary_images_title_bmp_start[];
extern const uint8_t _binary_images_title_bmp_end[];
extern const uint8_t _binary_images_ttn_icon32_bmp_start[];
extern const uint8_t _binary_images_ttn_icon32_bmp_end[];

static struct InternalFile internalFiles[] = {
    {"sounds/musics/ttn1.ogg", _binary_sounds_musics_ttn1_ogg_start,
     _binary_sounds_musics_ttn1_ogg_end},
    {"sounds/musics/ttn2.ogg", _binary_sounds_musics_ttn2_ogg_start,
     _binary_sounds_musics_ttn2_ogg_end},
    {"sounds/musics/ttn3.ogg", _binary_sounds_musics_ttn3_ogg_start,
     _binary_sounds_musics_ttn3_ogg_end},
    {"sounds/chunks/explosion1.wav", _binary_sounds_chunks_explosion1_wav_start,
     _binary_sounds_chunks_explosion1_wav_end},
    {"sounds/chunks/explosion2.wav", _binary_sounds_chunks_explosion2_wav_start,
     _binary_sounds_chunks_explosion2_wav_end},
    {"sounds/chunks/explosion3.wav", _binary_sounds_chunks_explosion3_wav_start,
     _binary_sounds_chunks_explosion3_wav_end},
    {"sounds/chunks/extend.wav", _binary_sounds_chunks_extend_wav_start,
     _binary_sounds_chunks_extend_wav_end},
    {"sounds/chunks/flick.wav", _binary_sounds_chunks_flick_wav_start,
     _binary_sounds_chunks_flick_wav_end},
    {"sounds/chunks/flying_down.wav",
     _binary_sounds_chunks_flying_down_wav_start,
     _binary_sounds_chunks_flying_down_wav_end},
    {"sounds/chunks/player_explosion.wav",
     _binary_sounds_chunks_player_explosion_wav_start,
     _binary_sounds_chunks_player_explosion_wav_end},
    {"sounds/chunks/shot.wav", _binary_sounds_chunks_shot_wav_start,
     _binary_sounds_chunks_shot_wav_end},
    {"sounds/chunks/tractor.wav", _binary_sounds_chunks_tractor_wav_start,
     _binary_sounds_chunks_tractor_wav_end},
    {"images/title.bmp", _binary_images_title_bmp_start,
     _binary_images_title_bmp_end},
    {"images/ttn_icon32.bmp", _binary_images_ttn_icon32_bmp_start,
     _binary_images_ttn_icon32_bmp_end}};

SDL_RWops *SDL_file_wrapper(const std::string &fileName) {
  for (auto &data : internalFiles) {
    if (fileName == data.fileName) {
      return SDL_RWFromConstMem(data.start, data.end - data.start);
    }
  }
  throw std::runtime_error(
      std::string("Failed to find file in internal storage: ") + fileName);
}

#else
SDL_RWops *SDL_file_wrapper(const std::string &fileName) {
  throw std::runtime_error(
      std::string("Internal files disabled, tried to access ") + fileName);
}
#endif
