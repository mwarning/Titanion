/*
 * $Id: field.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include "field.hpp"

void Field::drawTorusShape(float d1s) {
  Vector3 cp;
  cp.y = 0;
  Vector3 ringOfs;
  float torusRad = CIRCLE_RADIUS * 0.9f;
  float ringRad;
  float d1;
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_QUADS);
  ringRad = CIRCLE_RADIUS * 0.3f;
  d1 = d1s;
  for (int i = 0; i < 16; i++, d1 += PI * 2 / 32) {
    float d2 = cnt * 0.003f;
    for (int j = 0; j < 16; j++, d2 += PI * 2 / 16) {
      cp.x = sin(d1) * torusRad;
      cp.z = cos(d1) * torusRad;
      createRingOffset(ringOfs, cp, ringRad, d1, d2);
      Screen3D::setColor(0.3f, 0.3f, 0.3f, 0.8f);
      Screen3D::glVertex(ringOfs);
      createRingOffset(ringOfs, cp, ringRad, d1, d2 + PI * 2 / 16);
      Screen3D::glVertex(ringOfs);
      cp.x = sin(d1 + PI * 2 / 32) * torusRad;
      cp.z = cos(d1 + PI * 2 / 32) * torusRad;
      createRingOffset(ringOfs, cp, ringRad, d1 + PI * 2 / 32,
                       d2 + PI * 2 / 16);
      Screen3D::glVertex(ringOfs);
      createRingOffset(ringOfs, cp, ringRad, d1 + PI * 2 / 32, d2);
      Screen3D::setColor(0.3f, 0.3f, 0.3f, 0.2f);
      Screen3D::glVertex(ringOfs);
    }
  }
  glEnd();
  glBegin(GL_LINE_STRIP);
  ringRad = CIRCLE_RADIUS * 0.3f;
  Screen3D::setColor(0.1f, 0.1f, 0.1f);
  d1 = d1s;
  for (int i = 0; i < 16; i++, d1 += PI * 2 / 32) {
    float d2 = cnt * 0.003f;
    for (int j = 0; j < 16; j++, d2 += PI * 2 / 16) {
      cp.x = sin(d1 + PI * 2 / 32 * 0.1f) * torusRad;
      cp.z = cos(d1 + PI * 2 / 32 * 0.1f) * torusRad;
      createRingOffset(ringOfs, cp, ringRad, d1 + PI * 2 / 32 * 0.1f,
                       d2 + PI * 2 / 16 * 0.1f);
      Screen3D::glVertex(ringOfs);
      createRingOffset(ringOfs, cp, ringRad, d1 + PI * 2 / 32 * 0.1f,
                       d2 + PI * 2 / 16 * 0.9f);
      Screen3D::glVertex(ringOfs);
      cp.x = sin(d1 + PI * 2 / 32 * 0.9f) * torusRad;
      cp.z = cos(d1 + PI * 2 / 32 * 0.9f) * torusRad;
      createRingOffset(ringOfs, cp, ringRad, d1 + PI * 2 / 32 * 0.9f,
                       d2 + PI * 2 / 32 * 0.1f);
      Screen3D::glVertex(ringOfs);
      createRingOffset(ringOfs, cp, ringRad, d1 + PI * 2 / 32 * 0.9f,
                       d2 + PI * 2 / 16 * 0.9f);
      Screen3D::glVertex(ringOfs);
    }
  }
  glEnd();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}
