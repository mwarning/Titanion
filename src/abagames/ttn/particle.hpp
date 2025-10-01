/*
 * $Id: particle.d,v 1.3 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef PARTICLE_H
#define PARTICLE_H

#include <cmath>

#include "abagames/ttn/field.hpp"
#include "abagames/ttn/letter.hpp"
#include "abagames/ttn/shape.hpp"
#include "abagames/ttn/token.hpp"
#include "abagames/util/actor.hpp"
#include "abagames/util/math.hpp"
#include "abagames/util/rand.hpp"
#include "abagames/util/vector.hpp"

class Player;
class Particle;

/**
 * Particles (Triangle / Line / Quad / Bonus).
 */

class ParticleState : public TokenState {
public:
  Vector vel;
  Vector tailPos;
  float size;
  int cnt, startCnt;
  float r, g, b, a;
  float d1, d2;
  float vd1, vd2;
  bool effectedByGravity;
  float num, trgNum;
  float trgSize;
  int waitCnt;

  /*
    invariant {
      if (isInitialized) {
        assert(!isNaN(pos.x));
        assert(!isNaN(pos.y));
        assert(!isNaN(vel.x));
        assert(!isNaN(vel.y));
        assert(!isNaN(tailPos.x));
        assert(!isNaN(tailPos.y));
        assert(size > 0 && size < 20);
        assert(r >= 0 && r <= 1);
        assert(g >= 0 && g <= 1);
        assert(b >= 0 && b <= 1);
        assert(a >= 0 && a <= 1);
        assert(!isNaN(d1));
        assert(!isNaN(d2));
        assert(!isNaN(vd1));
        assert(!isNaN(vd2));
        assert(!isNaN(num));
        assert(!isNaN(trgNum));
        assert(trgSize > 0);
      }
    }
  */

public:
  ParticleState() {}

  void clear() override {
    vel.x = vel.y = 0;
    size = 1;
    cnt = 0;
    r = g = b = 0;
    a = 1;
    d1 = d2 = 0;
    vd1 = vd2 = 0;
    effectedByGravity = false;
    num = trgNum = 1;
    trgSize = 1;
    waitCnt = 0;
    TokenState::clear();
  }
};

class ParticlePool : public ActorPool<Particle> {};

class ParticleSpec : public TokenSpec<ParticleState> {
public:
  static Rand rand;

private:
  Player *player;

public:
  void setPlayer(Player *player) { this->player = player; }

protected:
  float calcNearPlayerAlpha(const Vector &pos);
};

class TriangleParticleSpec : public ParticleSpec {
private:
  static constexpr float SLOW_DOWN_RATIO = 0.05f;
  static constexpr float GRAVITY = 0.003f;
  Shape *particleShape;
  ParticlePool *particles;

public:
  TriangleParticleSpec(Field *field) {
    this->field = field;
    particleShape = new TriangleParticleShape();
  }

  void setParticles(ParticlePool *particles) { this->particles = particles; }

  void set(ParticleState &ps) override {
    ps.d1 = rand.nextFloat(PI * 2);
    ps.d2 = rand.nextFloat(PI * 2);
    ps.vd1 = rand.nextSignedFloat(0.1f);
    ps.vd2 = rand.nextSignedFloat(0.1f);
  }

  bool move(ParticleState &ps) override;

  void draw(const ParticleState &ps) override;
};

class LineParticleSpec : public ParticleSpec {
private:
  static constexpr float SLOW_DOWN_RATIO = 0.03f;

public:
  LineParticleSpec(Field *field) { this->field = field; }

  void set(ParticleState &ps) override {
    ps.tailPos.x = ps.pos.x;
    ps.tailPos.y = ps.pos.y;
  }

  bool move(ParticleState &ps) override {
    ps.stepForward();
    ps.tailPos.x += (ps.pos.x - ps.tailPos.x) * 0.05f;
    ps.tailPos.y += (ps.pos.y - ps.tailPos.y) * 0.05f;
    ps.speed *= (1 - SLOW_DOWN_RATIO);
    ps.pos.x = field->normalizeX(ps.pos.x);
    float cfr = 1.0f - (1.0f / (float)ps.startCnt);
    if (cfr < 0)
      cfr = 0;
    ps.r *= cfr;
    ps.g *= cfr;
    ps.b *= cfr;
    ps.a *= cfr;
    ps.cnt--;
    if (ps.cnt <= 0)
      return false;
    return true;
  }

  void draw(const ParticleState &ps) override;
};

class QuadParticleSpec : public ParticleSpec {
private:
  static constexpr float SLOW_DOWN_RATIO = 0.07f;
  static constexpr float GRAVITY = 0.002f;

public:
  QuadParticleSpec(Field *field) { this->field = field; }

  bool move(ParticleState &ps) override {
    ps.pos += ps.vel;
    ps.pos.x = field->normalizeX(ps.pos.x);
    if (ps.effectedByGravity)
      ps.vel.y -= GRAVITY;
    ps.vel *= (1 - SLOW_DOWN_RATIO);
    float cfr = 1.0f - (1.0f / (float)ps.startCnt);
    if (cfr < 0)
      cfr = 0;
    ps.r *= cfr;
    ps.g *= cfr;
    ps.b *= cfr;
    ps.a *= cfr;
    ps.size *= (1 - (1 - cfr) * 0.5f);
    ps.cnt--;
    if (ps.cnt <= 0)
      return false;
    return true;
  }

  void draw(const ParticleState &ps) override;
};

class BonusParticleSpec : public ParticleSpec {
private:
  static constexpr float SLOW_DOWN_RATIO = 0.04f;

public:
  BonusParticleSpec(Field *field) { this->field = field; }

  void setSize(ParticleState &ps, float sz) {
    ps.trgSize = sz;
    ps.size = 0.1f;
  }

  bool move(ParticleState &ps) override;

  void draw(const ParticleState &ps) override;
};

class Particle : public Token<ParticleState, ParticleSpec> {
public:
  enum Shape {
    TRIANGLE,
    LINE,
    QUAD,
    BONUS,
  };

private:
  TriangleParticleSpec *triangleParticleSpec;
  LineParticleSpec *lineParticleSpec;
  QuadParticleSpec *quadParticleSpec;
  BonusParticleSpec *bonusParticleSpec;

public:
  void initParticle(TriangleParticleSpec *ps1, LineParticleSpec *ps2,
                    QuadParticleSpec *ps3, BonusParticleSpec *ps4) {
    Token<ParticleState, ParticleSpec>::init();
    triangleParticleSpec = ps1;
    lineParticleSpec = ps2;
    quadParticleSpec = ps3;
    bonusParticleSpec = ps4;
  }

  void set(int type, float x, float y, float deg, float speed, float sz,
           float r, float g, float b, int c = 60, bool ebg = true,
           float num = 0, int waitCnt = 0) {
    switch (type) {
    case Shape::TRIANGLE:
      spec = triangleParticleSpec;
      break;
    case Shape::LINE:
      spec = lineParticleSpec;
      break;
    case Shape::QUAD:
      spec = quadParticleSpec;
      break;
    case Shape::BONUS:
      spec = bonusParticleSpec;
      break;
    }
    this->spec = spec;
    Token<ParticleState, ParticleSpec>::set(x, y, deg, speed);
    state->size = sz;
    state->vel.x = -sin(deg) * speed;
    state->vel.y = cos(deg) * speed;
    state->r = r;
    state->g = g;
    state->b = b;
    state->cnt = state->startCnt = c;
    state->effectedByGravity = ebg;
    state->trgNum = num;
    state->waitCnt = waitCnt;
    if (type == Shape::BONUS)
      dynamic_cast<BonusParticleSpec *>(spec)->setSize(*state, sz);
  }

  void setByVelocity(float x, float y, float vx, float vy, float sz, float r,
                     float g, float b, float a, int c = 60, bool ebg = true) {
    spec = triangleParticleSpec;
    Token<ParticleState, ParticleSpec>::set(x, y, 0, 0);
    state->vel.x = vx;
    state->vel.y = vy;
    state->size = sz;
    state->r = r;
    state->g = g;
    state->b = b;
    state->a = a;
    state->cnt = state->startCnt = c;
    state->effectedByGravity = ebg;
  }
};

#endif // PARTICLE_H
