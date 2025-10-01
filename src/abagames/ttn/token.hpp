/*
 * $Id: token.d,v 1.2 2006/11/23 02:29:44 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <cmath>
#include <vector>

#include "abagames/ttn/field.hpp"
#include "abagames/ttn/shape.hpp"
#include "abagames/util/actor.hpp"
#include "abagames/util/math.hpp"
#include "abagames/util/vector.hpp"

/**
 * Tokens of a player, enemies, bullets, particles, etc.
 * Handling these states (position, direction, speed, etc.) and
 *  specs (maneuver, method of attack, etc.).
 */
template <typename ST, typename SP> class Token : public Actor {
public:
  ST *state;
  SP *spec;

public:
  virtual ~Token() {}

  virtual void init() { state = new ST(); }

  void set(SP *spec, Vector pos, float deg, float speed) {
    set(spec, pos.x, pos.y, deg, speed);
  }

  void set(SP *spec, float x, float y, float deg, float speed) {
    this->spec = spec;
    set(x, y, deg, speed);
  }

  void set(float x, float y, float deg, float speed) {
    state->clear();
    state->pos.x = x;
    state->pos.y = y;
    state->deg = deg;
    state->speed = speed;
    spec->set(*state);
    _exists = true;
  }

  void move() {
    if (!spec->move(*state))
      remove();
  }

  void remove() {
    _exists = false;
    spec->removed(*state);
  }

  void draw() { spec->draw(*state); }

  Vector pos() const { return state->pos; }
};

/**
 * Holding a state of a token.
 */
class TokenState {
public:
  bool isInitialized = false;
  Vector pos;
  float deg;
  float speed;
  /*
    invariant {
      if (isInitialized) {
        assert(!isNaN(pos.x));
        assert(!isNaN(pos.y));
        assert(!isNaN(deg));
        assert(!isNaN(speed));
      }
    }
  */

public:
  TokenState() {}

  virtual ~TokenState() {}

  virtual void clear() {
    pos.x = pos.y = 0;
    deg = 0;
    speed = 0;
    isInitialized = true;
  }

  void stepForward() {
    pos.x -= sin(deg) * speed;
    pos.y += cos(deg) * speed;
  }
};

/**
 * Base class of a token's specification.
 */
template <typename T> class TokenSpec {
public:
  Field *field;
  Shape *shape;

public:
  virtual ~TokenSpec() {}

  virtual void set(T &state) {}
  virtual void removed(T &state) {}

  virtual bool move(T &state) { return true; }

  virtual void draw(const T &state) {
    Vector3 p = field->calcCircularPos(state.pos);
    float cd = field->calcCircularDeg(state.pos.x);
    shape->draw3(p, cd, state.deg);
  }
};

#endif // TOKEN_H
