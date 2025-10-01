/*
 * $Id: enemy.d,v 1.7 2006/12/09 04:17:40 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef ENEMY_H
#define ENEMY_H

#include <cassert>
#include <cmath>

#include "abagames/ttn/bullet.hpp"
#include "abagames/ttn/field.hpp"
#include "abagames/ttn/shape.hpp"
#include "abagames/ttn/stage.hpp"
#include "abagames/ttn/token.hpp"
#include "abagames/util/actor.hpp"
#include "abagames/util/math.hpp"
#include "abagames/util/rand.hpp"
#include "abagames/util/sdl/frame.hpp"
#include "abagames/util/sdl/screen.hpp"
#include "abagames/util/sdl/sound.hpp"
#include "abagames/util/vector.hpp"

/**
 * Enemies and turrets.
 */

class Enemy;
class ParticlePool;

class Trail {
public:
  Vector pos;
  float deg;
  int cnt;
  /*
    invariant {
      assert(!isNaN(pos.x));
      assert(!isNaN(pos.y));
      assert(!isNaN(deg));
    }
  */
public:
  Trail() { deg = 0; }

  void set(float x, float y, float d, int c) {
    pos.x = x;
    pos.y = y;
    deg = d;
    cnt = c;
  }
};

class TurretState : public TokenState {
public:
  float fireCnt, burstCnt;
  int burstNum;
  int nwaySpeedAccelDir;
  /*
    invariant {
      if (isInitialized) {
        assert(!isNaN(fireCnt));
        assert(!isNaN(burstCnt));
      }
    }
  */
public:
  void clear() override {
    fireCnt = burstCnt = 0;
    burstNum = 0;
    nwaySpeedAccelDir = 1;
    TokenState::clear();
  }

  void update(float x, float y, float d) {
    pos.x = x;
    pos.y = y;
    if (burstNum <= 0)
      deg = d;
  }
};

class EnemyState : public TokenState {
public:
  static constexpr int TRAIL_NUM = 64;
  static constexpr int TRAIL_INTERVAL = 8;
  static constexpr int TURRET_MAX_NUM = 3;
  TurretState *turretStates[TURRET_MAX_NUM];
  Enemy *enemy;
  Vector vel;
  Vector centerPos, centerVel, standByPos;
  float baseBaseSpeed, baseSpeed;
  float baseAngVel, angVel;
  int waitCnt;
  int cnt;
  float ellipseRatio, ellipseDeg;
  float shield;
  int phase;
  int phaseCnt, nextPhaseCnt;
  int captureState, captureIdx;
  bool isGoingDownBeforeStandBy;
  Vector size, targetSize, sizeVel;
  Trail *trails[TRAIL_NUM];
  int trailIdx;
  bool trailLooped;
  bool isFirstEnemy;
  float anger;

  /*
    invariant {
      if (isInitialized) {
        assert(baseBaseSpeed >= 0);
        assert(baseSpeed >= 0);
        assert(baseAngVel >= 0);
        assert(angVel >= 0);
        assert(!isNaN(centerPos.x));
        assert(!isNaN(centerPos.y));
        assert(!isNaN(centerVel.x));
        assert(!isNaN(centerVel.y));
        assert(!isNaN(shield));
        assert(captureState >= 0);
        assert(!isNaN(size.x));
        assert(!isNaN(size.y));
        assert(!isNaN(targetSize.x));
        assert(!isNaN(targetSize.y));
        assert(!isNaN(sizeVel.x));
        assert(!isNaN(sizeVel.y));
        assert(!isNaN(anger));
      }
    }
  */
public:
  EnemyState();

  void clear() override;

  void move() {
    cnt++;
    anger *= 0.9995f;
  }

  void recordTrail();

  void drawTrails(EnemyShape *s, float r, float g, float b, Vector size,
                  Field *field) const;
};

class TurretSpec : public TokenSpec<TurretState> {
public:
  static Rand rand;

public:
  static constexpr float SPEED_RATIO = 5.0f;

private:
  static constexpr float INTERVAL_MAX = 90.0f;
  BulletSpec *bulletSpec;
  BulletPool *bullets;
  Player *player;
  Stage *stage;
  GameState *gameState;

public:
  int interval;
  float speed;
  float speedAccel;
  int burstNum, burstInterval;
  int nway;
  float nwayAngle;
  float nwayBaseDeg;
  float nwaySpeedAccel;
  bool fireingAtATime;
  float fireIntervalRatio;
  bool _disabled;
  float minimumFireDist;
  /*
    invariant {
      assert(interval > 0);
      assert(speed > 0);
      assert(speedAccel < 1 && speedAccel > -1);
      assert(burstNum >= 1);
      assert(burstInterval >= 0);
      assert(nway >= 1);
      assert(nwayAngle >= 0);
      assert(!isNaN(nwayBaseDeg));
      assert(!isNaN(nwaySpeedAccel));
      assert(!isNaN(fireIntervalRatio));
      assert(minimumFireDist >= 0);
    }
  */
public:
  TurretSpec(Field *field, BulletPool *bullets, Player *player,
             EnemyPool *enemies, ParticlePool *particles, Stage *stage,
             BulletSpec *bulletSpec, GameState *gameState) {
    this->bulletSpec = bulletSpec;
    this->field = field;
    this->bullets = bullets;
    this->player = player;
    this->stage = stage;
    this->gameState = gameState;
    initParam();
  }

  void initParam() {
    interval = 99999;
    speed = 1;
    speedAccel = 0;
    burstNum = 1;
    burstInterval = 99999;
    nway = 1;
    nwayAngle = 0;
    nwayBaseDeg = 0;
    nwaySpeedAccel = 0;
    fireingAtATime = false;
    fireIntervalRatio = 0;
    _disabled = false;
    minimumFireDist = 0;
  }

  void copy(const TurretSpec &ts) {
    interval = ts.interval;
    speed = ts.speed;
    speedAccel = ts.speedAccel;
    burstNum = ts.burstNum;
    burstInterval = ts.burstInterval;
    nway = ts.nway;
    nwayAngle = ts.nwayAngle;
    nwayBaseDeg = ts.nwayBaseDeg;
    nwaySpeedAccel = ts.nwaySpeedAccel;
    fireingAtATime = ts.fireingAtATime;
  }

  void set(TurretState &ts) override {
    setFireIntervalRatio(ts, fireIntervalRatio);
  }

  void setFireIntervalRatio(TurretState &ts, float fir) {
    ts.fireCnt = fir * interval;
  }

  void setRankNormal(float rank, bool isWide = false);

  void setRankMiddle(float rank);

  bool move(TurretState &ts) override { return move(ts, 1, 0); }

  bool move(TurretState &ts, float time = 1, float anger = 0);

private:
  bool isAbleToFire(const Vector &p);

public:
  bool disabled(bool v) { return _disabled = v; }
};

class EnemySpec : public TokenSpec<EnemyState> {
public:
  static Rand rand;

public:
  static constexpr float BULLET_HIT_WIDTH = 0.8f;
  static constexpr float NEXT_PHASE_DIST = 5;
  static constexpr int TURRET_MAX_NUM = 3;

protected:
  BulletPool *bullets;
  Player *player;
  ParticlePool *particles, *bonusParticles;
  EnemyPool *enemies;
  Stage *stage;
  EnemyShape *trailShape;
  BulletSpec *bulletSpec, *counterBulletSpec;
  TurretSpec *turretSpecs[TURRET_MAX_NUM];
  int turretNum;
  float turretWidth = 0;
  GameState *gameState;
  float shield = 1;
  float rank = 0;
  bool capturable;
  int score;
  std::string explosionSeName;
  bool removeBullets;
  /*
    invariant {
      assert(shield > 0);
      assert(rank >= 0);
      assert(turretWidth >= 0);
    }
  */
public:
  EnemySpec() {}

  EnemySpec(Field *field, BulletPool *bullets, Player *player,
            ParticlePool *particles, ParticlePool *bonusParticles,
            EnemyPool *enemies, Stage *stage, Shape *shape,
            EnemyShape *trailShape, BulletSpec *bulletSpec,
            BulletSpec *counterBulletSpec, GameState *gameState) {
    this->field = field;
    this->bullets = bullets;
    this->player = player;
    this->particles = particles;
    this->bonusParticles = bonusParticles;
    this->enemies = enemies;
    this->stage = stage;
    this->shape = shape;
    this->trailShape = trailShape;
    this->bulletSpec = bulletSpec;
    this->counterBulletSpec = counterBulletSpec;
    this->gameState = gameState;
  }

  void set(EnemyState &es) override {
    es.shield = shield;
    for (int i = 0; i < turretNum; i++)
      turretSpecs[i]->set(*es.turretStates[i]);
  }

  bool move(EnemyState &es) override;

private:
  void moveCaptured(EnemyState &es);

  float calcCapturePosX(int idx);

  void checkCaptured(EnemyState es);

public:
  virtual void hitCaptured(const EnemyState &es);

  virtual bool isBeingCaptured(const EnemyState &es) {
    return (es.captureState > 0);
  }

  virtual bool isCaptured(EnemyState &es) { return (es.captureState == 3); }

  virtual bool beforeAlign(const EnemyState &es) { return (es.phase < -10); }

  virtual bool hitShot(EnemyState &es, float dd = 0);

  virtual void destroyed(const EnemyState &es, float dd = 0);

  virtual void provacated(EnemyState &es);

  virtual bool gotoNextPhaseInAppearing(EnemyState &es);

  virtual void movePhase(EnemyState &es);

  virtual bool isInScreen(const EnemyState &es) {
    return field->size().contains(es.pos);
  }

public:
  virtual void setRank(float rank) = 0;
  virtual void init(EnemyState &es) = 0;
  virtual bool gotoNextPhase(EnemyState &es) = 0;
  virtual bool isInAttack(const EnemyState &es) = 0;

protected:
  virtual int calcStandByTime(const EnemyState &es) = 0;

public:
  void draw(const EnemyState &es) override;

  void drawTrails(const EnemyState &es);
};

class Enemy : public Token<EnemyState, EnemySpec> {
public:
  void init() override {
    Token<EnemyState, EnemySpec>::init();
    state->enemy = this;
  }

public:
  void setSmallEnemyState(float baseSpeed, float angVel, int waitCnt,
                          int appPattern, float er = 0, float ed = 0,
                          bool gd = false, float fireIntervalRatio = 0,
                          Enemy *firstEnemy = NULL);

  void setMiddleEnemyState(float baseSpeed, float angVel, float er = 0,
                           float ed = 0) {
    state->baseBaseSpeed = state->baseSpeed = baseSpeed;
    state->baseAngVel = state->angVel = angVel;
    state->ellipseRatio = er;
    state->ellipseDeg = ed;
    spec->init(*state);
  }

  void setGhostEnemyState(float x, float y, float deg, int cnt) {
    state->pos.x = x;
    state->pos.y = y;
    state->deg = deg;
    state->cnt = cnt;
  }

  void hitShot(float deg = 0) {
    if (spec->hitShot(*state, deg))
      remove();
  }

  void hitCaptured();

  void destroyed() {
    spec->destroyed(*state);
    _exists = false;
  }

  bool isInAttack() {
    if (spec->isBeingCaptured(*state))
      return false;
    return spec->isInAttack(*state);
  }

  bool isInScreen() {
    if (spec->isBeingCaptured(*state))
      return false;
    return spec->isInScreen(*state);
  }

  bool isBeingCaptured() { return spec->isBeingCaptured(*state); }

  bool isCaptured();

  bool beforeAlign() {
    if (spec->isBeingCaptured(*state))
      return false;
    return spec->beforeAlign(*state);
  }

  void drawTrails() { spec->drawTrails(*state); }

  Vector &pos() { return state->pos; }
};

class EnemyPool : public ActorPool<Enemy> {
public:
  static bool trailEffect;

private:
  Field *_field;

public:
  virtual ~EnemyPool() {}

  Enemy *getNearestEnemy(const Vector &p);

  Enemy *getNearestMiddleEnemy(const Vector &p);

  bool checkShotHit(const Vector &p, float deg, float widthRatio = 1.0f) {
    Enemy *e = getNearestEnemy(p);
    if (e) {
      float ox = _field->normalizeX(e->pos().x - p.x);
      float oy = e->pos().y - p.y;
      if (fabs(ox) < 1.0f * e->state->size.x &&
          fabs(oy) < 1.0f * e->state->size.y * widthRatio) {
        e->hitShot(deg);
        return true;
      }
    }
    return false;
  }

  bool checkBulletHit(const Vector &p, const Vector &pp) {
    bool hitf = false;
    for (auto e : actors) {
      if (e->exists() && e->isCaptured())
        if (_field->checkHitDist(e->pos(), p, pp,
                                 EnemySpec::BULLET_HIT_WIDTH)) {
          e->hitCaptured();
          hitf = true;
        }
    }
    return hitf;
  }

  bool checkEnemyHit(const Vector &p, const Vector &size) {
    bool hitf = false;
    for (auto e : actors) {
      if (e->exists() && e->isCaptured()) {
        float ox = _field->normalizeX(e->pos().x - p.x);
        float oy = e->pos().y - p.y;
        if (fabs(ox) < 0.5f * (e->state->size.x + size.x) &&
            fabs(oy) < 0.5f * (e->state->size.y + size.y)) {
          e->hitCaptured();
          hitf = true;
        }
      }
    }
    return hitf;
  }

  bool checkMiddleEnemyExists(float x, float px);

  int num() {
    int n = 0;
    for (auto e : actors)
      if (e->exists() && !e->isCaptured())
        n++;
    return n;
  }

  int numInAttack() {
    int n = 0;
    for (auto e : actors)
      if (e->exists() && e->isInAttack())
        n++;
    return n;
  }

  int numInScreen() {
    int n = 0;
    for (auto e : actors)
      if (e->exists() && e->isInScreen())
        n++;
    return n;
  }

  int numBeforeAlign() {
    int n = 0;
    for (auto e : actors)
      if (e->exists() && e->beforeAlign())
        n++;
    return n;
  }

  void drawFront() {
    if (trailEffect)
      for (auto a : actors)
        if (a->exists() && a->state->pos.y <= _field->size().y * 1.5f)
          a->drawTrails();
    for (auto a : actors)
      if (a->exists() && a->state->pos.y <= _field->size().y * 1.5f)
        a->draw();
  }

  void drawBack() {
    if (trailEffect)
      for (auto a : actors)
        if (a->exists() && a->state->pos.y > _field->size().y * 1.5f &&
            (a->state->pos.x <= _field->circularDistance() / 4 &&
             a->state->pos.x >= -_field->circularDistance() / 4))
          a->drawTrails();
    for (auto a : actors)
      if (a->exists() && a->state->pos.y > _field->size().y * 1.5f &&
          (a->state->pos.x <= _field->circularDistance() / 4 &&
           a->state->pos.x >= -_field->circularDistance() / 4))
        a->draw();
  }

  void drawPillarBack() {
    if (trailEffect)
      for (auto a : actors)
        if (a->exists() && a->state->pos.y > _field->size().y * 1.5f &&
            (a->state->pos.x > _field->circularDistance() / 4 ||
             a->state->pos.x < -_field->circularDistance() / 4))
          a->drawTrails();
    for (auto a : actors)
      if (a->exists() && a->state->pos.y > _field->size().y * 1.5f &&
          (a->state->pos.x > _field->circularDistance() / 4 ||
           a->state->pos.x < -_field->circularDistance() / 4))
        a->draw();
  }

  Field *field(Field *v) {
    _field = v;
    return v;
  }
};

class GhostEnemySpec : public EnemySpec {
public:
  GhostEnemySpec(Field *field, Shape *shape) {
    this->field = field;
    this->shape = shape;
  }

  void draw(const EnemyState &es) {
    Vector3 p = field->calcCircularPos(es.pos);
    float cd = field->calcCircularDeg(es.pos.x);
    Screen3D::setColor(0.5f, 0.5f, 1, 0.8f);
    dynamic_cast<EnemyShape *>(shape)->draw(p, cd, es.deg, es.cnt, es.size);
  }

  void set(EnemyState &es) override {}
  bool move(EnemyState &es) override { return true; }
  void destroyed(const EnemyState &es, float dd = 0) override {}
  void setRank(float rank) {}
  void init(EnemyState &es) {}
  bool gotoNextPhase(EnemyState &es) { return false; }
  bool isInAttack(const EnemyState &es) { return false; }
  bool isBeingCaptured(const EnemyState &es) { return true; }
  bool isCaptured(const EnemyState &es) { return true; }

protected:
  int calcStandByTime(const EnemyState &es) { return 0; }
};

class MiddleEnemySpec : public EnemySpec {
public:
  MiddleEnemySpec(Field *field, BulletPool *bullets, Player *player,
                  ParticlePool *particles, ParticlePool *bonusParticles,
                  EnemyPool *enemies, Stage *stage, Shape *shape,
                  EnemyShape *trailShape, BulletSpec *bulletSpec,
                  BulletSpec *counterBulletSpec, GameState *gameState);

  void init(EnemyState &es) override {
    es.size.x = es.size.y = 1.33f;
    es.phase = -300;
    gotoNextPhaseInAppearing(es);
  }

  void setRank(float r);

  bool gotoNextPhase(EnemyState &es) override;

  bool isInAttack(const EnemyState &es) override {
    return (es.phase == 1 || es.phase == 2);
  }

protected:
  int calcStandByTime(const EnemyState &es) override;
};

class SmallEnemySpec : public EnemySpec {
public:
  SmallEnemySpec(Field *field, BulletPool *bullets, Player *player,
                 ParticlePool *particles, ParticlePool *bonusParticles,
                 EnemyPool *enemies, Stage *stage, Shape *shape,
                 EnemyShape *trailShape, BulletSpec *bulletSpec,
                 BulletSpec *counterBulletSpec, GameState *gameState);

  void init(EnemyState &es) override { gotoNextPhaseInAppearing(es); }

  void init(EnemyState &es, const EnemyState &fes) {
    es.centerPos.x = fes.centerPos.x;
    es.centerPos.y = fes.centerPos.y;
    es.standByPos.x = fes.standByPos.x;
    es.standByPos.y = fes.standByPos.y;
    es.nextPhaseCnt = fes.nextPhaseCnt;
    es.baseSpeed = fes.baseSpeed;
    es.angVel = fes.angVel;
    es.phase = fes.phase;
    es.size.x = es.size.y = 1.25f;
  }

  void setRank(float r);

protected:
  int calcStandByTime(const EnemyState &es) override {
    return 60 + rand.nextInt(120);
  }
};

class SE1Spec : public SmallEnemySpec {
public:
  SE1Spec(Field *field, BulletPool *bullets, Player *player,
          ParticlePool *particles, ParticlePool *bonusParticles,
          EnemyPool *enemies, Stage *stage, Shape *shape,
          EnemyShape *trailShape, BulletSpec *bulletSpec,
          BulletSpec *counterBulletSpec, GameState *gameState)
      : SmallEnemySpec(field, bullets, player, particles, bonusParticles,
                       enemies, stage, shape, trailShape, bulletSpec,
                       counterBulletSpec, gameState) {
    explosionSeName = "explosion1.wav";
  }

  bool gotoNextPhase(EnemyState &es) override;

  bool isInAttack(const EnemyState &es) override {
    return (es.phase < -10 || es.phase == 1 || es.phase == 2);
  }
};

class SE2Spec : public SmallEnemySpec {
public:
  SE2Spec(Field *field, BulletPool *bullets, Player *player,
          ParticlePool *particles, ParticlePool *bonusParticles,
          EnemyPool *enemies, Stage *stage, Shape *shape,
          EnemyShape *trailShape, BulletSpec *bulletSpec,
          BulletSpec *counterBulletSpec, GameState *gameState)
      : SmallEnemySpec(field, bullets, player, particles, bonusParticles,
                       enemies, stage, shape, trailShape, bulletSpec,
                       counterBulletSpec, gameState) {
    explosionSeName = "explosion2.wav";
  }

  bool gotoNextPhase(EnemyState &es) override;

  void movePhase(EnemyState &es) override {
    SmallEnemySpec::movePhase(es);
    if (es.phase == 3) {
      if (es.centerPos.x < 0) {
        if (es.pos.x > -field->size().x * 1.2f)
          es.pos.x += (es.centerPos.x - es.pos.x) * 0.2f;
      } else {
        if (es.pos.x < field->size().x * 1.2f)
          es.pos.x += (es.centerPos.x - es.pos.x) * 0.2f;
      }
    }
  }

  bool isInAttack(const EnemyState &es) override {
    return (es.phase < -10 || es.phase == 1 || es.phase == 2 || es.phase == 3);
  }
};

#endif // ENEMY_H
