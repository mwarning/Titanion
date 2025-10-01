/*
 * $Id: actor.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef ACTOR_H
#define ACTOR_H

#include <vector>

/**
 * Actor that has an interface to move and draw.
 */
class Actor {
protected:
  bool _exists;

public:
  virtual ~Actor() {}

  bool exists() const { return _exists; }

  bool exists(bool v) { return _exists = v; }

  virtual void move() = 0;
  virtual void draw() = 0;
};

/**
 * Object pool for actors.
 */
template <typename T> class ActorPool {
public:
  std::vector<T *> actors;

protected:
  int actorIdx = 0;

private:
  bool hasNoActor;

public:
  ActorPool() {}
  virtual ~ActorPool() {}

  void init(int n) { createActors(n); }

protected:
  void createActors(int n) {
    for (int i = 0; i < n; ++i) {
      T *a = new T();
      a->exists(false);
      a->init();
      actors.push_back(a);
    }
    actorIdx = 0;
    hasNoActor = false;
  }

public:
  T *getInstance() {
    if (hasNoActor)
      return NULL;
    for (size_t i = 0; i < actors.size(); i++) {
      actorIdx--;
      if (actorIdx < 0)
        actorIdx = actors.size() - 1;
      if (!actors[actorIdx]->exists())
        return actors[actorIdx];
    }
    hasNoActor = true;
    return NULL;
  }

  T *getInstanceForced() {
    actorIdx--;
    if (actorIdx < 0)
      actorIdx = actors.size() - 1;
    return actors[actorIdx];
  }

  std::vector<T *> getMultipleInstances(int n) {
    if (hasNoActor)
      return NULL;
    std::vector<T *> rsl;
    for (int i = 0; i < n; i++) {
      T *inst = getInstance();
      if (!inst) {
        for (auto r : rsl)
          r->exists = false;
        return NULL;
      }
      inst->exists = true;
      rsl.push_back(inst);
    }
    for (auto r : rsl)
      r->exists = false;
    return rsl;
  }

  virtual void move() {
    hasNoActor = false;
    for (auto a : actors)
      if (a->exists())
        a->move();
  }

  void draw() {
    for (auto a : actors)
      if (a->exists())
        a->draw();
  }

  void clear() {
    for (auto a : actors)
      a->exists(false);
    actorIdx = 0;
  }
};

#endif // ACTOR_H
