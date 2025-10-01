/*
 * $Id: field.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef FIELD_H
#define FIELD_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

#include "abagames/ttn/screen.hpp"
#include "abagames/util/math.hpp"
#include "abagames/util/sdl/frame.hpp"
#include "abagames/util/vector.hpp"

/**
 * Game field.
 */
class Field {
public:
  static constexpr float PIT_SIZE_Y_RATIO = 12.0f;

public:
  static constexpr float CIRCLE_RADIUS = 64.0f;
  static constexpr float EYE_POS_DIST_RATIO = 1.25f;
  static constexpr float X_EXPANSION_RATIO = 1.0f;
  static constexpr float SIDEWALL_WIDTH = 145;
  static constexpr float TORUS_Y = -24.0f;
  Frame *frame;
  ScreenImpl *screen;
  Vector _size;
  Vector _outerSize;
  Vector3 _eyePos;
  float eyeDeg;
  Vector3 circlePos;
  int cnt;

public:
  Field(Frame *frame, ScreenImpl *screen)
      : frame(frame), screen(screen), _size(12, 12), _outerSize(13, 13) {
    set();
  }

  void set() {
    _eyePos.x = _eyePos.y = _eyePos.z = 0;
    eyeDeg = 0;
    cnt = 0;
  }

  bool contains(Vector p) { return contains(p.x, p.y); }

  bool contains(float x, float y) { return _size.contains(x, y); }

  bool containsOuter(Vector p) { return containsOuter(p.x, p.y); }

  bool containsOuter(float x, float y) { return _outerSize.contains(x, y); }

  bool containsOuterY(float y) {
    return (y >= -_outerSize.y && y <= _outerSize.y);
  }

  bool containsIncludingPit(Vector p) {
    return (p.y >= -_outerSize.y && p.y <= _size.y * PIT_SIZE_Y_RATIO * 1.1f);
  }

  float normalizeX(float x) {
    float rx = x;
    float hd = CIRCLE_RADIUS * PI / X_EXPANSION_RATIO;
    if (rx < -hd)
      rx = hd * 2 - fmod(-rx, hd * 2);
    return fmod(rx + hd, hd * 2) - hd;
  }

  float calcCircularDist(const Vector &p1, const Vector &p2) {
    float ax = fabs(normalizeX(p1.x - p2.x));
    float ay = fabs(p1.y - p2.y);
    if (ax > ay)
      return ax + ay / 2;
    else
      return ay + ax / 2;
  }

  float circularDistance() {
    return CIRCLE_RADIUS * PI * 2 / X_EXPANSION_RATIO;
  }

  Vector3 calcCircularPos(const Vector &p) { return calcCircularPos(p.x, p.y); }

  Vector3 calcCircularPos(float x, float y) {
    float d = calcCircularDeg(x);
    if (y < _size.y) {
      circlePos.x = sin(d) * CIRCLE_RADIUS;
      circlePos.z = cos(d) * CIRCLE_RADIUS;
      circlePos.y = y;
    } else if (y < _size.y * 3) {
      float cd = (y - _size.y) * PI / 2 / (_size.y * 2);
      float cr = CIRCLE_RADIUS * (0.8f + 0.2f * cos(cd));
      circlePos.x = sin(d) * cr;
      circlePos.z = cos(d) * cr;
      circlePos.y = _size.y + sin(cd) * CIRCLE_RADIUS * 0.2f;
    } else if (y < _size.y * 7) {
      float cd = (y - _size.y * 3) * PI / 2 / (_size.y * 4);
      float cr = CIRCLE_RADIUS * (0.8f - 0.4f * sin(cd));
      circlePos.x = sin(d) * cr;
      circlePos.z = cos(d) * cr;
      circlePos.y =
          _size.y - CIRCLE_RADIUS * 0.2f + cos(cd) * CIRCLE_RADIUS * 0.4f;
    } else {
      float cr = CIRCLE_RADIUS * 0.4f;
      circlePos.x = sin(d) * cr;
      circlePos.z = cos(d) * cr;
      circlePos.y = _size.y - CIRCLE_RADIUS * 0.2f - (y - _size.y * 7);
    }
    return circlePos;
  }

  float calcCircularDeg(float x) {
    return x * X_EXPANSION_RATIO / CIRCLE_RADIUS;
  }

  float calcCircularDist(float d) {
    return d * CIRCLE_RADIUS / X_EXPANSION_RATIO;
  }

  bool checkHitDist(const Vector &pos, const Vector &p, const Vector &pp,
                    float dist) {
    float bmvx, bmvy, inaa;
    bmvx = pp.x;
    bmvy = pp.y;
    bmvx -= p.x;
    bmvy -= p.y;
    bmvx = normalizeX(bmvx);
    inaa = bmvx * bmvx + bmvy * bmvy;
    if (inaa > 0.00001) {
      float sofsx, sofsy, inab, hd;
      sofsx = pos.x;
      sofsy = pos.y;
      sofsx -= p.x;
      sofsy -= p.y;
      sofsx = normalizeX(sofsx);
      inab = bmvx * sofsx + bmvy * sofsy;
      if (inab >= 0 && inab <= inaa) {
        hd = sofsx * sofsx + sofsy * sofsy - inab * inab / inaa;
        if (hd >= 0 && hd <= dist)
          return true;
      }
    }
    return false;
  }

  void addSlowdownRatio(float sr) { frame->addSlowdownRatio(sr); }

  void setEyePos(const Vector &p) {
    eyeDeg = calcCircularDeg(p.x) * 0.25f;
    _eyePos.x = sin(eyeDeg) * CIRCLE_RADIUS * EYE_POS_DIST_RATIO;
    _eyePos.z = cos(eyeDeg) * CIRCLE_RADIUS * EYE_POS_DIST_RATIO;
  }

  void setLookAt() {
    glMatrixMode(GL_PROJECTION);
    screen->setPerspective();
    gluLookAt(_eyePos.x, _eyePos.y, _eyePos.z, 0, _eyePos.y, 0, 0, 1, 0);
    glMatrixMode(GL_MODELVIEW);
  }

  void resetLookAt() {
    glMatrixMode(GL_PROJECTION);
    screen->setPerspective();
    gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
    glMatrixMode(GL_MODELVIEW);
  }

  void beginDrawingFront() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 640, 480, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    drawSidewall();
  }

  void drawSidewall() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Screen3D::setColor(0.25f, 0.25f, 0.25f, 0.5f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    glVertex3f(SIDEWALL_WIDTH, 0, 0);
    glVertex3f(SIDEWALL_WIDTH, 480, 0);
    glVertex3f(0, 480, 0);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(640, 0, 0);
    glVertex3f(640 - SIDEWALL_WIDTH, 0, 0);
    glVertex3f(640 - SIDEWALL_WIDTH, 480, 0);
    glVertex3f(640, 480, 0);
    glEnd();
    Screen3D::setColor(1.0f, 1.0f, 1.0f, 0.8f);
    glBegin(GL_LINES);
    glVertex3f(SIDEWALL_WIDTH, 0, 0);
    glVertex3f(SIDEWALL_WIDTH, 480, 0);
    glVertex3f(640 - SIDEWALL_WIDTH, 0, 0);
    glVertex3f(640 - SIDEWALL_WIDTH, 480, 0);
    glEnd();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  }

  void move() { cnt++; }

  void drawBack() {
    glPushMatrix();
    glTranslatef(0, TORUS_Y, 0);
    drawTorusShape(PI / 2);
    glPopMatrix();
  }

  void drawFront() {
    glPushMatrix();
    glTranslatef(0, TORUS_Y, 0);
    drawTorusShape(-PI / 2);
    glPopMatrix();
  }

public:
  void drawTorusShape(float d1s);

  void createRingOffset(Vector3 &ringOfs, const Vector3 &centerPos, float rad,
                        float d1, float d2) {
    ringOfs.x = 0;
    ringOfs.y = 0;
    ringOfs.z = rad;
    ringOfs.rollX(d2);
    ringOfs.rollY(-d1);
    ringOfs += centerPos;
  }

  Vector3 eyePos() { return _eyePos; }

  Vector size() { return _size; }
};

#endif // FIELD_H
