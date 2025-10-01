/*
 * $Id: logger.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include "logger.hpp"

/**
 * Logger for error and info messages.
 */
#ifdef Win32_release

private
import std.string;
private
import std.c.windows.windows;

public
class Logger {

  static void info(char[] msg, bool nline = true) {}

  static void info(double n, bool nline = true) {}

private
  static void putMessage(char[] msg) {
    MessageBoxA(NULL, std.string.toStringz(msg), "Error",
                MB_OK | MB_ICONEXCLAMATION);
  }

  static void error(char[] msg) { putMessage("Error: " ~msg); }

  static void error(Exception e) { putMessage("Error: " ~e.toString()); }

  static void error(Error e) { putMessage("Error: " ~e.toString()); }
}

#else

namespace Logger {
void info(const std::string &msg, bool nline) {
  if (nline)
    std::cout << msg << std::endl;
  else
    std::cout << msg << std::flush;
}

void info(double n, bool nline) {
  if (nline)
    std::cout << n << std::endl;
  else
    std::cout << n << std::flush;
}

void error(const std::string &msg) {
  std::cout << "Error: " << msg << std::endl;
}

void error(std::exception &e) {
  std::cout << "Error: " << e.what() << std::endl;
}
} // namespace Logger

#endif
