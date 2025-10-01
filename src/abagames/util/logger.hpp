
#ifndef LOGGER_H
#define LOGGER_H

#include <exception>
#include <iostream>
#include <string>

namespace Logger {
void info(const std::string &msg, bool nline = true);
void info(double n, bool nline = true);
void error(const std::string &msg);
void error(std::exception &e);
} // namespace Logger

#endif // LOGGER_H
