/*
 * $Id: letter.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef LETTER_H
#define LETTER_H

#include <SDL/SDL_opengl.h>
#include <cassert>
#include <cmath>

#include "abagames/ttn/screen.hpp"
#include "abagames/util/sdl/displaylist.hpp"

/**
 * Letters.
 */

class Letter {
public:
  static DisplayList *displayList;
  static constexpr float LETTER_WIDTH = 2.1f;
  static constexpr float LETTER_HEIGHT = 3.0f;
  enum Direction {
    TO_RIGHT,
    TO_DOWN,
    TO_LEFT,
    TO_UP,
  };

private:
  static constexpr int LETTER_NUM = 44;
  static constexpr int DISPLAY_LIST_NUM = LETTER_NUM * 3;
  enum Shape {
    NORMAL,
    POLYGON,
    LINE,
  };

public:
  static void init() {
    displayList = new DisplayList(DISPLAY_LIST_NUM);
    displayList->resetList();
    for (int j = 0; j < 3; j++) {
      for (int i = 0; i < LETTER_NUM; i++) {
        displayList->newList();
        setLetter(i, j);
        displayList->endList();
      }
    }
  }

  static void close() { displayList->close(); }

  static float getWidth(int n, float s) { return n * s * LETTER_WIDTH; }

  static float getWidthNum(int num, float s) {
    // int dg = 1;
    int n = num;
    int c = 1;
    for (;;) {
      if (n < 10)
        break;
      n /= 10;
      c++;
    }
    return c * s * LETTER_WIDTH;
  }

  static float getHeight(float s) { return s * LETTER_HEIGHT; }

  static void drawLetter(int n) { displayList->call(n); }

private:
  static void drawLetter(int n, float x, float y, float s, float d) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(s, s, s);
    glRotatef(d, 0, 0, 1);
    displayList->call(n);
    glPopMatrix();
  }

  static void drawLetterRev(int n, float x, float y, float s, float d) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(s, -s, s);
    glRotatef(d, 0, 0, 1);
    displayList->call(n);
    glPopMatrix();
  }

  static int convertCharToInt(char c) {
    int idx;
    if (c >= '0' && c <= '9') {
      idx = c - '0';
    } else if (c >= 'A' && c <= 'Z') {
      idx = c - 'A' + 10;
    } else if (c >= 'a' && c <= 'z') {
      idx = c - 'a' + 10;
    } else if (c == '.') {
      idx = 36;
    } else if (c == '-') {
      idx = 38;
    } else if (c == '+') {
      idx = 39;
    } else if (c == '_') {
      idx = 37;
    } else if (c == '!') {
      idx = 42;
    } else if (c == '/') {
      idx = 43;
    } else {
      assert(0);
    }
    return idx;
  }

public:
  static void drawString(const std::string &str, float lx, float y, float s,
                         int d = Direction::TO_RIGHT, bool rev = false,
                         float od = 0, float r = 1, float g = 1, float b = 1);

  static void drawNum(int num, float lx, float y, float s, int dg = 0,
                      int headChar = -1, int floatDigit = -1);

  static void drawNumSign(int num, float lx, float ly, float s,
                          int headChar = -1, int floatDigit = -1, int type = 0);

  static void drawTime(int time, float lx, float y, float s);

private:
  static void setLetter(int idx, int type = Shape::NORMAL);
};

#endif // LETTER_H
