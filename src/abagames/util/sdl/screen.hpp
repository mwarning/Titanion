/*
 * $Id: screen.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2005 Kenta Cho. Some rights reserved.
 */

#ifndef SCREEN_H
#define SCREEN_H

/**
 * SDL screen handler interface.
 */
struct Screen {
  virtual void initSDL() = 0;
  virtual void closeSDL() = 0;
  virtual void flip() = 0;
  virtual void clear() = 0;
};

struct SizableScreen {
  virtual bool windowMode() = 0;
  virtual int width() = 0;
  virtual int height() = 0;
};

#endif // SCREEN_H
