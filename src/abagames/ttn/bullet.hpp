/*
 * $Id: bullet.d,v 1.4 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef BULLET_H
#define BULLET_H

#include <SDL/SDL_opengl.h>
#include <cmath>

#include "abagames/ttn/bullet.hpp"
#include "abagames/ttn/enemy.hpp"
#include "abagames/ttn/field.hpp"
#include "abagames/ttn/particle.hpp"
#include "abagames/ttn/screen.hpp"
#include "abagames/ttn/shape.hpp"
#include "abagames/ttn/token.hpp"
#include "abagames/util/actor.hpp"
#include "abagames/util/math.hpp"
#include "abagames/util/sdl/frame.hpp"
#include "abagames/util/vector.hpp"

/**
 * Enemies' bullets.
 */

class Player;
class EnemyPool;
class GameState;

class BulletState : public TokenState {
public:
  static int colorCnt;
  static float colorAlpha;
  Vector ppos;
  Vector tailPos;
  int cnt;
  int waitCnt;
  float speedRatio;

public:
  BulletState() {}

  static void move() {
    colorCnt++;
    int c = colorCnt % 30;
    if (c < 15)
      colorAlpha = (float)c / 15;
    else
      colorAlpha = 1 - (float)(c - 15) / 15;
  }

  void clear() override {
    ppos.x = ppos.y = 0;
    tailPos.x = tailPos.y = 0;
    cnt = 0;
    waitCnt = 0;
    speedRatio = 0;
    TokenState::clear();
  }
};

class BulletSpec : public TokenSpec<BulletState> {
public:
  static constexpr float DISAPPEAR_CNT = 300;
  Player *player;
  EnemyPool *enemies;
  ParticlePool *particles;
  Shape *lineShape;
  GameState *gameState;

public:
  BulletSpec(Field *field, Player *player, EnemyPool *enemies,
             ParticlePool *particles, Shape *shape, Shape *lineShape,
             GameState *gameState) {
    this->field = field;
    this->player = player;
    this->enemies = enemies;
    this->particles = particles;
    this->shape = shape;
    this->lineShape = lineShape;
    this->gameState = gameState;
  }

  void set(BulletState &bs) override {
    bs.ppos.x = bs.pos.x;
    bs.ppos.y = bs.pos.y;
    bs.tailPos.x = bs.pos.x;
    bs.tailPos.y = bs.pos.y;
    // assert(!isNaN(deg));
  }

  bool move(BulletState &bs) override;

  void draw(const BulletState &bs) override;
};

class Bullet : public Token<BulletState, BulletSpec> {
public:
  void setWaitCnt(int c) { state->waitCnt = c; }
};

class BulletPool : public ActorPool<Bullet> {
private:
  static constexpr float BULLET_REMOVED_RANGE = 2.0f;

public:
  void move() override {
    ActorPool<Bullet>::move();
    BulletState::move();
  }

  void removeAround(int &cnt, Vector pos, ParticlePool *particles,
                    ParticlePool *bonusParticles, Player *player);
};

#endif // BULLET_H
