/*
 * $Id: stage.d,v 1.6 2006/12/09 04:17:40 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef STAGE_H
#define STAGE_H

#include <cmath>

#include "abagames/ttn/field.hpp"
#include "abagames/ttn/letter.hpp"
#include "abagames/ttn/sound.hpp"
#include "abagames/ttn/token.hpp"
#include "abagames/util/rand.hpp"
#include "abagames/util/sdl/frame.hpp"
#include "abagames/util/vector.hpp"

/**
 * Enemy appearance pattern handler.
 */

class EnemySpec;
class EnemyShape;
class EnemyPool;
class BulletSpec;
class BulletPool;
class BulletShapeBase;
class RollBulletShapeBase;
class PillarSpec;
class PillarShape;
class PillarPool;
class ParticlePool;
class Player;
class GameState;

class Stage {
public:
  bool randomized;

private:
  static constexpr int PHASE_RESULT_SHOW_CNT = 150;
  static constexpr int PHASE_START_SHOW_CNT = 90;
  Field *field;
  EnemyPool *enemies;
  BulletPool *bullets;
  Player *player;
  ParticlePool *particles, *bonusParticles;
  PillarPool *pillars;
  GameState *gameState;
  Rand *rand;
  int appCnt;
  EnemySpec *middleEnemySpec, *smallEnemy1Spec, *smallEnemy2Spec;
  EnemyShape *enemy1Shape, *enemy2Shape, *enemy3Shape;
  EnemyShape *enemy1TrailShape, *enemy2TrailShape, *enemy3TrailShape;
  BulletSpec *bulletSpec, *middleBulletSpec, *counterBulletSpec;
  BulletShapeBase *bulletShape, *bulletLineShape, *middleBulletShape,
      *middleBulletLineShape;
  RollBulletShapeBase *counterBulletShape, *counterBulletLineShape;
  PillarSpec *pillarSpec;
  std::vector<PillarShape *> pillarShapes;
  PillarShape *outsidePillarShape;
  int smallEnemyNum;
  int smallEnemyFormationNum;
  float rank;
  int phaseTime;
  bool stageStarted, waitNextFormationPhase;
  int middleEnemyAppInterval;
  int _attackSmallEnemyNum;
  float goingDownBeforeStandByRatio;
  int appCntInterval;
  int formationIdx;
  int cnt;
  float rankTrg;
  int phaseNum;
  int shotFiredNum, shotHitNum;
  int shotFiredNumRsl, shotHitNumRsl;
  int shotFiredNumTotal, shotHitNumTotal;
  float hitRatio;
  int hitRatioBonus;
  bool _existsCounterBullet;

public:
  Stage(Field *field, EnemyPool *enemies, BulletPool *bullets, Player *player,
        ParticlePool *particles, ParticlePool *bonusParticles,
        PillarPool *pillars, GameState *gameState);

  void close();

  void start(long randSeed);

private:
  void clear();

  void startPhase();

  void calcHitRatioBonus();

  void setEnemySpecs();

  void initPillars();

public:
  void move();

  void moveOutsidePillars();

  void countShotFired() {
    if (phaseTime >= PHASE_RESULT_SHOW_CNT) {
      shotFiredNum++;
      shotFiredNumTotal++;
    }
  }

  void countShotHit() {
    if (phaseTime >= PHASE_RESULT_SHOW_CNT) {
      shotHitNum++;
      shotHitNumTotal++;
    }
  }

  void draw();

  void drawPhaseNum() { Letter::drawNum(phaseNum, 622, 448, 10); }

  void drawGameover() {
    float hr = 0;
    if (shotFiredNumTotal > 0)
      hr = (float)shotHitNumTotal / shotFiredNumTotal;
    Letter::drawString("SHOTS FIRED", 152, 250, 6, Letter::Direction::TO_RIGHT,
                       false, 0, 1, 1, 0.33f);
    Letter::drawNum(shotFiredNumTotal, 480, 250, 6);
    Letter::drawString("NUMBER OF HITS", 152, 280, 6,
                       Letter::Direction::TO_RIGHT, false, 0, 1, 1, 0.33f);
    Letter::drawNum(shotHitNumTotal, 480, 280, 6);
    Letter::drawString("HIT-MISS RATIO", 152, 310, 6);
    Letter::drawNum((int)(hr * 10000), 480, 310, 6, 3, -1, 2);
  }

  int attackSmallEnemyNum() { return _attackSmallEnemyNum; }

  bool existsCounterBullet();
};

#endif // STAGE_H
