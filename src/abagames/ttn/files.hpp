#ifndef FILES_H
#define FILES_H

/*
 * Load SDL file handler from files included in this binary.
 */

#include <SDL/SDL_rwops.h>

#include <string>

SDL_RWops *SDL_file_wrapper(const std::string &fileName);

#endif // FILES_H
