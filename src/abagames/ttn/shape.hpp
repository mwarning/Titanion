/*
 * $Id: shape.d,v 1.5 2006/12/04 16:04:27 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef SHAPE_H
#define SHAPE_H

#include "abagames/ttn/field.hpp"
#include "abagames/ttn/screen.hpp"
#include "abagames/util/sdl/displaylist.hpp"
#include "abagames/util/vector.hpp"
#include <cmath>

/**
 * 3D shapes of a player, enemies, particles, etc.
 */
class Shape {
public:
  virtual ~Shape() {}

  virtual void draw3(Vector3 pos, float cd, float deg) = 0;

  virtual void DisplayListShape_init() = 0;
};

class DisplayListShape : public Shape {
protected:
  DisplayList *displayList;

public:
  virtual ~DisplayListShape() {}

  void DisplayListShape_init() {
    displayList = new DisplayList(1);
    displayList->beginNewList();
    drawList();
    displayList->endNewList();
  }

  virtual void drawList() = 0;

public:
  void draw() { drawList(); }

  void draw3(Vector3 pos, float cd, float deg) {
    glPushMatrix();
    Screen3D::glTranslate(pos);
    glRotatef(cd * 180 / PI, 0, 1, 0);
    Screen3D::glRotate(deg);
    displayList->call();
    glPopMatrix();
  }

  void close() { displayList->close(); }
};

class PyramidShape {
public:
  static void draw() {
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, -1);
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, 1);
    glEnd();
    Screen3D::setColor(0.1f, 0.1f, 0.1f, 0.5f);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, -1);
    glVertex3f(0, 0, 0);
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1, 1);
    glVertex3f(0, 0, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, -1);
    glVertex3f(-1, 1, -1);
    glEnd();
  }

  static void drawShadow(float r, float g, float b, bool noAlpha = false) {
    glBegin(GL_TRIANGLE_FAN);
    Screen3D::setColor(r, g, b);
    glVertex3f(0, 0, 0);
    if (!noAlpha)
      Screen3D::setColor(r * 0.75f, g * 0.75f, b * 0.75f, 0.33f);
    else
      Screen3D::setColor(r * 0.75f, g * 0.75f, b * 0.75f, 0.75f);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, -1);
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, 1);
    glEnd();
  }

  static void drawPolygonShape() {
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, -1);
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, 1);
    glEnd();
  }

  static void drawLineShape() {
    glBegin(GL_LINE_STRIP);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, -1);
    glVertex3f(0, 0, 0);
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1, 1);
    glVertex3f(0, 0, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, -1);
    glVertex3f(-1, 1, -1);
    glEnd();
  }
};

class PlayerShape : public DisplayListShape {
public:
  PlayerShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(0, -0.6f, 0);
    glScalef(0.4f, 1.3f, 0.4f);
    PyramidShape::drawShadow(1, 0.5f, 0.5f, true);
    glPopMatrix();
    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(0.5f, -0.2f, 0);
    glScalef(0.3f, 0.9f, 0.3f);
    PyramidShape::drawShadow(1, 1, 1, true);
    glPopMatrix();
    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(-0.5f, -0.2f, 0);
    glScalef(0.3f, 0.9f, 0.3f);
    PyramidShape::drawShadow(1, 1, 1, true);
    glPopMatrix();
    Screen3D::setColor(1, 0.5f, 0.5f);
    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(0, -0.6f, 0);
    glScalef(0.3f, 1.2f, 0.3f);
    PyramidShape::drawPolygonShape();
    glPopMatrix();
    Screen3D::setColor(1, 1, 1);
    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(0.5f, -0.2f, 0);
    glScalef(0.2f, 0.8f, 0.2f);
    PyramidShape::drawPolygonShape();
    glPopMatrix();
    Screen3D::setColor(1, 1, 1);
    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(-0.5f, -0.2f, 0);
    glScalef(0.2f, 0.8f, 0.2f);
    PyramidShape::drawPolygonShape();
    glPopMatrix();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  }
};

class PlayerLineShape : public DisplayListShape {
public:
  PlayerLineShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(0, -0.6f, 0);
    glScalef(0.3f, 1.2f, 0.3f);
    PyramidShape::drawLineShape();
    glPopMatrix();
    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(0.5f, -0.2f, 0);
    glScalef(0.2f, 0.8f, 0.2f);
    PyramidShape::drawLineShape();
    glPopMatrix();
    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(-0.5f, -0.2f, 0);
    glScalef(0.2f, 0.8f, 0.2f);
    PyramidShape::drawLineShape();
    glPopMatrix();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  }
};

class ShotShape : public DisplayListShape {
public:
  ShotShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(0.5f, -0.5f, 0);
    glScalef(0.1f, 1.0f, 0.1f);
    Screen3D::setColor(0.4f, 0.2f, 0.8f);
    PyramidShape::drawLineShape();
    glPopMatrix();
    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(-0.5f, -0.5f, 0);
    glScalef(0.1f, 1.0f, 0.1f);
    Screen3D::setColor(0.4f, 0.2f, 0.8f);
    PyramidShape::drawLineShape();
    glPopMatrix();
  }
};

class TractorBeamShape : public DisplayListShape {
public:
  virtual ~TractorBeamShape() {}

protected:
  void drawTractorBeam(float r, float g, float b) {
    Screen3D::setColor(r, g, b, 0.5f);
    glBegin(GL_QUADS);
    glVertex3f(-1, 0, -1);
    glVertex3f(1, 0, -1);
    glVertex3f(1, 0, 1);
    glVertex3f(-1, 0, 1);
    glEnd();
    Screen3D::setColor(r, g, b);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-1, 0, -1);
    glVertex3f(1, 0, -1);
    glVertex3f(1, 0, 1);
    glVertex3f(-1, 0, 1);
    glEnd();
  }

  void drawTractorBeamLine(float r, float g, float b) {
    Screen3D::setColor(r, g, b);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-1, 0, -1);
    glVertex3f(1, 0, -1);
    glVertex3f(1, 0, 1);
    glVertex3f(-1, 0, 1);
    glEnd();
  }
};

class TractorBeamShapeRed : public TractorBeamShape {
public:
  TractorBeamShapeRed() { DisplayListShape_init(); }

protected:
  void drawList() override { drawTractorBeam(0.5f, 0.2f, 0.2f); }
};

class TractorBeamShapeBlue : public TractorBeamShape {
public:
  TractorBeamShapeBlue() { DisplayListShape_init(); }

protected:
  void drawList() override { drawTractorBeam(0.2f, 0.2f, 0.5f); }
};

class TractorBeamShapePurple : public TractorBeamShape {
public:
  TractorBeamShapePurple() { DisplayListShape_init(); }

protected:
  void drawList() override { drawTractorBeam(0.5f, 0.2f, 0.5f); }
};

class TractorBeamShapeDarkRed : public TractorBeamShape {
public:
  TractorBeamShapeDarkRed() { DisplayListShape_init(); }

protected:
  void drawList() override { drawTractorBeamLine(0.4f, 0.1f, 0.1f); }
};

class TractorBeamShapeDarkBlue : public TractorBeamShape {
public:
  TractorBeamShapeDarkBlue() { DisplayListShape_init(); }

protected:
  void drawList() override { drawTractorBeamLine(0.1f, 0.1f, 0.4f); }
};

class TractorBeamShapeDarkPurple : public TractorBeamShape {
public:
  TractorBeamShapeDarkPurple() { DisplayListShape_init(); }

protected:
  void drawList() override { drawTractorBeamLine(0.4f, 0.1f, 0.4f); }
};

class BulletShapeBase : public DisplayListShape {
public:
  virtual ~BulletShapeBase() {}

  void draw(Vector3 pos, float cd, float deg, float rd) {
    glPushMatrix();
    Screen3D::glTranslate(pos);
    glRotatef(cd * 180 / PI, 0, 1, 0);
    Screen3D::glRotate(deg);
    glRotatef(rd, 0, 1, 0);
    displayList->call();
    glPopMatrix();
  }
};

class BulletShape : public BulletShapeBase {
public:
  BulletShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Screen3D::setColor(0, 0, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0.5f, 0);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0, -0.3f, 0.4f);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0, -0.3f, 0.4f);
    glEnd();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glScalef(1.2f, 1.2f, 1.2f);
    Screen3D::setColor(0.1f, 0.3f, 0.3f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0.5f, 0);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0, -0.3f, 0.4f);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0, -0.3f, 0.4f);
    glEnd();
  }
};

class BulletLineShape : public BulletShapeBase {
public:
  BulletLineShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glScalef(1.2f, 1.2f, 1.2f);
    glBegin(GL_LINES);
    glVertex3f(0, 0.5f, 0);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(0, 0.5f, 0);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0, 0.5f, 0);
    glVertex3f(0, -0.3f, 0.4f);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0, -0.3f, 0.4f);
    glEnd();
  }
};

class MiddleBulletShape : public BulletShapeBase {
public:
  MiddleBulletShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glScalef(1.1f, 1.0f, 1.1f);
    Screen3D::setColor(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex3f(-0.17f, 0.3f, -0.1f);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0.17f, 0.3f, -0.1f);
    glVertex3f(0.17f, 0.3f, -0.1f);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0, -0.3f, 0.4f);
    glVertex3f(0, 0.3f, 0.2f);
    glVertex3f(0, 0.3f, 0.2f);
    glVertex3f(0, -0.3f, 0.4f);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(-0.17f, 0.3f, -0.1f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.17f, -0.3f, -0.1f);
    glVertex3f(0.17f, -0.3f, -0.1f);
    glVertex3f(0, -0.3f, 0.2f);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0, -0.3f, 0.4f);
    glEnd();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glScalef(1.4f, 1.3f, 1.4f);
    Screen3D::setColor(0.1f, 0.2f, 0.3f);
    glBegin(GL_QUADS);
    glVertex3f(-0.17f, 0.3f, -0.1f);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0.17f, 0.3f, -0.1f);
    glVertex3f(0.17f, 0.3f, -0.1f);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0, -0.3f, 0.4f);
    glVertex3f(0, 0.3f, 0.2f);
    glVertex3f(0, 0.3f, 0.2f);
    glVertex3f(0, -0.3f, 0.4f);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(-0.17f, 0.3f, -0.1f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.17f, 0.3f, -0.1f);
    glVertex3f(0.17f, 0.3f, -0.1f);
    glVertex3f(0, 0.3f, 0.2f);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0, -0.3f, 0.4f);
    glEnd();
  }
};

class MiddleBulletLineShape : public BulletShapeBase {
public:
  MiddleBulletLineShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glScalef(1.4f, 1.3f, 1.4f);
    glBegin(GL_LINES);
    glVertex3f(-0.17f, 0.3f, -0.1f);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(0.17f, 0.3f, -0.1f);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0, 0.3f, 0.2f);
    glVertex3f(0, -0.3f, 0.4f);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.17f, 0.3f, -0.1f);
    glVertex3f(0.17f, 0.3f, -0.1f);
    glVertex3f(0, 0.3f, 0.2f);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.34f, -0.3f, -0.2f);
    glVertex3f(0.34f, -0.3f, -0.2f);
    glVertex3f(0, -0.3f, 0.4f);
    glEnd();
  }
};

class RollBulletShapeBase : public BulletShapeBase {
public:
  virtual ~RollBulletShapeBase() {}

  void draw(Vector3 pos, float cd, float deg, float rd) {
    glPushMatrix();
    Screen3D::glTranslate(pos);
    glRotatef(cd * 180 / PI, 0, 1, 0);
    glRotatef(rd, 0, 0, 1);
    displayList->call();
    glPopMatrix();
  }
};

class CounterBulletShape : public RollBulletShapeBase {
public:
  CounterBulletShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Screen3D::setColor(0, 0, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0.5f);
    glVertex3f(0.5f, 0, 0);
    glVertex3f(0, 0.5f, 0);
    glVertex3f(-0.5f, 0, 0);
    glVertex3f(0, -0.5f, 0);
    glVertex3f(0.5f, 0, 0);
    glEnd();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glScalef(1.2f, 1.2f, 1.2f);
    Screen3D::setColor(0.5f, 0.5f, 0.5f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0.5f);
    glVertex3f(0.5f, 0, 0);
    glVertex3f(0, 0.5f, 0);
    glVertex3f(-0.5f, 0, 0);
    glVertex3f(0, -0.5f, 0);
    glVertex3f(0.5f, 0, 0);
    glEnd();
  }
};

class CounterBulletLineShape : public RollBulletShapeBase {
public:
  CounterBulletLineShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glScalef(1.2f, 1.2f, 1.2f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.5f, 0, 0);
    glVertex3f(0, 0.5f, 0);
    glVertex3f(-0.5f, 0, 0);
    glVertex3f(0, -0.5f, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0.5f);
    glVertex3f(0.5f, 0, 0);
    glVertex3f(0, 0, 0.5f);
    glVertex3f(0, 0.5f, 0);
    glVertex3f(0, 0, 0.5f);
    glVertex3f(-0.5f, 0, 0);
    glVertex3f(0, 0, 0.5f);
    glVertex3f(0, -0.5f, 0);
    glEnd();
  }
};

class EnemyShape : public DisplayListShape {
public:
  virtual ~EnemyShape() {}

  void draw(Vector3 pos, float cd, float deg, float cnt, Vector size) {
    draw(pos, cd, deg, cnt, size.x, size.y);
  }

  void draw(Vector3 pos, float cd, float deg, float cnt, float sx, float sy) {
    glPushMatrix();
    Screen3D::glTranslate(pos);
    glRotatef(cd * 180 / PI, 0, 1, 0);
    Screen3D::glRotate(deg);
    glScalef(sx, sy, 1);
    glRotatef(cnt * 3.0f, 0, 1, 0);
    displayList->call();
    glPopMatrix();
  }
};

class Enemy1Shape : public EnemyShape {
public:
  Enemy1Shape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(0, -0.6f, 0);
    glScalef(0.5f, 1.4f, 0.5f);
    PyramidShape::drawShadow(0.5f, 0.5f, 0.3f);
    glPopMatrix();
    glPushMatrix();
    glRotatef(120, 0, 0, 1);
    glTranslatef(0.5f, -0.2f, 0);
    glScalef(0.4f, 1.0f, 0.4f);
    PyramidShape::drawShadow(0.2f, 0.2f, 0.5f);
    glPopMatrix();
    Screen3D::setColor(0.2f, 0.2f, 0.5f);
    glPushMatrix();
    glRotatef(240, 0, 0, 1);
    glTranslatef(-0.5f, -0.2f, 0);
    glScalef(0.4f, 1.0f, 0.4f);
    PyramidShape::drawShadow(0.2f, 0.2f, 0.5f);
    glPopMatrix();
    Screen3D::setColor(1, 1, 0.6f);
    glPushMatrix();
    glTranslatef(0, -0.6f, 0);
    glScalef(0.3f, 1.2f, 0.3f);
    PyramidShape::draw();
    glPopMatrix();
    Screen3D::setColor(0.5f, 0.5f, 1);
    glPushMatrix();
    glRotatef(120, 0, 0, 1);
    glTranslatef(0.5f, -0.2f, 0);
    glScalef(0.2f, 0.8f, 0.2f);
    PyramidShape::draw();
    glPopMatrix();
    Screen3D::setColor(0.5f, 0.5f, 1);
    glPushMatrix();
    glRotatef(240, 0, 0, 1);
    glTranslatef(-0.5f, -0.2f, 0);
    glScalef(0.2f, 0.8f, 0.2f);
    PyramidShape::draw();
    glPopMatrix();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  }
};

class Enemy1TrailShape : public EnemyShape {
public:
  Enemy1TrailShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glPushMatrix();
    glTranslatef(0, -0.6f, 0);
    glScalef(0.3f, 1.2f, 0.3f);
    PyramidShape::drawLineShape();
    glPopMatrix();
    glPushMatrix();
    glRotatef(120, 0, 0, 1);
    glTranslatef(0.5f, -0.2f, 0);
    glScalef(0.2f, 0.8f, 0.2f);
    PyramidShape::drawLineShape();
    glPopMatrix();
    glPushMatrix();
    glRotatef(240, 0, 0, 1);
    glTranslatef(-0.5f, -0.2f, 0);
    glScalef(0.2f, 0.8f, 0.2f);
    PyramidShape::drawLineShape();
    glPopMatrix();
  }
};

class Enemy2Shape : public EnemyShape {
public:
  Enemy2Shape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(0, -0.5f, 0);
    glScalef(0.5f, 1.2f, 0.5f);
    PyramidShape::drawShadow(0.5f, 0.4f, 0.5f);
    glPopMatrix();
    glPushMatrix();
    glRotatef(60, 0, 0, 1);
    glTranslatef(0.6f, -0.7f, 0);
    glScalef(0.4f, 1.4f, 0.4f);
    PyramidShape::drawShadow(0.9f, 0.6f, 0.5f);
    glPopMatrix();
    glPushMatrix();
    glRotatef(300, 0, 0, 1);
    glTranslatef(-0.6f, -0.7f, 0);
    glScalef(0.4f, 1.4f, 0.4f);
    PyramidShape::drawShadow(0.9f, 0.6f, 0.5f);
    glPopMatrix();
    Screen3D::setColor(1, 0.9f, 1.0f);
    glPushMatrix();
    glTranslatef(0, -0.5f, 0);
    glScalef(0.3f, 1.0f, 0.3f);
    PyramidShape::draw();
    glPopMatrix();
    Screen3D::setColor(0.9f, 0.6f, 0.5f);
    glPushMatrix();
    glRotatef(60, 0, 0, 1);
    glTranslatef(0.6f, -0.7f, 0);
    glScalef(0.2f, 1.2f, 0.2f);
    PyramidShape::draw();
    glPopMatrix();
    Screen3D::setColor(0.9f, 0.6f, 0.5f);
    glPushMatrix();
    glRotatef(300, 0, 0, 1);
    glTranslatef(-0.6f, -0.7f, 0);
    glScalef(0.2f, 1.2f, 0.2f);
    PyramidShape::draw();
    glPopMatrix();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  }
};

class Enemy2TrailShape : public EnemyShape {
public:
  Enemy2TrailShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glPushMatrix();
    glTranslatef(0, -0.5f, 0);
    glScalef(0.3f, 1.0f, 0.3f);
    PyramidShape::drawLineShape();
    glPopMatrix();
    glPushMatrix();
    glRotatef(60, 0, 0, 1);
    glTranslatef(0.6f, -0.7f, 0);
    glScalef(0.2f, 1.2f, 0.2f);
    PyramidShape::drawLineShape();
    glPopMatrix();
    glPushMatrix();
    glRotatef(300, 0, 0, 1);
    glTranslatef(-0.6f, -0.7f, 0);
    glScalef(0.2f, 1.2f, 0.2f);
    PyramidShape::drawLineShape();
    glPopMatrix();
  }
};

class Enemy3Shape : public EnemyShape {
public:
  Enemy3Shape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(0, -0.4f, 0);
    glScalef(0.5f, 1.4f, 0.5f);
    PyramidShape::drawShadow(0.5f, 0.5f, 0.3f);
    glPopMatrix();
    glPushMatrix();
    glRotatef(150, 0, 0, 1);
    glTranslatef(0.5f, 0.2f, 0);
    glScalef(0.4f, 1.0f, 0.4f);
    PyramidShape::drawShadow(0.2f, 0.2f, 0.5f);
    glPopMatrix();
    Screen3D::setColor(0.2f, 0.2f, 0.5f);
    glPushMatrix();
    glRotatef(210, 0, 0, 1);
    glTranslatef(-0.5f, 0.2f, 0);
    glScalef(0.4f, 1.0f, 0.4f);
    PyramidShape::drawShadow(0.2f, 0.2f, 0.5f);
    glPopMatrix();
    Screen3D::setColor(1, 0.6f, 0.9f);
    glPushMatrix();
    glTranslatef(0, -0.4f, 0);
    glScalef(0.3f, 1.2f, 0.3f);
    PyramidShape::draw();
    glPopMatrix();
    Screen3D::setColor(0.3f, 0.5f, 1);
    glPushMatrix();
    glRotatef(150, 0, 0, 1);
    glTranslatef(0.5f, 0.2f, 0);
    glScalef(0.2f, 0.8f, 0.2f);
    PyramidShape::draw();
    glPopMatrix();
    Screen3D::setColor(0.3f, 0.5f, 1);
    glPushMatrix();
    glRotatef(210, 0, 0, 1);
    glTranslatef(-0.5f, 0.2f, 0);
    glScalef(0.2f, 0.8f, 0.2f);
    PyramidShape::draw();
    glPopMatrix();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  }
};

class Enemy3TrailShape : public EnemyShape {
public:
  Enemy3TrailShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glPushMatrix();
    glTranslatef(0, -0.4f, 0);
    glScalef(0.3f, 1.2f, 0.3f);
    PyramidShape::drawLineShape();
    glPopMatrix();
    glPushMatrix();
    glRotatef(150, 0, 0, 1);
    glTranslatef(0.5f, 0.2f, 0);
    glScalef(0.2f, 0.8f, 0.2f);
    PyramidShape::drawLineShape();
    glPopMatrix();
    glPushMatrix();
    glRotatef(210, 0, 0, 1);
    glTranslatef(-0.5f, 0.2f, 0);
    glScalef(0.2f, 0.8f, 0.2f);
    PyramidShape::drawLineShape();
    glPopMatrix();
  }
};

class TriangleParticleShape : public DisplayListShape {
public:
  TriangleParticleShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glBegin(GL_LINE_LOOP);
    glVertex3f(0, 0.5f, 0);
    glVertex3f(0.4f, -0.3f, 0);
    glVertex3f(-0.4f, -0.3f, 0);
    glEnd();
  }
};

class PillarShape : public DisplayListShape {
public:
  static constexpr float TICKNESS = 4.0f;

private:
  static constexpr float RADIUS_RATIO = 0.3f;

public:
  virtual ~PillarShape() {}

protected:
  void drawPillar(float r, float g, float b, bool outside = false) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    Screen3D::setColor(r, g, b);
    for (int i = 0; i < 8; i++) {
      float d = PI * 2 * i / 8;
      glVertex3f(sin(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO, TICKNESS,
                 cos(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO);
      d += PI * 2 / 8;
      glVertex3f(sin(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO, TICKNESS,
                 cos(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO);
      glVertex3f(sin(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO, -TICKNESS,
                 cos(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO);
      d -= PI * 2 / 8;
      glVertex3f(sin(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO, -TICKNESS,
                 cos(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO);
    }
    glEnd();
    if (!outside) {
      Screen3D::setColor(r, g, b);
      glBegin(GL_TRIANGLES);
      for (int i = 0; i < 8; i++) {
        float d = PI * 2 * i / 8;
        glVertex3f(0, TICKNESS, 0);
        glVertex3f(sin(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO, TICKNESS,
                   cos(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO);
        d += PI * 2 / 8;
        glVertex3f(sin(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO, TICKNESS,
                   cos(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO);
        d -= PI * 2 / 8;
        glVertex3f(0, -TICKNESS, 0);
        glVertex3f(sin(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO, -TICKNESS,
                   cos(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO);
        d += PI * 2 / 8;
        glVertex3f(sin(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO, -TICKNESS,
                   cos(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO);
      }
      glEnd();
    }
    Screen3D::setColor(0.1f, 0.1f, 0.1f);
    for (int i = 0; i < 8; i++) {
      float d = PI * 2 * i / 8;
      glBegin(GL_LINE_STRIP);
      glVertex3f(sin(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO, TICKNESS,
                 cos(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO);
      d += PI * 2 / 8;
      glVertex3f(sin(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO, TICKNESS,
                 cos(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO);
      glVertex3f(sin(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO, -TICKNESS,
                 cos(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO);
      d -= PI * 2 / 8;
      glVertex3f(sin(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO, -TICKNESS,
                 cos(d) * Field::CIRCLE_RADIUS * RADIUS_RATIO);
      glEnd();
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  }

public:
  void draw(float y, float deg) {
    glPushMatrix();
    glTranslatef(0, y, 0);
    glRotatef(deg * 180 / PI, 0, 1, 0);
    displayList->call();
    glPopMatrix();
  }
};

class Pillar1Shape : public PillarShape {
public:
  Pillar1Shape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glScalef(0.6f, 1.0f, 0.6f);
    drawPillar(0.5f, 0.4f, 0.4f);
  }
};

class Pillar2Shape : public PillarShape {
public:
  Pillar2Shape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glScalef(0.8f, 1.0f, 0.8f);
    drawPillar(0.6f, 0.3f, 0.3f);
  }
};

class Pillar3Shape : public PillarShape {
public:
  Pillar3Shape() { DisplayListShape_init(); }

protected:
  void drawList() override { drawPillar(0.5f, 0.5f, 0.4f); }
};

class Pillar4Shape : public PillarShape {
public:
  Pillar4Shape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glScalef(1.1f, 1.0f, 1.1f);
    drawPillar(0.5f, 0.4f, 0.5f);
  }
};

class OutsidePillarShape : public PillarShape {
public:
  OutsidePillarShape() { DisplayListShape_init(); }

protected:
  void drawList() override {
    glScalef(7.0f, 3.0f, 7.0f);
    drawPillar(0.2f, 0.2f, 0.3f, true);
  }
};

#endif // SHAPE_H
