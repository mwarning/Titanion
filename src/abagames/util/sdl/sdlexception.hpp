/*
 * $Id: sdlexception.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2004 Kenta Cho. Some rights reserved.
 */

#ifndef SDLEXCEPTION_H
#define SDLEXCEPTION_H

#include <stdexcept>

/**
 * SDL initialize failed.
 */
struct SDLInitFailedException : public virtual std::runtime_error {
  using std::runtime_error::runtime_error;
};

/**
 * SDL general exception.
 */
struct SDLException : public virtual std::runtime_error {
  using std::runtime_error::runtime_error;
};

#endif // SDLEXCEPTION_H
