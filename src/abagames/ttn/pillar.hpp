/*
 * $Id: pillar.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef PILLAR_H
#define PILLAR_H

#include <algorithm>
#include <cmath>
#include <vector>

#include "abagames/ttn/enemy.hpp"
#include "abagames/ttn/field.hpp"
#include "abagames/ttn/shape.hpp"
#include "abagames/ttn/token.hpp"
#include "abagames/util/actor.hpp"

/**
 * Pillars at the center and on the background.
 */

class Pillar;
class PillarShape;
class PillarSpec;

class PillarState : public TokenState {
public:
  Pillar *previousPillar;
  float vy, vdeg;
  float maxY;
  PillarShape *pshape;
  bool isEnded;
  bool isOutside;

public:
  void clear() override {
    previousPillar = NULL;
    vy = 0;
    vdeg = 0;
    maxY = 0;
    isEnded = false;
    isOutside = false;
    TokenState::clear();
  }
};

class PillarSpec : public TokenSpec<PillarState> {
private:
  static constexpr float VELOCITY_Y = 0.025f;

public:
  PillarSpec(Field *field) { this->field = field; }

  bool move(PillarState &ps) override;

  void draw(const PillarState &ps) override {
    ps.pshape->draw(ps.pos.y, ps.deg);
  }
};

class Pillar : public Token<PillarState, PillarSpec> {
public:
  void set(PillarSpec *ps, float y, float maxY, Pillar *pp, PillarShape *s,
           float vdeg, bool outside = false) {
    Token<PillarState, PillarSpec>::set(ps, 0, y, 0, 0);
    state->maxY = maxY;
    state->previousPillar = pp;
    state->pshape = s;
    state->vdeg = vdeg;
    state->isOutside = outside;
  }

  void setEnd() { state->isEnded = true; }
  /*
    int opCmp(Pillar *p) {
      if (!p)
        return 0;
      return (int) (fabs(p->pos.y) - fabs(pos().y));
    }
  */
};

class PillarPool : public ActorPool<Pillar> {
public:
  void setEnd() {
    for (auto a : actors)
      if (a->exists())
        a->setEnd();
  }

  void drawCenter();

  void drawOutside() {
    for (auto a : actors)
      if (a->exists() && a->state->isOutside)
        a->draw();
  }
};

#endif // PILLAR_H
