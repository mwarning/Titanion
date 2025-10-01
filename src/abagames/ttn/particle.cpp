/*
 * $Id: particle.d,v 1.3 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include "particle.hpp"

#include "abagames/ttn/player.hpp"

Rand ParticleSpec::rand;

float ParticleSpec::calcNearPlayerAlpha(const Vector &pos) {
  if (!player->isActive())
    return 1;
  float pd = player->pos().dist(pos);
  if (pd < 20)
    return pd / 20;
  else
    return 1;
}

bool TriangleParticleSpec::move(ParticleState &ps) {
  ps.pos += ps.vel;
  ps.pos.x = field->normalizeX(ps.pos.x);
  if (ps.effectedByGravity)
    ps.vel.y -= GRAVITY;
  ps.vel *= (1 - SLOW_DOWN_RATIO);
  ps.d1 += ps.vd1;
  ps.d2 += ps.vd2;
  ps.vd1 *= (1 - SLOW_DOWN_RATIO * 0.2f);
  ps.vd2 *= (1 - SLOW_DOWN_RATIO * 0.2f);
  float cfr = 1.0f - (1.0f / (float)ps.startCnt);
  if (cfr < 0)
    cfr = 0;
  ps.r *= cfr;
  ps.g *= cfr;
  ps.b *= cfr;
  ps.a *= cfr;
  float fs = 0;
  if (ps.size > 2.0f && rand.nextInt(45) == 0)
    fs = 0.5f - rand.nextFloat(0.2f);
  else if (ps.size > 0.5f && rand.nextInt(10) == 0)
    fs = 0.1f + rand.nextSignedFloat(0.05f);
  if (fs > 0) {
    float vx = ps.vel.x * rand.nextSignedFloat(0.8f);
    float vy = ps.vel.y * rand.nextSignedFloat(0.8f);
    ps.vel.x -= vx * fs;
    ps.vel.y -= vy * fs;
    float cr = 1 - fs * 0.2f;
    ps.vel /= cr;
    Particle *p = particles->getInstanceForced();
    int nc = (int)(ps.cnt * (0.8f + fs * 0.2f));
    if (nc > 0)
      p->setByVelocity(ps.pos.x, ps.pos.y, vx, vy, ps.size * fs, ps.r, ps.g,
                       ps.b, ps.a, nc, ps.effectedByGravity);
    ps.size *= (1 - fs);
    ps.cnt *= cr;
  }
  ps.cnt--;
  if (ps.cnt <= 0)
    return false;
  return true;
}

void TriangleParticleSpec::draw(const ParticleState &ps) {
  Vector3 p = field->calcCircularPos(ps.pos);
  float aa = ps.a * calcNearPlayerAlpha(ps.pos);
  Screen3D::setColor(ps.r, ps.g, ps.b, aa);
  particleShape->draw3(p, ps.d1, ps.d2);
}

void LineParticleSpec::draw(const ParticleState &ps) {
  Vector3 p;
  glBegin(GL_LINES);
  float aa = ps.a; // * calcNearPlayerAlpha(pos);
  Screen3D::setColor(ps.r, ps.g, ps.b, aa);
  p = field->calcCircularPos(ps.pos);
  Screen3D::glVertex(p);
  p = field->calcCircularPos(ps.tailPos);
  Screen3D::glVertex(p);
  glEnd();
}

void QuadParticleSpec::draw(const ParticleState &ps) {
  Vector3 p;
  float sz = ps.size * 0.5f;
  float aa = ps.a * calcNearPlayerAlpha(ps.pos);
  Screen3D::setColor(ps.r, ps.g, ps.b, aa);
  glBegin(GL_QUADS);
  p = field->calcCircularPos(ps.pos.x - sz, ps.pos.y - sz);
  Screen3D::glVertex(p);
  p = field->calcCircularPos(ps.pos.x + sz, ps.pos.y - sz);
  Screen3D::glVertex(p);
  p = field->calcCircularPos(ps.pos.x + sz, ps.pos.y + sz);
  Screen3D::glVertex(p);
  p = field->calcCircularPos(ps.pos.x - sz, ps.pos.y + sz);
  Screen3D::glVertex(p);
  glEnd();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  Screen3D::setColor(0, 0, 0, aa * 0.66f);
  glBegin(GL_LINE_LOOP);
  p = field->calcCircularPos(ps.pos.x - sz, ps.pos.y - sz);
  Screen3D::glVertex(p);
  p = field->calcCircularPos(ps.pos.x + sz, ps.pos.y - sz);
  Screen3D::glVertex(p);
  p = field->calcCircularPos(ps.pos.x + sz, ps.pos.y + sz);
  Screen3D::glVertex(p);
  p = field->calcCircularPos(ps.pos.x - sz, ps.pos.y + sz);
  Screen3D::glVertex(p);
  glEnd();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

bool BonusParticleSpec::move(ParticleState &ps) {
  if (ps.waitCnt > 0) {
    ps.waitCnt--;
    return true;
  }
  ps.stepForward();
  ps.speed *= (1 - SLOW_DOWN_RATIO);
  field->addSlowdownRatio(0.01f);
  ps.pos.x = field->normalizeX(ps.pos.x);
  float cfr = 1.0f - (1.0f / (float)ps.startCnt);
  if (cfr < 0)
    cfr = 0;
  ps.a *= cfr;
  ps.num += (ps.trgNum - ps.num) * 0.2f;
  if (fabs(ps.trgNum - ps.num) < 0.5f)
    ps.num = ps.trgNum;
  ps.size += (ps.trgSize - ps.size) * 0.1f;
  ps.cnt--;
  if (ps.cnt <= 0)
    return false;
  return true;
}

void BonusParticleSpec::draw(const ParticleState &ps) {
  if (ps.waitCnt > 0)
    return;
  glPushMatrix();
  Vector3 p = field->calcCircularPos(ps.pos);
  float aa = ps.a * calcNearPlayerAlpha(ps.pos);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  Screen3D::setColor(1, 1, 1, aa * 0.5f);
  Screen3D::glTranslate(p);
  Letter::drawNumSign((int)ps.num, 0, 0, ps.size, 33, 0, 1);
  Screen3D::setColor(1, 1, 1, aa);
  Letter::drawNumSign((int)ps.num, 0, 0, ps.size, 33, 0, 2);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glPopMatrix();
}
