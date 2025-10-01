/*
 * $Id: title.d,v 1.4 2006/12/04 16:04:27 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include "title.hpp"

void Title::move() {
  PadState *input;
  input = pad->getState(false);
  if (input->button & PadState::Button::A) {
    if (!aPressed) {
      aPressed = true;
      frame->startInGame(cursorIdx);
    }
  } else {
    aPressed = false;
  }
  if (input->dir & (PadState::Dir::UP | PadState::Dir::DOWN)) {
    if (!udPressed) {
      udPressed = true;
      if (input->dir & PadState::Dir::UP)
        cursorIdx--;
      else if (input->dir & PadState::Dir::DOWN)
        cursorIdx++;
      if (cursorIdx < 0)
        cursorIdx = GameState::MODE_NUM - 1;
      else if (cursorIdx > GameState::MODE_NUM - 1)
        cursorIdx = 0;
    }
  } else {
    udPressed = false;
  }
  if (cnt > 180 && cnt < 235)
    titlePos.y -= 2;
  if (cnt > 600 && cnt < 675) {
    titlePos.x -= 2;
    titlePos.y++;
    titleSize -= 0.007f;
  }
  cnt++;
}

void Title::draw() {
  Screen3D::setColor(1, 1, 1);
  glEnable(GL_TEXTURE_2D);
  titleTexture->bind();
  drawBoard(titlePos.x, titlePos.y, 280 * titleSize, 64 * titleSize);
  glDisable(GL_TEXTURE_2D);
  if ((cnt % 120) < 60) {
    float x = 175, sz = 6;
    if (cnt >= 600) {
      int c = cnt - 600;
      if (c > 75)
        c = 75;
      x += c * 4.33f;
      sz -= c * 0.045f;
    }
    Letter::drawString("PUSH SHOT BUTTON TO START", x, 440, sz);
  }
  if (cnt >= 240) {
    drawRanking();
  }
  if ((cnt % 60) < 30) {
    drawTriangle(575, 398, 180);
    drawTriangle(575, 417, 0);
  }
  Letter::drawString(GameState::MODE_NAME[cursorIdx], 540, 400, 5);
}
