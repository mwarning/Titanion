/*
 * $Id: stage.d,v 1.6 2006/12/09 04:17:40 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include <vector>

#include "stage.hpp"

#include "abagames/ttn/bullet.hpp"
#include "abagames/ttn/enemy.hpp"
#include "abagames/ttn/frame.hpp"
#include "abagames/ttn/particle.hpp"
#include "abagames/ttn/pillar.hpp"
#include "abagames/ttn/player.hpp"
#include "abagames/ttn/shape.hpp"

Stage::Stage(Field *field, EnemyPool *enemies, BulletPool *bullets,
             Player *player, ParticlePool *particles,
             ParticlePool *bonusParticles, PillarPool *pillars,
             GameState *gameState) {
  this->field = field;
  this->enemies = enemies;
  this->bullets = bullets;
  this->player = player;
  this->particles = particles;
  this->bonusParticles = bonusParticles;
  this->pillars = pillars;
  this->gameState = gameState;
  rand = new Rand();
  enemy1Shape = new Enemy1Shape();
  enemy2Shape = new Enemy2Shape();
  enemy3Shape = new Enemy3Shape();
  enemy1TrailShape = new Enemy1TrailShape();
  enemy2TrailShape = new Enemy2TrailShape();
  enemy3TrailShape = new Enemy3TrailShape();
  bulletShape = new BulletShape();
  bulletLineShape = new BulletLineShape();
  middleBulletShape = new MiddleBulletShape();
  middleBulletLineShape = new MiddleBulletLineShape();
  counterBulletShape = new CounterBulletShape();
  counterBulletLineShape = new CounterBulletLineShape();
  bulletSpec = new BulletSpec(field, player, enemies, particles, bulletShape,
                              bulletLineShape, gameState);
  middleBulletSpec =
      new BulletSpec(field, player, enemies, particles, middleBulletShape,
                     middleBulletLineShape, gameState);
  counterBulletSpec =
      new BulletSpec(field, player, enemies, particles, counterBulletShape,
                     counterBulletLineShape, gameState);
  pillarSpec = new PillarSpec(field);
  pillarShapes.push_back(new Pillar1Shape());
  pillarShapes.push_back(new Pillar2Shape());
  pillarShapes.push_back(new Pillar3Shape());
  pillarShapes.push_back(new Pillar4Shape());
  outsidePillarShape = new OutsidePillarShape();
}

void Stage::close() {
  enemy1Shape->close();
  enemy2Shape->close();
  enemy3Shape->close();
  enemy1TrailShape->close();
  enemy2TrailShape->close();
  enemy3TrailShape->close();
  bulletShape->close();
  bulletLineShape->close();
  middleBulletShape->close();
  middleBulletLineShape->close();
  counterBulletShape->close();
  counterBulletLineShape->close();
  for (auto ps : pillarShapes)
    ps->close();
  outsidePillarShape->close();
}

void Stage::start(long randSeed) {
  clear();
  rand->setSeed(randSeed);
  EnemySpec::rand.setRandSeed(randSeed);
  TurretSpec::rand.setRandSeed(randSeed);
  PlayerSpec::rand.setRandSeed(randSeed);
  ParticleSpec::rand.setRandSeed(randSeed);
  SoundImpl::rand.setRandSeed(randSeed);
  rank = rankTrg = 0;
  phaseNum = 0;
  cnt = 0;
  shotFiredNumTotal = shotHitNumTotal = 0;
  for (int i = 0; i < 1000; i++) {
    cnt++;
    moveOutsidePillars();
    pillars->move();
  }
  startPhase();
}

void Stage::clear() {
  smallEnemyNum = smallEnemyFormationNum = 0;
  rank = 0;
  phaseTime = 0;
  stageStarted = waitNextFormationPhase = false;
  middleEnemyAppInterval = 0;
  _attackSmallEnemyNum = 0;
  goingDownBeforeStandByRatio = 0;
  appCntInterval = 0;
  formationIdx = 0;
  cnt = 0;
  rankTrg = 0;
  phaseNum = 0;
  shotFiredNum = shotHitNum = 0;
  shotFiredNumRsl = shotHitNumRsl = 0;
  shotFiredNumTotal = shotHitNumTotal = 0;
  hitRatio = 0;
  hitRatioBonus = 0;
  _existsCounterBullet = false;
}

void Stage::startPhase() {
  phaseTime = 0;
  phaseNum++;
  if (phaseNum > 1)
    calcHitRatioBonus();
  if (phaseNum % 10 == 0)
    SoundImpl::fadeBgm();
  setEnemySpecs();
  initPillars();
}

void Stage::calcHitRatioBonus() {
  shotFiredNumRsl = shotFiredNum;
  shotHitNumRsl = shotHitNum;
  shotFiredNum = shotHitNum = 0;
  if (shotFiredNumRsl <= 0) {
    hitRatio = 0;
  } else {
    hitRatio = (float)shotHitNumRsl / shotFiredNumRsl;
  }
  float r = (float)((int)(hitRatio * 100)) / 100;
  if (r > 1)
    r = 1;
  hitRatioBonus = (int)(10000.0f * r * r * r * r);
  if (gameState->mode() == GameState::Mode::MODERN)
    return;
  if (gameState->mode() == GameState::Mode::BASIC)
    hitRatioBonus *= 10;
  gameState->addScore(hitRatioBonus, true);
}

void Stage::setEnemySpecs() {
  rankTrg += 1.25f;
  rank += (rankTrg - rank) * 0.33f;
  if (phaseNum % 10 == 0)
    rank *= 0.1f;
  if (!randomized) {
    long rs = phaseNum;
    switch (gameState->mode()) {
    case GameState::Mode::CLASSIC:
      rs *= 2;
      break;
    case GameState::Mode::BASIC:
      break;
    case GameState::Mode::MODERN:
      rs *= 3;
      break;
    }
    rand->setSeed(rs);
    EnemySpec::rand.setRandSeed(rs);
    TurretSpec::rand.setRandSeed(rs);
  }
  _existsCounterBullet = false;
  int en;
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
    en = 24 + (int)((50 + rand->nextInt(10)) * sqrt(rank) * 0.2f);
    smallEnemyNum = 4 + rand->nextInt(2);
    if (rank > 10)
      _existsCounterBullet = true;
    middleEnemyAppInterval = 6 + rand->nextInt(2);
    break;
  case GameState::Mode::BASIC:
    en = 32 + (int)((50 + rand->nextInt(10)) * sqrt(rank) * 0.33f);
    smallEnemyNum = 7 + rand->nextInt(4);
    middleEnemyAppInterval = 5 + rand->nextInt(2);
    break;
  case GameState::Mode::MODERN:
    en = 24 + (int)((50 + rand->nextInt(10)) * sqrt(rank) * 0.5);
    smallEnemyNum = 4 + rand->nextInt(2);
    middleEnemyAppInterval = 7 + rand->nextInt(3);
    break;
  }
  smallEnemyFormationNum = (int)(en / smallEnemyNum) + 1;
  middleEnemySpec =
      new MiddleEnemySpec(field, bullets, player, particles, bonusParticles,
                          enemies, this, enemy3Shape, enemy3TrailShape,
                          middleBulletSpec, counterBulletSpec, gameState);
  middleEnemySpec->setRank(rank * 0.15f);
  smallEnemy1Spec = new SE1Spec(
      field, bullets, player, particles, bonusParticles, enemies, this,
      enemy1Shape, enemy1TrailShape, bulletSpec, counterBulletSpec, gameState);
  ((SE1Spec *)smallEnemy1Spec)->setRank(rank * 0.22f);
  smallEnemy2Spec = new SE2Spec(
      field, bullets, player, particles, bonusParticles, enemies, this,
      enemy2Shape, enemy2TrailShape, bulletSpec, counterBulletSpec, gameState);
  ((SE2Spec *)smallEnemy2Spec)->setRank(rank * 0.22f);
  _attackSmallEnemyNum = (int)sqrt(rank + 2);
  goingDownBeforeStandByRatio = 0;
  if (rand->nextFloat(rank + 1) > 2)
    goingDownBeforeStandByRatio = rand->nextFloat(0.2f) + 0.1f;
  appCntInterval = 48 + rand->nextSignedInt(10);
  appCntInterval *= (0.5f + 0.5f / sqrt(rank));
  if (gameState->mode() == GameState::Mode::MODERN) {
    appCntInterval *= 0.75f;
    _attackSmallEnemyNum *= 2;
  }
  appCnt = 0;
  formationIdx = 0;
  stageStarted = false;
  waitNextFormationPhase = false;
}

void Stage::initPillars() {
  pillars->setEnd();
  Pillar *pp = NULL;
  int pln = 0;
  int pn = phaseNum;
  std::vector<int> pshapes;
  for (;;) {
    if (pn <= 0)
      break;
    if (pn >= 20) {
      pshapes.push_back(3);
      pn -= 20;
    } else if (pn >= 10) {
      pshapes.push_back(2);
      pn -= 10;
    } else if (pn >= 5) {
      pshapes.push_back(1);
      pn -= 5;
    } else {
      pshapes.push_back(0);
      pn--;
    }
    pln++;
  }
  float maxY = -15 + pln * 8;
  for (int i = 0; i < pln; i++) {
    Pillar *p = pillars->getInstance();
    if (!p)
      break;
    p->set(pillarSpec, -80 - i * 10, maxY, pp, pillarShapes[pshapes[i]],
           (pln - i) * 0.03f);
    pp = p;
  }
}

void Stage::move() {
  if (appCnt <= 0) {
    if ((formationIdx % middleEnemyAppInterval) == middleEnemyAppInterval - 1) {
      Enemy *me = enemies->getInstance();
      if (!me)
        return;
      float x = rand->nextFloat(field->circularDistance());
      x = field->normalizeX(x);
      float sp = 0.1f + rand->nextSignedFloat(0.01f);
      float av = sp * 0.4f + rand->nextSignedFloat(0.005f);
      float er = rand->nextFloat(0.5f);
      float ed = rand->nextFloat(PI * 2);
      me->set(middleEnemySpec, x, field->size().y * Field::PIT_SIZE_Y_RATIO, PI,
              sp);
      me->setMiddleEnemyState(sp, av, er, ed);
    }
    float x = rand->nextFloat(field->circularDistance());
    x = field->normalizeX(x);
    float sp = 0.15f + rand->nextSignedFloat(0.01f);
    float av = sp * 0.5f + rand->nextSignedFloat(0.005f);
    float dst = sp * 6.0f;
    float er = rand->nextFloat(0.8f);
    float ed = rand->nextFloat(PI * 2);
    Enemy *fe = NULL;
    float fir = 0;
    for (int i = 0; i < smallEnemyNum; i++) {
      Enemy *e = enemies->getInstance();
      if (!e)
        break;
      SmallEnemySpec *ses;
      int appPattern = formationIdx % 2;
      switch (formationIdx % 3) {
      case 0:
      case 1:
        ses = dynamic_cast<SmallEnemySpec *>(smallEnemy1Spec);
        break;
      case 2:
        ses = dynamic_cast<SmallEnemySpec *>(smallEnemy2Spec);
        break;
      default:
        break;
      }
      e->set(ses, x, field->size().y * Field::PIT_SIZE_Y_RATIO + i * dst, PI,
             sp);
      bool gd = false;
      if (rand->nextFloat(1) < goingDownBeforeStandByRatio)
        gd = true;
      if (!fe) {
        e->setSmallEnemyState(sp, av, (int)(i * (dst / sp)), appPattern, er, ed,
                              gd);
        fe = e;
      } else {
        e->setSmallEnemyState(sp, av, (int)(i * (dst / sp)), appPattern, er, ed,
                              gd, fir, fe);
      }
      fir += (1.0f / smallEnemyNum);
    }
    smallEnemyFormationNum--;
    formationIdx++;
    if (smallEnemyFormationNum <= 0) {
      stageStarted = true;
      appCnt = 9999999;
    } else {
      appCnt += appCntInterval * (1 - 1 / (enemies->num() + 1));
    }
  }
  appCnt--;
  phaseTime++;
  if (phaseNum >= 10 && phaseNum % 10 == 0 && phaseTime == 120 &&
      gameState->isInGameAndNotGameOver())
    SoundImpl::nextBgm();
  cnt++;
  moveOutsidePillars();
  if (enemies->numInScreen() > 0)
    gameState->mulMultiplier(0.999f);
  if (stageStarted && enemies->num() <= 0)
    startPhase();
}

void Stage::moveOutsidePillars() {
  if (cnt % 120 == 0) {
    Pillar *p = pillars->getInstance();
    if (p)
      p->set(pillarSpec, 180, 0, NULL, outsidePillarShape,
             (((int)cnt / 120) % 2 * 2 - 1) * 0.003f, true);
  }
}

void Stage::draw() {
  if (gameState->mode() != GameState::Mode::MODERN &&
      phaseTime < PHASE_RESULT_SHOW_CNT && phaseNum > 1) {
    Letter::drawString("SHOTS FIRED", 152, 250, 6, Letter::Direction::TO_RIGHT,
                       false, 0, 1, 1, 0.33f);
    Letter::drawNum(shotFiredNumRsl, 480, 250, 6);
    Letter::drawString("NUMBER OF HITS", 152, 280, 6,
                       Letter::Direction::TO_RIGHT, false, 0, 1, 1, 0.33f);
    Letter::drawNum(shotHitNumRsl, 480, 280, 6);
    Letter::drawString("HIT-MISS RATIO", 152, 310, 6);
    Letter::drawNum((int)(hitRatio * 10000), 480, 310, 6, 3, -1, 2);
    Letter::drawString("BONUS", 200, 350, 6, Letter::Direction::TO_RIGHT, false,
                       0, 1, 0.33f, 0.33f);
    Letter::drawNum(hitRatioBonus, 440, 350, 6);
  } else if (phaseTime < (PHASE_RESULT_SHOW_CNT + PHASE_START_SHOW_CNT)) {
    Letter::drawNum(phaseNum, 392, 200, 10);
    Letter::drawString("PHASE", 232, 200, 10);
  }
}

bool Stage::existsCounterBullet() {
  return (_existsCounterBullet && stageStarted &&
          enemies->numBeforeAlign() <= 0);
}
