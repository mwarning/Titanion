/*
 * $Id: tokenizer.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>
#include <string>
#include <vector>

/**
 * Tokenizer.
 */
class Tokenizer {
public:
  static void readFile(std::vector<std::string> &args,
                       const std::string &fileName,
                       const std::string &separator);
};

#endif // TOKENIZER_H
