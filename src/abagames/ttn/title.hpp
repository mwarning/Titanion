/*
 * $Id: title.d,v 1.4 2006/12/04 16:04:27 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef TITLE_H
#define TITLE_H

#include <string>

#include "abagames/ttn/frame.hpp"
#include "abagames/ttn/letter.hpp"
#include "abagames/ttn/preference.hpp"
#include "abagames/ttn/screen.hpp"
#include "abagames/util/sdl/pad.hpp"
#include "abagames/util/sdl/texture.hpp"
#include "abagames/util/vector.hpp"

class FrameImpl;

/**
 * Title screen.
 */
class Title {
private:
  PreferenceImpl *preference;
  RecordablePad *pad;
  FrameImpl *frame;
  int cnt;
  bool aPressed, udPressed;
  Texture *titleTexture;
  Vector titlePos;
  float titleSize;
  int cursorIdx;

public:
  Title(PreferenceImpl *preference, Pad *pad, FrameImpl *frame) {
    this->preference = preference;
    this->pad = dynamic_cast<RecordablePad *>(pad);
    this->frame = frame;
    cursorIdx = 0;
  }

  void init() { titleTexture = new Texture("title.bmp"); }

  void close() { titleTexture->close(); }

  void setMode(int mode) { cursorIdx = mode; }

  void start() {
    cnt = 0;
    aPressed = true;
    udPressed = true;
    titlePos.x = 150;
    titlePos.y = 150;
    titleSize = 1.0f;
  }

  void move();

  void draw();

private:
  void drawBoard(float x, float y, float w, float h) {
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0, 0);
    glVertex3f(x, y, 0);
    glTexCoord2f(1, 0);
    glVertex3f(x + w, y, 0);
    glTexCoord2f(1, 1);
    glVertex3f(x + w, y + h, 0);
    glTexCoord2f(0, 1);
    glVertex3f(x, y + h, 0);
    glEnd();
  }

  void drawTriangle(float x, float y, float d) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glRotatef(d, 0, 0, 1);
    glScalef(5, 5, 1);
    glBegin(GL_TRIANGLE_FAN);
    Screen3D::setColor(1, 1, 1, 0.5f);
    glVertex3f(0, 1.7f, 0);
    glVertex3f(1, 0, 0);
    glVertex3f(-1, 0, 0);
    glEnd();
    glBegin(GL_LINE_LOOP);
    Screen3D::setColor(1, 1, 1, 1);
    glVertex3f(0, 1.7f, 0);
    glVertex3f(1, 0, 0);
    glVertex3f(-1, 0, 0);
    glEnd();
    glPopMatrix();
  }

  void drawRanking() {
    int rn = (cnt - 240) / 30;
    if (rn > PreferenceImpl::RANKING_NUM)
      rn = PreferenceImpl::RANKING_NUM;
    float y = 140;
    for (int i = 0; i < rn; i++) {
      if (cnt < 600) {
        std::string rstr;
        switch (i) {
        case 0:
          rstr = "1ST";
          break;
        case 1:
          rstr = "2ND";
          break;
        case 2:
          rstr = "3RD";
          break;
        default:
          rstr = std::to_string(i + 1) + "TH";
          break;
        }
        if (i < 9)
          Letter::drawString(rstr, 180, y, 7);
        else
          Letter::drawString(rstr, 166, y, 7);
      }
      float sx = 450, sy = y, sz = 6;
      if (cnt >= 600) {
        int c = cnt - 600;
        if (c > 75)
          c = 75;
        sx += (int)(c * 2.35f);
        sz -= c * 0.03f;
      }
      Letter::drawNum(preference->getHighScore(cursorIdx, i), sx, sy, sz);
      y += 24;
    }
  }
};

#endif // TITLE_H
