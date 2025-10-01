/*
 * $Id: player.d,v 1.4 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <cmath>
#include <vector>

#include "abagames/ttn/bullet.hpp"
#include "abagames/ttn/enemy.hpp"
#include "abagames/ttn/field.hpp"
#include "abagames/ttn/letter.hpp"
#include "abagames/ttn/particle.hpp"
#include "abagames/ttn/shape.hpp"
#include "abagames/ttn/sound.hpp"
#include "abagames/ttn/token.hpp"
#include "abagames/util/actor.hpp"
#include "abagames/util/math.hpp"
#include "abagames/util/rand.hpp"
#include "abagames/util/sdl/frame.hpp"
#include "abagames/util/sdl/pad.hpp"
#include "abagames/util/sdl/recordableinput.hpp"
#include "abagames/util/sdl/screen.hpp"
#include "abagames/util/vector.hpp"

/**
 * Player and shots.
 */

class Enemy;
class PlayerSpec;
class Shot;
class EnemyPool;
class BulletPool;
class PlayerState;
class GameState;
class TractorBeam;
class GhostEnemySpec;

class TractorBeam {
  static constexpr float MAX_LENGTH = 10;
  static constexpr float WIDTH = 3.0f;
  static constexpr float SHAPE_INTERVAL_TIME = 10;
  static constexpr float SHAPE_INTERVAL_LENGTH = 0.5f;
  Field *field;
  PlayerState *playerState;
  GameState *gameState;
  std::vector<TractorBeamShape *> shapes;
  float length = 0;
  int cnt;
  bool isExtending;
  /*
    invariant {
      assert(isNaN(length));
    }
  */
public:
  TractorBeam(Field *field, PlayerState *playerState, GameState *gameState) {
    this->field = field;
    this->playerState = playerState;
    this->gameState = gameState;
    shapes.push_back(new TractorBeamShapeRed());
    shapes.push_back(new TractorBeamShapeBlue());
    shapes.push_back(new TractorBeamShapePurple());
    shapes.push_back(new TractorBeamShapeDarkRed());
    shapes.push_back(new TractorBeamShapeDarkBlue());
    shapes.push_back(new TractorBeamShapeDarkPurple());
    clear();
  }

  void clear() {
    length = 0;
    cnt = 0;
    isExtending = false;
  }

  void move() {
    if (length <= 0)
      return;
    cnt++;
    if (cnt % 12 == 0 && isExtending)
      SoundImpl::playSe("tractor.wav");
  }

  void extendLength(float ratio = 1) {
    length += (MAX_LENGTH - length) * 0.05f * ratio;
    isExtending = true;
  }

  bool reduceLength(float ratio = 1) {
    length += (0 - length) * 0.1f * ratio;
    if (length < 0.33f) {
      length = 0;
      return true;
    }
    isExtending = false;
    return false;
  }

  bool contains(const Vector &p);

  void draw();
};

class ShotState : public TokenState {
public: // private:
  Shot *parent;
  int cnt;

public:
  void clear() override {
    parent = NULL;
    cnt = 0;
    TokenState::clear();
  }
};

class ShotSpec : public TokenSpec<ShotState> {
private:
  EnemyPool *enemies;
  BulletPool *bullets;
  PlayerState *playerState;
  GameState *gameState;

public:
  ShotSpec(Field *field, EnemyPool *enemies, BulletPool *bullets,
           GameState *gameState) {
    this->field = field;
    this->enemies = enemies;
    this->bullets = bullets;
    this->gameState = gameState;
    shape = new ShotShape();
  }

  void setPlayerState(PlayerState *ps) { playerState = ps; }

  void close() { dynamic_cast<ShotShape *>(shape)->close(); }

  void set(ShotState &ss) override {
    ss.parent = NULL;
    ss.cnt = 0;
  }

  void setParent(ShotState *ss, Shot *s) { ss->parent = s; }

  bool move(ShotState &ss) override;

  bool checkParent(ShotState &ss);
};

class Shot : public Token<ShotState, ShotSpec> {
public:
  void setParent(Shot *s) { spec->setParent(state, s); }
};

class ShotPool : public ActorPool<Shot> {
public:
  virtual ~ShotPool() {}

  void checkParent();

  int num();
};

class PlayerState : public TokenState {
public:
  bool replayMode;

public:
  static constexpr int RESPAWN_INTERVAL = 72;
  static constexpr int INVINCIBLE_INTERVAL_RESPAWN = 240;
  static constexpr int MAX_CAPTURED_ENEMIES_NUM = 10;
  PlayerSpec *spec;
  std::vector<Enemy *> capturedEnemies;
  int capturedEnemyNum;
  int respawnCnt;
  bool isInRespawn;
  int invincibleCnt;
  bool isInvincible;
  int shotCnt;
  int capturedEnemyShotCnt;
  bool aPressed, bPressed;
  Vector vel;
  float capturedEnemyWidth;
  int colorCnt;
  bool isFirstShot;
  float captureBeamEnergy;
  bool captureBeamReleased;
  int ghostCnt;
  int ghostShotCnt;
  bool midEnemyProvacated;
  /*
    invariant {
      if (isInitialized) {
        assert(!isNaN(vel.x));
        assert(!isNaN(vel.y));
        assert(capturedEnemyWidth >= 0);
        assert(!isNaN(captureBeamEnergy));
      }
    }
  */
public:
  PlayerState();

  void setSpec(PlayerSpec *spec) { this->spec = spec; }

  void set() {
    reset();
    pos.x = 0;
    respawnCnt = 0;
    isInRespawn = false;
    aPressed = bPressed = true;
    shotCnt = 60;
  }

  void clear() override {
    capturedEnemyNum = 0;
    respawnCnt = invincibleCnt = 0;
    isInRespawn = isInvincible = false;
    shotCnt = 0;
    capturedEnemyShotCnt = 0;
    vel.x = vel.y = 0;
    capturedEnemyWidth = 1.0f;
    colorCnt = 0;
    isFirstShot = false;
    captureBeamEnergy = 0;
    captureBeamReleased = false;
    ghostCnt = 0;
    ghostShotCnt = 0;
    midEnemyProvacated = false;
    TokenState::clear();
  }

private:
  void reset();

public:
  void move() {
    colorCnt++;
    ghostCnt++;
    if (isInRespawn) {
      respawnCnt--;
      if (respawnCnt <= 0) {
        reset();
        isInRespawn = false;
      }
    } else if (isInvincible) {
      invincibleCnt--;
      if (invincibleCnt <= 0)
        isInvincible = false;
    }
    midEnemyProvacated = false;
  }

  bool isActive() const { return !isInRespawn; }

  bool hasCollision() const { return (!isInRespawn && !isInvincible); }

  bool hasShape() const {
    if (isInRespawn)
      return false;
    if (!isInvincible)
      return true;
    if (invincibleCnt % 60 < 30)
      return false;
    else
      return true;
  }

  void destroyed() {
    respawnCnt = RESPAWN_INTERVAL;
    destroyCapturedEnemies(0);
    isInRespawn = true;
  }

  int addCapturedEnemy(Enemy *e) {
    if (isInRespawn ||
        capturedEnemyNum >= PlayerState::MAX_CAPTURED_ENEMIES_NUM)
      return -1;
    capturedEnemies[capturedEnemyNum] = e;
    capturedEnemyNum++;
    return capturedEnemyNum - 1;
  }

  void destroyCapturedEnemies(int idx);

  void countShotHit() {
    captureBeamEnergy += 0.02f / (capturedEnemyNum + 1);
    if (captureBeamEnergy > 1)
      captureBeamEnergy = 1;
  }
};

class PlayerSpec : public TokenSpec<PlayerState> {
public:
  static Rand rand;

public:
  static constexpr float BASE_SPEED = 0.15f;
  static constexpr float BASE_VELOCITY = 0.03f;
  static constexpr float CAPTURED_ENEMIES_INTERVAL_LENGTH = 1.2f;

public:
  static constexpr float TILT_DEG = 1.0f;
  static constexpr float SHOT_INTERVAL = 3;
  static constexpr float FIRST_SHOT_INTERVAL = 6;
  static constexpr int TWIN_SHOT_MAX_NUM = 2;
  ShotPool *shots, *capturedEnemiesShots;
  ShotSpec *shotSpec;
  EnemyPool *enemies;
  BulletPool *bullets;
  ParticlePool *particles;
  RecordablePad *pad;
  GameState *gameState;
  PlayerState *playerState;
  TractorBeam *tractorBeam;
  Shape *lineShape;
  float bulletHitWidth;
  GhostEnemySpec *ghostEnemySpec;
  EnemyShape *ghostEnemyShape;
  int shotMaxNum;

public:
  PlayerSpec(Pad *pad, GameState *gameState, Field *field, EnemyPool *enemies,
             BulletPool *bullets, ParticlePool *particles);

  void setState(PlayerState *ps) {
    playerState = ps;
    shotSpec->setPlayerState(ps);
    tractorBeam = new TractorBeam(field, ps, gameState);
  }

  void close() {
    ghostEnemyShape->close();
    dynamic_cast<PlayerShape *>(shape)->close();
    shotSpec->close();
  }

  void start();

  void respawn(PlayerState &ps);

  void clear();

  bool move(PlayerState &ps) override;

public:
  void fireShot(PlayerState &ps);

  void addShotParticle(Vector p, float d);

  void addVelocity(PlayerState &ps, Vector v, Vector o);

  void destroyed(PlayerState &ps);

  void addScore(int sc);

  void addMultiplier(float mp);

  float multiplier();

  void draw(const PlayerState &ps) override;

  void drawState(PlayerState &ps);
};

class Player : public Token<PlayerState, PlayerSpec> {
private:
  Vector hitOffset;

public:
  Player(PlayerSpec *spec) {
    state = new PlayerState();
    this->spec = spec;
    spec->setState(state);
    state->setSpec(spec);
  }

  bool replayMode(bool v) { return state->replayMode = v; }

  void set() {
    state->set();
    spec->start();
    hitOffset.x = hitOffset.y = 0;
    spec->field->setEyePos(pos());
  }

  bool checkBulletHit(const Vector &p, const Vector &pp);

  bool checkEnemyHit(const Vector &p, const Vector &v, Vector size);

  void destroy() {
    remove();
    spec->destroyed(*state);
  }

  void drawState();

  void destroyCapturedEnemies(int idx) { state->destroyCapturedEnemies(idx); }

  bool isInTractorBeam(const Vector &p) {
    return spec->tractorBeam->contains(p);
  }

  int addCapturedEnemy(Enemy *e) { return state->addCapturedEnemy(e); }

  float capturedEnemyWidth() { return state->capturedEnemyWidth; }

  void midEnemyProvacated() { state->midEnemyProvacated = true; }

  void addScore(int sc) { spec->addScore(sc); }

  void addMultiplier(float mp) { spec->addMultiplier(mp); }

  float multiplier() { return spec->multiplier(); }

  float deg() { return state->deg; }

  bool isActive() { return state->isActive(); }

  bool hasCollision() { return state->hasCollision(); }

  bool enemiesHasCollision();
};

#endif // PLAYER_H
