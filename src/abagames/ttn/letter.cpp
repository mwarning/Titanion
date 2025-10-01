/*
 * $Id: letter.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include <cmath>

#include "abagames/ttn/screen.hpp"
#include "abagames/ttn/shape.hpp"
#include "abagames/util/sdl/displaylist.hpp"

#include "letter.hpp"

DisplayList *Letter::displayList = NULL;

static void drawSegmentPart(float width, float height) {
  glVertex3f(-width / 2, 0, 0);
  glVertex3f(-width / 3 * 1, -height / 2, 0);
  glVertex3f(width / 3 * 1, -height / 2, 0);
  glVertex3f(width / 2, 0, 0);
  glVertex3f(width / 3 * 1, height / 2, 0);
  glVertex3f(-width / 3 * 1, height / 2, 0);
}

static void drawSegment(float x, float y, float width, float height,
                        float deg) {
  glPushMatrix();
  glTranslatef(x - width / 2, y, 0);
  glRotatef(deg, 0, 0, 1);
  Screen3D::setColor(1, 1, 1, 0.5);
  glBegin(GL_TRIANGLE_FAN);
  drawSegmentPart(width, height);
  glEnd();
  Screen3D::setColor(1, 1, 1);
  glBegin(GL_LINE_LOOP);
  drawSegmentPart(width, height);
  glEnd();
  glPopMatrix();
}

static void drawSegmentPolygon(float x, float y, float width, float height,
                               float deg) {
  glPushMatrix();
  glTranslatef(x - width / 2, y, 0);
  glRotatef(deg, 0, 0, 1);
  glBegin(GL_TRIANGLE_FAN);
  drawSegmentPart(width, height);
  glEnd();
  glPopMatrix();
}

static void drawSegmentLine(float x, float y, float width, float height,
                            float deg) {
  glPushMatrix();
  glTranslatef(x - width / 2, y, 0);
  glRotatef(deg, 0, 0, 1);
  glBegin(GL_LINE_LOOP);
  drawSegmentPart(width, height);
  glEnd();
  glPopMatrix();
}

typedef const float Element[5];

Element glyph00[] = {
    // 0
    {0, 1.15f, 0.65f, 0.3f, 0},
    {-0.6f, 0.55f, 0.65f, 0.3f, 90},
    {0.6f, 0.55f, 0.65f, 0.3f, 90},
    {-0.6f, -0.55f, 0.65f, 0.3f, 90},
    {0.6f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0},
    {0, 0, 0, 0, 99999},
};
Element glyph01[] = {
    // 1
    {0.5f, 0.55f, 0.65f, 0.3f, 90},
    {0.5f, -0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph02[] = {
    // 2
    {0, 1.15f, 0.65f, 0.3f, 0},  {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0.65f, 0.3f, 0},      {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0}, {0, 0, 0, 0, 99999},
};
Element glyph03[] = {
    // 3
    {0, 1.15f, 0.65f, 0.3f, 0},  {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0.65f, 0.3f, 0},      {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0}, {0, 0, 0, 0, 99999},
};
Element glyph04[] = {
    // 4
    {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0.65f, 0.3f, 0},
    {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph05[] = {
    // 5
    {0, 1.15f, 0.65f, 0.3f, 0},  {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0.65f, 0.3f, 0},      {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0}, {0, 0, 0, 0, 99999},
};
Element glyph06[] = {
    // 6
    {0, 1.15f, 0.65f, 0.3f, 0},
    {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0.65f, 0.3f, 0},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0},
    {0, 0, 0, 0, 99999},
};
Element glyph07[] = {
    // 7
    {0, 1.15f, 0.65f, 0.3f, 0},
    {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph08[] = {
    // 8
    {0, 1.15f, 0.65f, 0.3f, 0},        {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.65f, 0.55f, 0.65f, 0.3f, 90},   {0, 0, 0.65f, 0.3f, 0},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90}, {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0},       {0, 0, 0, 0, 99999},
};
Element glyph09[] = {
    // 9
    {0, 1.15f, 0.65f, 0.3f, 0},
    {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0.65f, 0.3f, 0},
    {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0},
    {0, 0, 0, 0, 99999},
};
Element glyph10[] = {
    // A
    {0, 1.15f, 0.65f, 0.3f, 0},
    {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0.65f, 0.3f, 0},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph11[] = {
    // B
    {-0.18f, 1.15f, 0.45f, 0.3f, 0},   {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.45f, 0.55f, 0.65f, 0.3f, 90},   {-0.18f, 0, 0.45f, 0.3f, 0},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90}, {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0},       {0, 0, 0, 0, 99999},
};
Element glyph12[] = {
    // C
    {0, 1.15f, 0.65f, 0.3f, 0},
    {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0},
    {0, 0, 0, 0, 99999},
};
Element glyph13[] = {
    // D
    {-0.15f, 1.15f, 0.45f, 0.3f, 0},
    {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.45f, 0.45f, 0.65f, 0.3f, 90},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0},
    {0, 0, 0, 0, 99999},
};
Element glyph14[] = {
    // E
    {0, 1.15f, 0.65f, 0.3f, 0},  {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0.65f, 0.3f, 0},      {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0}, {0, 0, 0, 0, 99999},
};
Element glyph15[] = {
    // F
    {0, 1.15f, 0.65f, 0.3f, 0}, {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0.65f, 0.3f, 0},     {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph16[] = {
    // G
    {0, 1.15f, 0.65f, 0.3f, 0},
    {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.05f, 0, 0.3f, 0.3f, 0},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0},
    {0, 0, 0, 0, 99999},
};
Element glyph17[] = {
    // H
    {-0.65f, 0.55f, 0.65f, 0.3f, 90}, {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0.65f, 0.3f, 0},           {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0.65f, -0.55f, 0.65f, 0.3f, 90}, {0, 0, 0, 0, 99999},
};
Element glyph18[] = {
    // I
    {0, 0.55f, 0.65f, 0.3f, 90},
    {0, -0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph19[] = {
    // J
    {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {-0.7f, -0.7f, 0.3f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0},
    {0, 0, 0, 0, 99999},
};
Element glyph20[] = {
    // K
    {-0.65f, 0.55f, 0.65f, 0.3f, 90}, {0.4f, 0.55f, 0.65f, 0.3f, 100},
    {-0.25f, 0, 0.45f, 0.3f, 0},      {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0.6f, -0.55f, 0.65f, 0.3f, 80},  {0, 0, 0, 0, 99999},
};
Element glyph21[] = {
    // L
    {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0},
    {0, 0, 0, 0, 99999},
};
Element glyph22[] = {
    // M
    {-0.5f, 1.15f, 0.3f, 0.3f, 0},
    {0.1f, 1.15f, 0.3f, 0.3f, 0},
    {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, 0.55f, 0.65f, 0.3f, 90},
    {0, -0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph23[] = {
    // N
    {0, 1.15f, 0.65f, 0.3f, 0},       {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.65f, 0.55f, 0.65f, 0.3f, 90},  {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0.65f, -0.55f, 0.65f, 0.3f, 90}, {0, 0, 0, 0, 99999},
};
Element glyph24[] = {
    // O
    {0, 1.15f, 0.65f, 0.3f, 0},
    {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0},
    {0, 0, 0, 0, 99999},
};
Element glyph25[] = {
    // P
    {0, 1.15f, 0.65f, 0.3f, 0},        {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.65f, 0.55f, 0.65f, 0.3f, 90},   {0, 0, 0.65f, 0.3f, 0},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90}, {0, 0, 0, 0, 99999},
};
Element glyph26[] = {
    // Q
    {0, 1.15f, 0.65f, 0.3f, 0},       {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.65f, 0.55f, 0.65f, 0.3f, 90},  {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0.65f, -0.55f, 0.65f, 0.3f, 90}, {0, -1.15f, 0.65f, 0.3f, 0},
    {0.05f, -0.55f, 0.45f, 0.3f, 60}, {0, 0, 0, 0, 99999},
};
Element glyph27[] = {
    // R
    {0, 1.15f, 0.65f, 0.3f, 0},
    {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {-0.2f, 0, 0.45f, 0.3f, 0},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0.45f, -0.55f, 0.65f, 0.3f, 80},
    {0, 0, 0, 0, 99999},
};
Element glyph28[] = {
    // S
    {0, 1.15f, 0.65f, 0.3f, 0},  {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0.65f, 0.3f, 0},      {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0}, {0, 0, 0, 0, 99999},
};
Element glyph29[] = {
    // T
    {-0.5f, 1.15f, 0.55f, 0.3f, 0},
    {0.5f, 1.15f, 0.55f, 0.3f, 0},
    {0.1f, 0.55f, 0.65f, 0.3f, 90},
    {0.1f, -0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph30[] = {
    // U
    {-0.65f, 0.55f, 0.65f, 0.3f, 90},  {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90}, {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0, -1.15f, 0.65f, 0.3f, 0},       {0, 0, 0, 0, 99999},
};
Element glyph31[] = {
    // V
    {-0.65f, 0.55f, 0.65f, 0.3f, 90}, {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {-0.5f, -0.55f, 0.65f, 0.3f, 90}, {0.5f, -0.55f, 0.65f, 0.3f, 90},
    {-0.1f, -1.15f, 0.45f, 0.3f, 0},  {0, 0, 0, 0, 99999},
};
Element glyph32[] = {
    // W
    {-0.65f, 0.55f, 0.65f, 0.3f, 90},
    {0.65f, 0.55f, 0.65f, 0.3f, 90},
    {-0.65f, -0.55f, 0.65f, 0.3f, 90},
    {0.65f, -0.55f, 0.65f, 0.3f, 90},
    {-0.5f, -1.15f, 0.3f, 0.3f, 0},
    {0.1f, -1.15f, 0.3f, 0.3f, 0},
    {0, 0.55f, 0.65f, 0.3f, 90},
    {0, -0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph33[] = {
    // X
    {-0.4f, 0.6f, 0.85f, 0.3f, 360 - 120},
    {0.4f, 0.6f, 0.85f, 0.3f, 360 - 60},
    {-0.4f, -0.6f, 0.85f, 0.3f, 360 - 240},
    {0.4f, -0.6f, 0.85f, 0.3f, 360 - 300},
    {0, 0, 0, 0, 99999},
};
Element glyph34[] = {
    // Y
    {-0.4f, 0.6f, 0.85f, 0.3f, 360 - 120},
    {0.4f, 0.6f, 0.85f, 0.3f, 360 - 60},
    {-0.1f, -0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph35[] = {
    // Z
    {0, 1.15f, 0.65f, 0.3f, 0},
    {0.3f, 0.4f, 0.65f, 0.3f, 120},
    {-0.3f, -0.4f, 0.65f, 0.3f, 120},
    {0, -1.15f, 0.65f, 0.3f, 0},
    {0, 0, 0, 0, 99999},
};
Element glyph36[] = {
    // .
    {0, -1.15f, 0.3f, 0.3f, 0},
    {0, 0, 0, 0, 99999},
};
Element glyph37[] = {
    // _
    {0, -1.15f, 0.8f, 0.3f, 0},
    {0, 0, 0, 0, 99999},
};
Element glyph38[] = {
    // -
    {0, 0, 0.9f, 0.3f, 0},
    {0, 0, 0, 0, 99999},
};
Element glyph39[] = {
    // +
    {-0.5f, 0, 0.45f, 0.3f, 0},
    {0.45f, 0, 0.45f, 0.3f, 0},
    {0.1f, 0.55f, 0.65f, 0.3f, 90},
    {0.1f, -0.55f, 0.65f, 0.3f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph40[] = {
    // '
    {0, 1.0f, 0.4f, 0.2f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph41[] = {
    // ''
    {-0.19f, 1.0f, 0.4f, 0.2f, 90},
    {0.2f, 1.0f, 0.4f, 0.2f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph42[] = {
    // !
    {0.56f, 0.25f, 1.1f, 0.3f, 90},
    {0, -1.0f, 0.3f, 0.3f, 90},
    {0, 0, 0, 0, 99999},
};
Element glyph43[] = {
    // /
    {0.8f, 0, 1.75f, 0.3f, 120},
    {0, 0, 0, 0, 99999},
};

Element *spData[] = {
    (Element *)&glyph00, (Element *)&glyph01, (Element *)&glyph02,
    (Element *)&glyph03, (Element *)&glyph04, (Element *)&glyph05,
    (Element *)&glyph06, (Element *)&glyph07, (Element *)&glyph08,
    (Element *)&glyph09, (Element *)&glyph10, (Element *)&glyph11,
    (Element *)&glyph12, (Element *)&glyph13, (Element *)&glyph14,
    (Element *)&glyph15, (Element *)&glyph16, (Element *)&glyph17,
    (Element *)&glyph18, (Element *)&glyph19, (Element *)&glyph20,
    (Element *)&glyph21, (Element *)&glyph22, (Element *)&glyph23,
    (Element *)&glyph24, (Element *)&glyph25, (Element *)&glyph26,
    (Element *)&glyph27, (Element *)&glyph28, (Element *)&glyph29,
    (Element *)&glyph30, (Element *)&glyph31, (Element *)&glyph32,
    (Element *)&glyph33, (Element *)&glyph34, (Element *)&glyph35,
    (Element *)&glyph36, (Element *)&glyph37, (Element *)&glyph38,
    (Element *)&glyph39, (Element *)&glyph40, (Element *)&glyph41,
    (Element *)&glyph42, (Element *)&glyph43};

void Letter::setLetter(int idx, int type) {
  float x, y, length, size; //, t;
  float deg;
  for (int i = 0;; i++) {
    deg = (int)spData[idx][i][4];
    if (deg > 99990)
      break;
    x = -spData[idx][i][0];
    y = -spData[idx][i][1];
    size = spData[idx][i][2];
    length = spData[idx][i][3];
    y *= 0.9;
    size *= 1.4;
    length *= 1.05;
    x = -x;
    y = y;
    deg = fmod(deg, 180);
    switch (type) {
    case Shape::NORMAL:
      drawSegment(x, y, size, length, deg);
      break;
    case Shape::POLYGON:
      drawSegmentPolygon(x, y, size, length, deg);
      break;
    case Shape::LINE:
      drawSegmentLine(x, y, size, length, deg);
      break;
    }
  }
}

void Letter::drawString(const std::string &str, float lx, float y, float s,
                        int d, bool rev, float od, float r, float g, float b) {
  lx += LETTER_WIDTH * s / 2;
  y += LETTER_HEIGHT * s / 2;
  float x = lx;
  int idx;
  float ld;
  switch (d) {
  case Direction::TO_RIGHT:
    ld = 0;
    break;
  case Direction::TO_DOWN:
    ld = 90;
    break;
  case Direction::TO_LEFT:
    ld = 180;
    break;
  case Direction::TO_UP:
    ld = 270;
    break;
  }
  ld += od;
  for (size_t i = 0; i < str.length(); ++i) {
    char c = str[i];
    if (c != ' ') {
      idx = convertCharToInt(c);
      if (r == 1 && g == 1 && b == 1) {
        if (rev)
          drawLetterRev(idx, x, y, s, ld);
        else
          drawLetter(idx, x, y, s, ld);
      } else {
        Screen3D::setColor(r, g, b, 0.5f);
        if (rev)
          drawLetterRev(idx + LETTER_NUM, x, y, s, ld);
        else
          drawLetter(idx + LETTER_NUM, x, y, s, ld);
        Screen3D::setColor(r, g, b);
        if (rev)
          drawLetterRev(idx + LETTER_NUM * 2, x, y, s, ld);
        else
          drawLetter(idx + LETTER_NUM * 2, x, y, s, ld);
      }
    }
    if (od == 0) {
      switch (d) {
      case Direction::TO_RIGHT:
        x += s * LETTER_WIDTH;
        break;
      case Direction::TO_DOWN:
        y += s * LETTER_WIDTH;
        break;
      case Direction::TO_LEFT:
        x -= s * LETTER_WIDTH;
        break;
      case Direction::TO_UP:
        y -= s * LETTER_WIDTH;
        break;
      }
    } else {
      x += cos(ld * PI / 180) * s * LETTER_WIDTH;
      y += sin(ld * PI / 180) * s * LETTER_WIDTH;
    }
  }
}

void Letter::drawNum(int num, float lx, float y, float s, int dg, int headChar,
                     int floatDigit) {
  lx += LETTER_WIDTH * s / 2;
  y += LETTER_HEIGHT * s / 2;
  int n = num;
  float x = lx;
  float ld = 0;
  int digit = dg;
  int fd = floatDigit;
  for (;;) {
    if (fd <= 0) {
      drawLetter(n % 10, x, y, s, ld);
      x -= s * LETTER_WIDTH;
    } else {
      drawLetter(n % 10, x, y + s * LETTER_WIDTH * 0.25f, s * 0.5f, ld);
      x -= s * LETTER_WIDTH * 0.5f;
    }
    n /= 10;
    digit--;
    fd--;
    if (n <= 0 && digit <= 0 && fd < 0)
      break;
    if (fd == 0) {
      drawLetter(36, x, y + s * LETTER_WIDTH * 0.25f, s * 0.5f, ld);
      x -= s * LETTER_WIDTH * 0.5f;
    }
  }
  if (headChar >= 0)
    drawLetter(headChar, x + s * LETTER_WIDTH * 0.2f,
               y + s * LETTER_WIDTH * 0.2f, s * 0.6f, ld);
}

void Letter::drawNumSign(int num, float lx, float ly, float s, int headChar,
                         int floatDigit, int type) {
  float x = lx;
  float y = ly;
  int n = num;
  int fd = floatDigit;
  for (;;) {
    if (fd <= 0) {
      drawLetterRev(n % 10 + type * LETTER_NUM, x, y, s, 0);
      x -= s * LETTER_WIDTH;
    } else {
      drawLetterRev(n % 10 + type * LETTER_NUM, x, y - s * LETTER_WIDTH * 0.25f,
                    s * 0.5f, 0);
      x -= s * LETTER_WIDTH * 0.5f;
    }
    n /= 10;
    if (n <= 0)
      break;
    fd--;
    if (fd == 0) {
      drawLetterRev(36 + type * LETTER_NUM, x, y - s * LETTER_WIDTH * 0.25f,
                    s * 0.5f, 0);
      x -= s * LETTER_WIDTH * 0.5f;
    }
  }
  if (headChar >= 0)
    drawLetterRev(headChar + type * LETTER_NUM, x + s * LETTER_WIDTH * 0.2f,
                  y - s * LETTER_WIDTH * 0.2f, s * 0.6f, 0);
}

void Letter::drawTime(int time, float lx, float y, float s) {
  int n = time;
  if (n < 0)
    n = 0;
  float x = lx;
  for (int i = 0; i < 7; i++) {
    if (i != 4) {
      drawLetter(n % 10, x, y, s, Direction::TO_RIGHT);
      n /= 10;
    } else {
      drawLetter(n % 6, x, y, s, Direction::TO_RIGHT);
      n /= 6;
    }
    if ((i & 1) == 1 || i == 0) {
      switch (i) {
      case 3:
        drawLetter(41, x + s * 1.16f, y, s, Direction::TO_RIGHT);
        break;
      case 5:
        drawLetter(40, x + s * 1.16f, y, s, Direction::TO_RIGHT);
        break;
      default:
        break;
      }
      x -= s * LETTER_WIDTH;
    } else {
      x -= s * LETTER_WIDTH * 1.3f;
    }
    if (n <= 0)
      break;
  }
}
