/*
 * $Id: tokenizer.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include <fstream>
#include <iostream>
#include <string>

#include "tokenizer.hpp"

std::vector<std::string> split(const std::string &str, const std::string &sep) {
  // TODO
  return std::vector<std::string>();
}

std::string strip(const std::string &str) {
  // TODO
  return "";
}

/**
 * Tokenizer.
 */
void Tokenizer::readFile(std::vector<std::string> &args,
                         const std::string &fileName,
                         const std::string &separator) {
  std::ifstream file(fileName);
  std::string line;

  if (file.is_open()) {
    while (getline(file, line)) {
      auto spl = split(line, separator);
      for (auto &s : spl) {
        std::string r = strip(s);
        if (r.size() > 0)
          args.push_back(r);
      }
    }
  }

  file.close();
}

/**
 * CSV format tokenizer.
 */
class CSVTokenizer {
  static std::vector<std::string> readFile(const std::string &fileName) {
    std::vector<std::string> args;
    Tokenizer::readFile(args, fileName, ",");
    return args;
  }
};
