/*
 * $Id: boot.d,v 1.3 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

#include "abagames/ttn/enemy.hpp"
#include "abagames/ttn/frame.hpp"
#include "abagames/ttn/preference.hpp"
#include "abagames/ttn/screen.hpp"
#include "abagames/util/logger.hpp"
#include "abagames/util/sdl/input.hpp"
#include "abagames/util/sdl/mainloop.hpp"
#include "abagames/util/sdl/pad.hpp"
#include "abagames/util/sdl/recordableinput.hpp"
#include "abagames/util/sdl/sound.hpp"
#include "abagames/util/tokenizer.hpp"

/**
 * Boot the game.
 */
ScreenImpl *screen = NULL;
RecordablePad *input = NULL;
FrameImpl *frame = NULL;
Preference *preference = NULL;
MainLoop *mainLoop = NULL;

void usage(const char *progName) {
  Logger::error(std::string("Usage: ") + progName +
                " [-window] [-fullscreen] [-res x y] [-brightness [0-100]] "
                "[-nosound] [-bgmvol [0-128]] [-sevol [0-128]] [-exchange] "
                "[-trail] [-noslowdown] [-randomized]");
}

static const char *OPTIONS_INI_FILE = "options.ini";

void readOptionsIniFile(std::vector<std::string> &args) {
  try {
    Tokenizer::readFile(args, OPTIONS_INI_FILE, " ");
  } catch (std::exception &e) {
  }
}

void parseArgs(const char **commandArgs, ScreenImpl &screen, RecordablePad &pad,
               MainLoop &mainLoop) {
  std::vector<std::string> args;
  for (int i = 1; commandArgs[i]; i++)
    args.push_back(commandArgs[i]);

  readOptionsIniFile(args);

  const char *progName = commandArgs[0];
  for (size_t i = 0; i < args.size(); i++) {
    if (args[i] == "-fullscreen") {
      screen.windowMode(false);
    } else if (args[i] == "-window") {
      screen.windowMode(true);
    } else if (args[i] == "-res") {
      if (i >= args.size() - 2) {
        usage(progName);
        throw std::runtime_error("Invalid options");
      }
      i++;
      int w = std::stoi(args[i]);
      i++;
      int h = std::stoi(args[i]);
      screen.width(w);
      screen.height(h);
    } else if (args[i] == "-brightness") {
      if (i >= args.size() - 1) {
        usage(progName);
        throw std::runtime_error("Invalid options");
      }
      i++;
      float b = (float)std::stoi(args[i]) / 100;
      if (b < 0 || b > 1) {
        usage(progName);
        throw std::runtime_error("Invalid options");
      }
      Screen3D::brightness(b);
    } else if (args[i] == "-nosound") {
      Sound::noSound = true;
    } else if (args[i] == "-bgmvol") {
      if (i >= args.size() - 1) {
        usage(progName);
        throw std::runtime_error("Invalid options");
      }
      i++;
      int v = std::stoi(args[i]);
      if (v < 0 || v > 128) {
        usage(progName);
        throw std::runtime_error("Invalid options");
      }
      Sound::bgmVol = v;
    } else if (args[i] == "-sevol") {
      if (i >= args.size() - 1) {
        usage(progName);
        throw std::runtime_error("Invalid options");
      }
      i++;
      int v = std::stoi(args[i]);
      if (v < 0 || v > 128) {
        usage(progName);
        throw std::runtime_error("Invalid options");
      }
      Sound::seVol = v;
    } else if (args[i] == "-exchange") {
      pad.buttonsExchanged = true;
    } else if (args[i] == "-trail") {
      EnemyPool::trailEffect = true;
    } else if (args[i] == "-noslowdown") {
      mainLoop.noSlowdown = true;
    } else if (args[i] == "-randomized") {
      GameState::stageRandomized = true;
    } else {
      usage(progName);
      throw std::runtime_error("Invalid options");
    }
  }
}

int boot(int argc, const char **args) {
  screen = new ScreenImpl();
  input = new RecordablePad();
  frame = new FrameImpl();
  preference = new PreferenceImpl();
  mainLoop = new MainLoop(screen, input, frame, preference);
  try {
    parseArgs(args, *screen, *input, *mainLoop);
  } catch (std::exception &e) {
    return EXIT_FAILURE;
  }
  mainLoop->loop();
  return EXIT_SUCCESS;
}

/*
version (Win32_release) {
  // Boot as the Windows executable.
  private import std.c.windows.windows;
  private import std.string;

  extern (C) void gc_init();
  extern (C) void gc_term();
  extern (C) void _minit();
  extern (C) void _moduleCtor();

  extern (Windows)
    public int WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow) {
    int result;
    gc_init();
    _minit();
    try {
      _moduleCtor();
      char exe[4096];
      GetModuleFileNameA(NULL, exe, 4096);
      char[][1] prog;
      prog[0] = std.string.toString(exe);
      result = boot(prog + std.string.split(std.string.toString(lpCmdLine)));
    } catch (std::exception &e) {
      Logger.error("Exception: " + e.what());
      result = EXIT_FAILURE;
    }
    gc_term();
    return result;
  }
} else {
*/
// Boot as the general executable.
int main(int argc, const char **args) { return boot(argc, args); }
//}
