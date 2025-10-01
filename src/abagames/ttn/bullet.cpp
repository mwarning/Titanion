/*
 * $Id: bullet.d,v 1.4 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include "bullet.hpp"

#include "abagames/ttn/frame.hpp"
#include "abagames/ttn/player.hpp"

int BulletState::colorCnt = 0;
float BulletState::colorAlpha = 0;

bool BulletSpec::move(BulletState &bs) {
  if (bs.waitCnt > 0) {
    bs.waitCnt--;
    return true;
  }
  bs.ppos.x = bs.pos.x;
  bs.ppos.y = bs.pos.y;
  float sp = bs.speed;
  if (gameState->mode() != GameState::Mode::CLASSIC && bs.cnt < 40)
    sp *= ((float)(bs.cnt + 10) / 50);
  bs.tailPos.x -= sin(bs.deg) * sp * 0.7f;
  bs.tailPos.y += cos(bs.deg) * sp * 0.7f;
  bs.pos.x -= sin(bs.deg) * sp;
  bs.pos.y += cos(bs.deg) * sp;
  field->addSlowdownRatio(bs.speed * 0.04f);
  bs.pos.x = field->normalizeX(bs.pos.x);
  if (!field->containsOuter(bs.pos))
    return false;
  if (!field->contains(bs.pos) || bs.cnt >= DISAPPEAR_CNT * 0.9f) {
    bs.tailPos.x += (bs.pos.x - bs.tailPos.x) * 0.1f;
    bs.tailPos.y += (bs.pos.y - bs.tailPos.y) * 0.1f;
  }
  bs.tailPos.x = field->normalizeX(bs.tailPos.x);
  if (player->enemiesHasCollision())
    if (enemies->checkBulletHit(bs.pos, bs.ppos))
      return false;
  if (player->checkBulletHit(bs.pos, bs.ppos))
    return false;
  bs.cnt++;
  if (bs.cnt >= DISAPPEAR_CNT)
    return false;
  return true;
}

void BulletSpec::draw(const BulletState &bs) {
  if (bs.waitCnt > 0)
    return;
  Vector3 p;
  glBegin(GL_LINES);
  Screen3D::setColor(0.1f, 0.4f, 0.4f, 0.5f);
  p = field->calcCircularPos(bs.tailPos);
  Screen3D::glVertex(p);
  Screen3D::setColor(0.2f * bs.colorAlpha, 0.8f * bs.colorAlpha,
                     0.8f * bs.colorAlpha);
  p = field->calcCircularPos(bs.pos);
  Screen3D::glVertex(p);
  glEnd();
  p = field->calcCircularPos(bs.pos);
  float d;
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
    d = PI;
    break;
  case GameState::Mode::BASIC:
  case GameState::Mode::MODERN:
    d = bs.deg;
    break;
  }
  float cd = field->calcCircularDeg(bs.pos.x);
  dynamic_cast<BulletShapeBase *>(shape)->draw(p, cd, d, bs.cnt * 3.0f);
  Screen3D::setColor(0.6f * bs.colorAlpha, 0.9f * bs.colorAlpha,
                     0.9f * bs.colorAlpha);
  dynamic_cast<BulletShapeBase *>(lineShape)->draw(p, cd, d, bs.cnt * 3.0f);
}

void BulletPool::removeAround(int &cnt, Vector pos, ParticlePool *particles,
                              ParticlePool *bonusParticles, Player *player) {
  for (auto b : actors) {
    if (b->exists()) {
      if (b->pos().dist(pos) < BULLET_REMOVED_RANGE) {
        b->remove();
        player->addScore(cnt);
        cnt++;
        int wc;
        if (cnt <= 50)
          wc = cnt;
        else
          wc = 50 + (int)sqrt((float)(cnt - 50));
        Particle *bp = bonusParticles->getInstanceForced();
        bp->set(Particle::Shape::BONUS, b->state->pos.x, b->state->pos.y, 0,
                0.2f, 0.5f, 1, 1, 1, 60, false, cnt, wc);
        Particle *p = particles->getInstanceForced();
        p->set(Particle::Shape::QUAD, b->state->pos.x, b->state->pos.y,
               b->state->deg, b->state->speed, 1.5f, 0.5f, 0.75f, 1.0f, 60,
               false);
        this->removeAround(cnt, b->pos(), particles, bonusParticles, player);
      }
    }
  }
}
