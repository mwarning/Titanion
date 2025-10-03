/*
 * $Id: enemy.d,v 1.7 2006/12/09 04:17:40 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include <cassert>

#include "enemy.hpp"

#include "abagames/ttn/frame.hpp"

Rand TurretSpec::rand;
Rand EnemySpec::rand;

bool EnemyPool::trailEffect = false;

void TurretSpec::setRankNormal(float rank, bool isWide) {
  initParam();
  float rr = rand.nextFloat(0.5f);
  float nsr = 0.5f + rand.nextSignedFloat(0.3f);
  float nr, br, ir;
  float intervalMax = INTERVAL_MAX;
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
    nr = br = 0;
    ir = sqrt(rank * nsr) * 2;
    burstInterval = 3 + rand.nextInt(2);
    break;
  case GameState::Mode::BASIC:
    if (isWide) {
      nr = (rank * nsr * rr);
      br = 0;
      ir = (rank * nsr * (1 - rr));
    } else {
      nr = 0;
      br = (rank * nsr * rr);
      ir = (rank * nsr * (1 - rr));
    }
    burstInterval = 3 + rand.nextInt(2);
    break;
  case GameState::Mode::MODERN:
    if (isWide) {
      nr = (rank * nsr * rr);
      br = 0;
      ir = (rank * nsr * (1 - rr));
    } else {
      nr = 0;
      br = (rank * nsr * rr);
      ir = (rank * nsr * (1 - rr));
    }
    intervalMax = 120;
    burstInterval = 4 + rand.nextInt(4);
    break;
  default:
    assert(0);
  }
  burstNum = (int)sqrt(br) + 1;
  nway = (int)sqrt(nr) + 1;
  interval = (int)(intervalMax / (ir + 1)) + 1;
  float sr = rank - nway + 1 - burstNum + 1 - ir;
  if (sr < 0.01f)
    sr = 0.01f;
  speed = sqrt(sr * 0.66f);
  assert(speed > 0);
  speed *= 0.2f;
  if (speed < 0.1f)
    speed = 0.1f;
  else
    speed = sqrt(speed * 10) / 10;
  assert(speed > 0);
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
    speed *= 0.36f;
    if (speed < 0.05f)
      speed = 0.05f;
    else
      speed = sqrt(speed * 20) / 20;
    break;
  case GameState::Mode::BASIC:
    speed *= 0.33f;
    break;
  case GameState::Mode::MODERN:
    speed *= 0.25f;
    if (speed < 0.04f)
      speed = 0.04f;
    if (speed > 0.04f)
      speed = sqrt(speed * 25) / 25;
    break;
  }
  nwayAngle = (1.66f + rand.nextFloat(0.33f)) / (1 + nway * 0.7f) * 0.06f;
  fireingAtATime = true;
  minimumFireDist = 10;
}

void TurretSpec::setRankMiddle(float rank) {
  initParam();
  float nr, br, ir;
  float nwayDegRatio;
  float intervalMax = INTERVAL_MAX;
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
    nr = br = 0;
    ir = sqrt(rank * (0.5f + rand.nextSignedFloat(0.3f))) * 2;
    nwayDegRatio = 0;
    burstInterval = 3 + rand.nextInt(2);
    break;
  case GameState::Mode::BASIC:
    if (rand.nextInt(3) == 0) {
      nr = 0;
      br = (rank * 0.4f) * (1.0f + rand.nextSignedFloat(0.2f));
      ir = (rank * 0.5f) * (1.0f + rand.nextSignedFloat(0.2f));
    } else {
      rank *= 0.5f;
      nr = (rank * 0.3f) * (1.0f + rand.nextSignedFloat(0.2f));
      br = (rank * 0.3f) * (1.0f + rand.nextSignedFloat(0.2f));
      ir = (rank * 0.3f) * (1.0f + rand.nextSignedFloat(0.2f));
    }
    ir *= 1.5f;
    nwayDegRatio = 0.06f;
    burstInterval = 3 + rand.nextInt(2);
    break;
  case GameState::Mode::MODERN:
    switch (rand.nextInt(5)) {
    case 0:
      rank *= 1.2f;
      nr = 0;
      br = (rank * 0.7f) * (1.0f + rand.nextSignedFloat(0.2f));
      ir = (rank * 0.2f) * (1.0f + rand.nextSignedFloat(0.2f));
      break;
    case 1:
    case 2:
      nr = (rank * 0.7f) * (1.0f + rand.nextSignedFloat(0.2f));
      br = 0;
      ir = (rank * 0.2f) * (1.0f + rand.nextSignedFloat(0.2f));
      break;
    case 3:
    case 4:
      rank *= 0.75f;
      nr = (rank * 0.3f) * (1.0f + rand.nextSignedFloat(0.2f));
      br = (rank * 0.3f) * (1.0f + rand.nextSignedFloat(0.2f));
      ir = (rank * 0.3f) * (1.0f + rand.nextSignedFloat(0.2f));
      break;
    default:
      assert(0);
    }
    nwayDegRatio = 1;
    intervalMax = 120;
    burstInterval = 4 + rand.nextInt(8);
    break;
  default:
    assert(0);
  }
  bool acf = false;
  burstNum = (int)sqrt(br) + 1;
  if (burstNum > 1 && rand.nextInt(3) > 0) {
    acf = true;
    nr *= 0.9f;
    ir *= 0.9f;
    rank *= 0.9f;
  }
  nway = (int)sqrt(nr) + 1;
  interval = (int)(intervalMax / (sqrt(ir + 1))) + 1;
  float sr = rank - burstNum + 1 - nway + 1 - ir;
  if (sr < 0.01f)
    sr = 0.01f;
  speed = sqrt(sr * 0.66f);
  assert(speed > 0);
  speed *= 0.2f;
  if (speed < 0.1f)
    speed = 0.1f;
  else
    speed = sqrt(speed * 10) / 10;
  assert(speed > 0);
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
    speed *= 0.36f;
    if (speed < 0.05f)
      speed = 0.05f;
    else
      speed = sqrt(speed * 20) / 20;
    break;
  case GameState::Mode::BASIC:
    speed *= 0.4f;
    break;
  case GameState::Mode::MODERN:
    speed *= 0.22f;
    if (speed < 0.04f)
      speed = 0.04f;
    if (speed > 0.04f)
      speed = sqrt(speed * 25) / 25;
    break;
  }
  if (acf) {
    speedAccel = (speed * (0.2f + burstNum * 0.05f + rand.nextFloat(0.1f))) /
                 (burstNum - 1);
    if (rand.nextInt(2) == 0)
      speedAccel *= -1;
  }
  if (gameState->mode() == GameState::Mode::BASIC && nway > 1 &&
      rand.nextInt(3) == 0) {
    speed *= 0.9f;
    nwaySpeedAccel =
        (speed * (0.2f + nway * 0.05f + rand.nextFloat(0.1f))) / (nway - 1);
    if (rand.nextInt(2) == 0)
      nwaySpeedAccel *= -1;
  }
  if (nway > 1)
    nwayAngle =
        (1.66f + rand.nextFloat(0.33f)) / (1 + nway * 0.7f) * nwayDegRatio;
  if (rand.nextInt(3) == 0)
    fireingAtATime = true;
  minimumFireDist = 5;
}

bool TurretSpec::move(TurretState &ts, float time, float anger) {
  if (_disabled)
    return true;
  float itv = interval * ((1 - anger) * 0.99f + 0.01f);
  if (itv < 3)
    itv = 3;
  if (ts.fireCnt > itv)
    ts.fireCnt = itv;
  float spd = speed * (1 + anger * 0.2f);
  if (fireingAtATime) {
    ts.fireCnt -= time;
    if (ts.fireCnt <= 0) {
      ts.fireCnt = itv;
      if (ts.fireCnt < 3)
        ts.fireCnt = 3;
      if (isAbleToFire(ts.pos)) {
        float sp = spd - speedAccel * (burstNum - 1) / 2;
        for (int i = 0; i < burstNum; i++) {
          float d = ts.deg - nwayAngle * (nway - 1) / 2 + nwayBaseDeg;
          float nsp =
              sp - nwaySpeedAccel * ts.nwaySpeedAccelDir * (nway - 1) / 2;
          for (int j = 0; j < nway; j++) {
            Bullet *b = bullets->getInstance();
            if (!b)
              break;
            b->set(bulletSpec, ts.pos, d, nsp * SPEED_RATIO);
            b->setWaitCnt(i * burstInterval);
            d += nwayAngle;
            nsp += nwaySpeedAccel * ts.nwaySpeedAccelDir;
          }
          sp += speedAccel;
        }
        ts.nwaySpeedAccelDir *= -1;
      }
    }
  } else {
    if (ts.burstNum <= 0) {
      ts.fireCnt -= time;
      if (ts.fireCnt <= 0) {
        ts.fireCnt = itv;
        if (ts.fireCnt < 3)
          ts.fireCnt = 3;
        ts.burstNum = burstNum;
        ts.burstCnt = 0;
        ts.speed = spd - speedAccel * (ts.burstNum - 1) / 2;
      }
    }
    if (ts.burstNum > 0) {
      ts.burstCnt -= time;
      if (ts.burstCnt <= 0) {
        ts.burstCnt = burstInterval;
        ts.burstNum--;
        if (isAbleToFire(ts.pos)) {
          float d = ts.deg - nwayAngle * (nway - 1) / 2 + nwayBaseDeg;
          float nsp =
              ts.speed - nwaySpeedAccel * ts.nwaySpeedAccelDir * (nway - 1) / 2;
          for (int i = 0; i < nway; i++) {
            Bullet *b = bullets->getInstance();
            if (!b)
              break;
            b->set(bulletSpec, ts.pos, d, nsp * SPEED_RATIO);
            d += nwayAngle;
            nsp += nwaySpeedAccel * ts.nwaySpeedAccelDir;
          }
        }
        ts.speed += speedAccel;
      }
    }
  }
  return true;
}

bool TurretSpec::isAbleToFire(const Vector &p) {
  if (gameState->mode() != GameState::Mode::MODERN)
    return (p.y > 0);
  else
    return (p.y > 0 && p.dist(player->pos()) > minimumFireDist);
}

bool EnemySpec::move(EnemyState &es) {
  es.move();
  if (isInScreen(es) && es.isFirstEnemy) {
    SoundImpl::playSe("flying_down.wav");
    es.isFirstEnemy = false;
  }
  if (es.captureState > 0) {
    moveCaptured(es);
    return true;
  }
  if (player->enemiesHasCollision()) {
    if (enemies->checkEnemyHit(es.pos, es.size)) {
      destroyed(es);
      return false;
    }
  }
  if (player->checkEnemyHit(es.pos, es.vel, es.size)) {
    destroyed(es);
    return false;
  }
  if (capturable)
    checkCaptured(es);
  float er = (1 - es.ellipseRatio) +
             fabs(sin(es.deg + es.ellipseDeg)) * es.ellipseRatio * 2;
  float rk = rank;
  es.vel.x -= sin(es.deg) * es.speed * er * 0.1f * rk;
  es.vel.y += cos(es.deg) * es.speed * er * 0.1f * rk;
  es.vel *= 0.9f;
  es.pos += es.vel;
  if (isInScreen(es))
    field->addSlowdownRatio(es.speed * 0.04f * rk);
  es.pos.x = field->normalizeX(es.pos.x);
  es.recordTrail();
  if (es.phase >= -50 && es.phase < 0 && !field->containsIncludingPit(es.pos))
    return false;
  if (es.waitCnt > 0) {
    es.waitCnt--;
  } else {
    Vector cp = es.centerPos;
    es.centerPos.x = field->normalizeX(es.centerPos.x);
    es.phaseCnt++;
    if (field->calcCircularDist(es.centerPos, es.pos) < NEXT_PHASE_DIST) {
      es.nextPhaseCnt--;
      if (es.nextPhaseCnt <= 0) {
        es.phase++;
        if (!gotoNextPhase(es))
          return false;
      }
    }
    cp.x = field->normalizeX(cp.x);
    float dst = field->calcCircularDist(cp, es.pos);
    es.speed += ((es.baseSpeed * (1 + dst * 0.1f)) - es.speed) * 0.05f;
    float av = es.angVel * rk;
    float td = atan2(field->normalizeX(-(cp.x - es.pos.x)), cp.y - es.pos.y);
    float ad = Math::normalizeDeg(td - es.deg);
    av *= (2.5f - er);
    if (ad > av || ad < -PI * 0.8f)
      es.deg += av;
    else if (ad < -av)
      es.deg -= av;
    else
      es.deg = td;
    // assert(!isNaN(es.deg));
    for (int i = 0; i < turretNum; i++) {
      TurretState *ts = es.turretStates[i];
      float tx = es.pos.x;
      float ty = es.pos.y;
      switch (i) {
      case 0:
        break;
      case 1:
        tx -= turretWidth;
        break;
      case 2:
        tx += turretWidth;
        break;
      }
      float turretDeg = atan2(field->normalizeX(-(player->pos().x - tx)),
                              player->pos().y - ty);
      switch (gameState->mode()) {
      case GameState::Mode::CLASSIC:
        if (turretDeg >= 0 && turretDeg < PI - PI / 6)
          turretDeg = PI - PI / 6;
        else if (turretDeg < 0 && turretDeg > -PI + PI / 6)
          turretDeg = -PI + PI / 6;
        turretDeg = (int)((turretDeg + PI / 64) / (PI / 32)) * (PI / 32);
        break;
      case GameState::Mode::BASIC:
        if (turretDeg >= 0 && turretDeg < PI - PI / 4)
          turretDeg = PI - PI / 4;
        else if (turretDeg < 0 && turretDeg > -PI + PI / 4)
          turretDeg = -PI + PI / 4;
        break;
      case GameState::Mode::MODERN:
        break;
      }
      ts->update(tx, ty, turretDeg);
    }
    movePhase(es);
    es.sizeVel.x += (es.targetSize.x - es.size.x) * 0.2f;
    es.sizeVel.y += (es.targetSize.y - es.size.y) * 0.2f;
    es.size += es.sizeVel;
    es.sizeVel *= 0.95f;
  }
  return true;
}

void EnemySpec::checkCaptured(EnemyState es) {
  if (player->isInTractorBeam(es.pos)) {
    if (gameState->mode() != GameState::Mode::MODERN) {
      int idx = player->addCapturedEnemy(es.enemy);
      if (idx >= 0) {
        es.captureState = 1;
        es.captureIdx = idx;
      }
    } else {
      provacated(es);
    }
  }
}

bool EnemySpec::hitShot(EnemyState &es, float dd) {
  es.shield--;
  float r = 0.5f + rand.nextFloat(0.5f);
  float g = 0.1f + rand.nextFloat(0.5f);
  float b = 0.5f + rand.nextFloat(0.5f);
  for (int i = 0; i < 10; i++) {
    Particle *p;
    float d;
    p = particles->getInstanceForced();
    d = dd + rand.nextSignedFloat(PI / 4);
    p->set(Particle::Shape::LINE, es.pos.x, es.pos.y, d,
           0.1f + rand.nextFloat(0.5f), 1, r, g, b, 30 + rand.nextInt(30));
    p = particles->getInstanceForced();
    d = dd + PI + rand.nextSignedFloat(PI / 4);
    p->set(Particle::Shape::LINE, es.pos.x, es.pos.y, d,
           0.1f + rand.nextFloat(0.5f), 1, r, g, b, 30 + rand.nextInt(30));
  }
  if (es.shield <= 0) {
    destroyed(es, dd);
    return true;
  }
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
    es.targetSize.x *= 1.3f;
    es.targetSize.y *= 1.3f;
    break;
  case GameState::Mode::BASIC:
    es.targetSize.x *= 1.2f;
    es.targetSize.y *= 1.2f;
    break;
  case GameState::Mode::MODERN:
    es.targetSize.x *= 1.01f;
    es.targetSize.y *= 1.01f;
    break;
  }
  es.sizeVel.x = 0.3f;
  es.sizeVel.y = 0.3f;
  return false;
}

void EnemySpec::destroyed(const EnemyState &es, float dd) {
  float r = 0.5f + rand.nextFloat(0.5f);
  float g = 0.1f + rand.nextFloat(0.5f);
  float b = 0.5f + rand.nextFloat(0.5f);
  float sz = (es.targetSize.x + es.targetSize.y) / 2;
  sz = (sz - 1) * 2 + 1;
  int n = 3 + rand.nextInt(2);
  n *= sz;
  for (int i = 0; i < n; i++) {
    Particle *p = particles->getInstanceForced();
    float d = dd + rand.nextSignedFloat(PI / 5);
    p->set(Particle::Shape::TRIANGLE, es.pos.x, es.pos.y, d, 0.5f,
           (2 + rand.nextFloat(0.5f)) * sz, r, g, b, 50 + rand.nextInt(100));
  }
  for (int i = 0; i < n; i++) {
    Particle *p = particles->getInstanceForced();
    float d = rand.nextFloat(PI * 2);
    p->set(Particle::Shape::QUAD, es.pos.x, es.pos.y, d,
           0.1f + rand.nextFloat(0.1f), (1 + rand.nextFloat(0.5f)) * sz, r, g,
           b, 50 + rand.nextInt(100));
  }
  if (!isBeingCaptured(es)) {
    if (removeBullets) {
      int cnt = 1;
      bullets->removeAround(cnt, es.pos, particles, bonusParticles, player);
      Particle *p = bonusParticles->getInstanceForced();
      int wc;
      if (cnt <= 50)
        wc = cnt;
      else
        wc = 50 + (int)sqrt((float)(cnt - 50));
      p->set(Particle::Shape::BONUS, es.pos.x, es.pos.y, 0, 0.1f,
             1.0f + (float)wc / 75, 1, 1, 1, 120, false, cnt, wc);
      player->addScore(score * cnt);
    } else {
      if (gameState->mode() == GameState::Mode::BASIC) {
        float oy = es.pos.y - player->pos().y;
        int pm = (int)(18 - oy);
        if (pm > 16)
          pm = 16;
        else if (pm < 1)
          pm = 1;
        player->addScore(score * pm);
        Particle *p = bonusParticles->getInstanceForced();
        p->set(Particle::Shape::BONUS, es.pos.x, es.pos.y, 0, 0.1f, 0.5f, 1, 1,
               1, 60, false, pm);
        gameState->setProximityMultiplier(pm);
      } else {
        player->addScore(score);
      }
    }
    player->addMultiplier(0.1f);
    if (stage->existsCounterBullet()) {
      Bullet *blt = bullets->getInstance();
      if (blt)
        blt->set(counterBulletSpec, es.pos, es.turretStates[0]->deg,
                 turretSpecs[0]->speed * TurretSpec::SPEED_RATIO);
    }
  }
  SoundImpl::playSe(explosionSeName);
}

void EnemySpec::provacated(EnemyState &es) {
  es.anger += (1 - es.anger) * 0.05f;
  if (es.sizeVel.dist() < 0.1f) {
    es.sizeVel.x = 0.2f;
    es.sizeVel.y = 0.2f;
  }
  Particle *p;
  p = particles->getInstanceForced();
  p->set(Particle::Shape::LINE, es.pos.x, es.pos.y,
         PI / 2 + rand.nextSignedFloat(PI / 4), 0.1f + rand.nextFloat(0.2f), 1,
         1, 0.5f, 0.5f, 30 + rand.nextInt(30));
  p = particles->getInstanceForced();
  p->set(Particle::Shape::LINE, es.pos.x, es.pos.y,
         -PI / 2 + rand.nextSignedFloat(PI / 4), 0.1f + rand.nextFloat(0.2f), 1,
         1, 0.5f, 0.5f, 30 + rand.nextInt(30));
  if (removeBullets)
    player->midEnemyProvacated();
}

bool EnemySpec::gotoNextPhaseInAppearing(EnemyState &es) {
  switch (es.phase) {
  case -300: {
    float cpw;
    switch (gameState->mode()) {
    case GameState::Mode::CLASSIC:
    case GameState::Mode::BASIC:
      cpw = 0.2f;
      break;
    case GameState::Mode::MODERN:
      cpw = 0.4f;
      break;
    default:
      assert(0);
    }
    es.centerPos.x = rand.nextSignedFloat(field->size().x * cpw);
    es.centerPos.y = field->size().y * 2.0f;
    es.standByPos.x = rand.nextSignedFloat(field->size().x * cpw);
    es.standByPos.y = field->size().y * (0.7f + rand.nextFloat(0.1f));
    es.nextPhaseCnt = 15;
    es.baseSpeed = es.baseBaseSpeed * 1.5f;
    es.angVel = es.baseAngVel * 1.5f;
    es.phase = -50;
    break;
  }
  case -200:
    es.centerPos.x = rand.nextSignedFloat(field->size().x * 0.1f);
    es.centerPos.y = field->size().y * 1.6f;
    if (es.centerPos.x < 0)
      es.standByPos.x = field->size().x * (rand.nextSignedFloat(0.4f) + 0.4f);
    else
      es.standByPos.x = field->size().x * (rand.nextSignedFloat(0.4f) - 0.4f);
    es.standByPos.y = field->size().y * (0.5f + rand.nextFloat(0.3f));
    es.nextPhaseCnt = 60;
    es.baseSpeed = es.baseBaseSpeed * 1.0f;
    es.angVel = es.baseAngVel * 1.0f;
    break;
  case -199:
    if (es.standByPos.x < 0)
      es.centerPos.x = field->size().x * 0.75f;
    else
      es.centerPos.x = -field->size().x * 0.75f;
    es.centerPos.y = 0;
    if (es.isGoingDownBeforeStandBy)
      es.nextPhaseCnt = 20;
    else
      es.nextPhaseCnt = 60;
    es.baseSpeed = es.baseBaseSpeed * 2;
    es.angVel = es.baseAngVel * 2;
    es.phase = -50;
    break;

  case -100:
    es.centerPos.x = field->size().x * 4.0f;
    if (rand.nextInt(2) == 0)
      es.centerPos.x *= -1;
    es.centerPos.y = field->size().y * 1.6f;
    if (es.centerPos.x < 0)
      es.standByPos.x = field->size().x * (rand.nextSignedFloat(0.4f) + 0.4f);
    else
      es.standByPos.x = field->size().x * (rand.nextSignedFloat(0.4f) - 0.4f);
    es.standByPos.y = field->size().y * (0.5f + rand.nextFloat(0.3f));
    es.nextPhaseCnt = 20;
    es.baseSpeed = es.baseBaseSpeed * 2.0f;
    es.angVel = es.baseAngVel * 2.0f;
    break;
  case -99:
    if (es.centerPos.x > 0)
      es.centerPos.x = field->size().x * 2.0f;
    else
      es.centerPos.x = -field->size().x * 2.0f;
    es.centerPos.y = -field->size().y * 1.2f;
    es.nextPhaseCnt = 20;
    es.baseSpeed = es.baseBaseSpeed * 2;
    es.angVel = es.baseAngVel * 2;
    break;
  case -98:
    if (es.centerPos.x > 0)
      es.centerPos.x = field->size().x * 0.5f;
    else
      es.centerPos.x = -field->size().x * 0.5f;
    es.centerPos.y = 0;
    es.nextPhaseCnt = 30;
    es.phase = -50;
    break;

  case -49:
    if (es.isGoingDownBeforeStandBy) {
      es.centerPos.x = es.centerPos.x / 2;
      es.centerPos.y = 0;
      es.phase = -30;
      es.nextPhaseCnt = 10;
      break;
    }
    es.centerPos.x = es.standByPos.x;
    es.centerPos.y = es.standByPos.y;
    es.nextPhaseCnt = calcStandByTime(es);
    es.baseSpeed = es.baseBaseSpeed;
    es.angVel = es.baseAngVel;
    es.phase = -10;
    break;

  case -29:
    es.centerPos.x = (es.centerPos.x + player->pos().x * 2) / 3;
    es.centerPos.y = -field->size().y * 1.2f;
    es.baseSpeed = es.baseBaseSpeed * 1.2f;
    es.angVel = es.baseAngVel * 1.2f;
    es.nextPhaseCnt = 5;
    break;
  case -28:
    es.centerPos.y = -field->size().y * 1.5f;
    es.nextPhaseCnt = 10;
    break;
  case -9:
    es.phase = 0;
    break;
  default:
    return false;
  }
  es.nextPhaseCnt /= rank;
  es.phaseCnt = 0;
  return true;
}

void EnemySpec::movePhase(EnemyState &es) {
  switch (es.phase) {
  case -200:
  case -100:
    if (es.pos.y < field->size().y * 1.5f)
      es.pos.y = field->size().y * 1.5f;
    break;
  case -99:
    if (es.centerPos.x < 0 && es.pos.x > -field->size().x)
      es.pos.x += (-field->size().x - es.pos.x) * 0.2f;
    else if (es.centerPos.x > 0 && es.pos.x < field->size().x)
      es.pos.x += (field->size().x - es.pos.x) * 0.2f;
    break;
  case -50:
  case -49:
  case -10:
    if (es.pos.y < -field->size().y * 0.5f)
      es.pos.y += (-field->size().y * 0.5f - es.pos.y) * 0.2f;
    break;
  default:
    break;
  }
  if (isInAttack(es))
    if (gameState->mode() == GameState::Mode::MODERN || es.phase >= 0 ||
        rand.nextInt(5) == 0)
      for (int i = 0; i < turretNum; i++)
        turretSpecs[i]->move(*es.turretStates[i], rank, es.anger);
}

void EnemySpec::draw(const EnemyState &es) {
  Vector3 p = field->calcCircularPos(es.pos);
  float cd = field->calcCircularDeg(es.pos.x);
  dynamic_cast<EnemyShape *>(shape)->draw(p, cd, es.deg, es.cnt, es.size);
  for (int i = 1; i < turretNum; i++) {
    float x = es.pos.x;
    switch (i) {
    case 1:
      x -= turretWidth;
      break;
    case 2:
      x += turretWidth;
      break;
    }
    p = field->calcCircularPos(x, es.pos.y);
    cd = field->calcCircularDeg(x);
    Screen3D::setColor(0.5f, 0.5f, 1);
    dynamic_cast<EnemyShape *>(trailShape)
        ->draw(p, cd, es.deg, es.cnt, es.size.x * 0.5f, es.size.y * 0.5f);
  }
}

void EnemySpec::drawTrails(const EnemyState &es) {
  if (es.captureState > 0)
    return;
  es.drawTrails(trailShape, 0.2f, 0.2f, 0.8f, es.size, field);
}

void EnemySpec::moveCaptured(EnemyState &es) {
  switch (es.captureState) {
  case 1:
    es.vel.x += (player->pos().x - es.pos.x) * 0.03f;
    es.vel.y += (player->pos().y - es.pos.y) * 0.03f;
    es.pos.x += (player->pos().x - es.pos.x) * 0.03f;
    es.pos.y += (player->pos().y - es.pos.y) * 0.03f;
    es.deg *= 0.95f;
    if (player->pos().dist(es.pos) < 1)
      es.captureState = 2;
    break;
  case 2: {
    float cx = calcCapturePosX(es.captureIdx);
    es.vel.x += (player->pos().x + cx - es.pos.x) * 0.03f;
    es.pos.x += (player->pos().x + cx - es.pos.x) * 0.1f;
    es.pos.y += (player->pos().y - es.pos.y) * 0.33f;
    es.vel.y *= 0.6f;
    es.deg *= 0.95f;
    if (fabs(player->pos().x + cx - es.pos.x) < 0.2f)
      es.captureState = 3;
    break;
  }
  case 3: {
    float cx = calcCapturePosX(es.captureIdx);
    es.pos.x = player->pos().x + cx;
    es.pos.y = player->pos().y;
    es.deg = player->deg();
    break;
  }
  }
  es.vel *= 0.9f;
  es.pos += es.vel;
}

float EnemySpec::calcCapturePosX(int idx) {
  if (idx % 2 == 0)
    return ((idx / 2) + 0.5f) * PlayerSpec::CAPTURED_ENEMIES_INTERVAL_LENGTH *
           player->capturedEnemyWidth();
  else
    return -((idx / 2) + 0.5f) * PlayerSpec::CAPTURED_ENEMIES_INTERVAL_LENGTH *
           player->capturedEnemyWidth();
}

void EnemySpec::hitCaptured(const EnemyState &es) {
  player->destroyCapturedEnemies(es.captureIdx);
}

void Enemy::setSmallEnemyState(float baseSpeed, float angVel, int waitCnt,
                               int appPattern, float er, float ed, bool gd,
                               float fireIntervalRatio, Enemy *firstEnemy) {
  state->baseBaseSpeed = state->baseSpeed = baseSpeed;
  state->baseAngVel = state->angVel = angVel;
  state->waitCnt = waitCnt;
  state->ellipseRatio = er;
  state->ellipseDeg = ed;
  state->isGoingDownBeforeStandBy = gd;
  switch (appPattern) {
  case 0:
    state->phase = -200;
    break;
  case 1:
    state->phase = -100;
    break;
  }
  if (firstEnemy) {
    dynamic_cast<SmallEnemySpec *>(spec)->init(*state, *firstEnemy->state);
    state->isFirstEnemy = false;
  } else {
    spec->init(*state);
    state->isFirstEnemy = true;
  }
}

void Enemy::hitCaptured() {
  SmallEnemySpec *ses = dynamic_cast<SmallEnemySpec *>(spec);
  if (ses)
    ses->hitCaptured(*state);
}

bool Enemy::isCaptured() {
  GhostEnemySpec *ges = dynamic_cast<GhostEnemySpec *>(spec);
  if (ges) {
    return true;
  }
  SmallEnemySpec *ses = dynamic_cast<SmallEnemySpec *>(spec);
  if (!ses) {
    return false;
  }
  return ses->isCaptured(*state);
}

Enemy *EnemyPool::getNearestEnemy(const Vector &p) {
  float dst = 99999;
  Enemy *ne = NULL;
  for (auto e : actors) {
    if (e->exists() && !e->isBeingCaptured())
      if (_field->calcCircularDist(e->pos(), p) < dst) {
        dst = _field->calcCircularDist(e->pos(), p);
        ne = e;
      }
  }
  return ne;
}

Enemy *EnemyPool::getNearestMiddleEnemy(const Vector &p) {
  float dst = 99999;
  Enemy *ne = NULL;
  for (auto e : actors) {
    if (e->exists() && !e->isBeingCaptured())
      if (dynamic_cast<MiddleEnemySpec *>(e->spec))
        if (_field->calcCircularDist(e->pos(), p) < dst) {
          dst = _field->calcCircularDist(e->pos(), p);
          ne = e;
        }
  }
  return ne;
}

bool EnemyPool::checkMiddleEnemyExists(float x, float px) {
  for (auto e : actors) {
    if (e->exists() && !e->isBeingCaptured())
      if (dynamic_cast<MiddleEnemySpec *>(e->spec))
        if ((e->pos().x - x) * (e->pos().x - px) < 0)
          return true;
  }
  return false;
}

MiddleEnemySpec::MiddleEnemySpec(Field *field, BulletPool *bullets,
                                 Player *player, ParticlePool *particles,
                                 ParticlePool *bonusParticles,
                                 EnemyPool *enemies, Stage *stage, Shape *shape,
                                 EnemyShape *trailShape, BulletSpec *bulletSpec,
                                 BulletSpec *counterBulletSpec,
                                 GameState *gameState)
    : EnemySpec(field, bullets, player, particles, bonusParticles, enemies,
                stage, shape, trailShape, bulletSpec, counterBulletSpec,
                gameState) {
  for (size_t i = 0; i < TURRET_MAX_NUM; i++) {
    turretSpecs[i] = new TurretSpec(field, bullets, player, enemies, particles,
                                    stage, bulletSpec, gameState);
  }
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
    shield = 2;
    capturable = false;
    removeBullets = false;
    break;
  case GameState::Mode::BASIC:
    shield = 3;
    capturable = false;
    removeBullets = false;
    break;
  case GameState::Mode::MODERN:
    shield = 32;
    capturable = true;
    removeBullets = true;
    break;
  }
  score = 100;
  explosionSeName = "explosion3.wav";
}

void MiddleEnemySpec::setRank(float r) {
  rank = sqrt(r);
  float tr;
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
    rank = sqrt(rank);
    tr = r * 2;
    break;
  case GameState::Mode::BASIC:
    tr = r * 3;
    break;
  case GameState::Mode::MODERN:
    rank = 1;
    tr = r * 15;
    break;
  default:
    assert(0);
  }
  if (rank < 1.5f)
    rank = 1.5f;
  turretSpecs[0]->setRankMiddle(tr);
  turretNum = 1;
  if (gameState->mode() == GameState::Mode::MODERN) {
    TurretSpec *ts = turretSpecs[0];
    int ptn = rand.nextInt(6);
    switch (ptn) {
    case 0:
      break;
    case 1:
    case 2:
    case 4:
      turretSpecs[1]->copy(*turretSpecs[0]);
      turretSpecs[2]->copy(*turretSpecs[0]);
      if (ts->nway > 1 && rand.nextInt(2) == 0) {
        float nsa =
            (ts->speed * (0.2f + ts->nway * 0.05f + rand.nextFloat(0.1f))) /
            (ts->nway - 1);
        if (rand.nextInt(2) == 0)
          nsa *= -1;
        turretSpecs[1]->nwaySpeedAccel = nsa;
        turretSpecs[2]->nwaySpeedAccel = -nsa;
      }
      turretWidth = 1.0f + rand.nextFloat(1.0f);
      turretNum = 3;
      if (ptn == 4) {
        turretSpecs[0]->setRankMiddle(tr * 2);
        turretSpecs[1]->interval *= 4;
        turretSpecs[2]->interval *= 4;
        turretSpecs[0]->interval = turretSpecs[1]->interval;
        turretSpecs[2]->fireIntervalRatio = 0.25f;
        turretSpecs[0]->fireIntervalRatio = 0.5f;
      } else {
        turretSpecs[0]->disabled(true);
        turretSpecs[1]->interval *= 2;
        turretSpecs[2]->interval *= 2;
        if (rand.nextInt(2) == 0)
          turretSpecs[2]->fireIntervalRatio = 0.5f;
      }
      break;
    case 3:
    case 5:
      turretSpecs[0]->interval *= 2;
      if (rand.nextInt(3) == 0)
        turretSpecs[0]->nwayAngle *= 0.1f;
      turretSpecs[1]->setRankMiddle(tr);
      turretSpecs[1]->interval *= 2;
      turretSpecs[2]->copy(*turretSpecs[1]);
      if (ts->nway > 1 && rand.nextInt(2) == 0) {
        float nsa =
            (ts->speed * (0.2f + ts->nway * 0.05f + rand.nextFloat(0.1f))) /
            (ts->nway - 1);
        if (rand.nextInt(2) == 0)
          nsa *= -1;
        turretSpecs[1]->nwaySpeedAccel = nsa;
        turretSpecs[2]->nwaySpeedAccel = -nsa;
      }
      turretSpecs[1]->nwayBaseDeg = -PI / 8 - rand.nextFloat(PI / 12);
      if (turretSpecs[1]->nway > 1)
        turretSpecs[1]->nwayBaseDeg -=
            turretSpecs[1]->nwayAngle * (turretSpecs[1]->nway - 1) / 2;
      turretSpecs[2]->nwayBaseDeg = -turretSpecs[1]->nwayBaseDeg;
      turretWidth = 1.5f + rand.nextFloat(0.5f);
      turretNum = 3;
      break;
    }
  }
}

bool MiddleEnemySpec::gotoNextPhase(EnemyState &es) {
  if (es.phase < 0)
    return gotoNextPhaseInAppearing(es);
  switch (es.phase) {
  case 1:
    if (gameState->mode() != GameState::Mode::MODERN &&
        !player->hasCollision()) {
      es.phase = 0;
      es.nextPhaseCnt = calcStandByTime(es);
      break;
    }
    SoundImpl::playSe("flying_down.wav");
    if (gameState->mode() != GameState::Mode::MODERN) {
      es.centerPos.x = field->size().x * (0.6f + rand.nextSignedFloat(0.1f));
      if (rand.nextInt(2) == 0)
        es.centerPos.x *= -1;
      es.centerPos.y = field->size().y * (0.2f + rand.nextFloat(0.2f));
      es.nextPhaseCnt = 60;
    } else {
      es.centerPos.x = es.standByPos.x;
      es.centerPos.y = field->size().y * 0.95f;
      es.baseSpeed = es.baseBaseSpeed * 0.3f;
      es.nextPhaseCnt = 60;
    }
    break;
  case 2:
    if (gameState->mode() != GameState::Mode::MODERN) {
      es.centerPos.x *= -0.9f;
      es.centerPos.y = field->size().y * (0.2f + rand.nextFloat(0.2f));
      es.nextPhaseCnt = 60;
    } else {
      es.centerPos.x = es.standByPos.x;
      es.centerPos.y = 0;
      es.baseSpeed = es.baseBaseSpeed * 0.1f;
      es.nextPhaseCnt = 10;
    }
    break;
  case 3:
    if (gameState->mode() != GameState::Mode::MODERN) {
      es.centerPos.x = es.standByPos.x;
      es.centerPos.y = es.standByPos.y;
      es.phase = 0;
      es.nextPhaseCnt = calcStandByTime(es);
    } else {
      es.centerPos.x = es.standByPos.x;
      es.centerPos.y = -field->size().y * 1.5f;
      es.baseSpeed = es.baseBaseSpeed * 0.5f;
      es.nextPhaseCnt = 10;
    }
    break;
  default:
    return false;
  }
  es.nextPhaseCnt /= rank;
  es.phaseCnt = 0;
  return true;
}

int MiddleEnemySpec::calcStandByTime(const EnemyState &es) {
  if (es.phase < 0 || gameState->mode() == GameState::Mode::MODERN)
    return 30 + rand.nextInt(30);
  else
    return 200 + rand.nextInt(150);
}

SmallEnemySpec::SmallEnemySpec(Field *field, BulletPool *bullets,
                               Player *player, ParticlePool *particles,
                               ParticlePool *bonusParticles, EnemyPool *enemies,
                               Stage *stage, Shape *shape,
                               EnemyShape *trailShape, BulletSpec *bulletSpec,
                               BulletSpec *counterBulletSpec,
                               GameState *gameState)
    : EnemySpec(field, bullets, player, particles, bonusParticles, enemies,
                stage, shape, trailShape, bulletSpec, counterBulletSpec,
                gameState) {
  turretSpecs[0] = new TurretSpec(field, bullets, player, enemies, particles,
                                  stage, bulletSpec, gameState);
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
  case GameState::Mode::BASIC:
    shield = 1;
    break;
  case GameState::Mode::MODERN:
    shield = 2;
    break;
  }
  capturable = true;
  score = 10;
  removeBullets = false;
}

void SmallEnemySpec::setRank(float r) {
  rank = sqrt(r * 0.5f);
  float tr;
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
    rank = sqrt(rank);
    tr = r;
    break;
  case GameState::Mode::BASIC:
    tr = r * 2;
    break;
  case GameState::Mode::MODERN:
    rank = 1;
    tr = r;
    break;
  default:
    assert(0);
  }
  if (rank < 1)
    rank = 1;
  turretSpecs[0]->setRankNormal(tr);
  turretNum = 1;
}

bool SE1Spec::gotoNextPhase(EnemyState &es) {
  if (es.phase < 0)
    return gotoNextPhaseInAppearing(es);
  switch (es.phase) {
  case 1:
    if (!player->hasCollision() ||
        enemies->numInAttack() > stage->attackSmallEnemyNum()) {
      es.phase = 0;
      es.nextPhaseCnt = calcStandByTime(es);
      break;
    }
    SoundImpl::playSe("flying_down.wav");
    es.centerPos.y = 0;
    es.centerPos.x = (es.standByPos.x + player->pos().x) / 2;
    es.nextPhaseCnt = 60;
    break;
  case 2:
    es.centerPos.y = -field->size().y * 0.7f;
    es.centerPos.x = player->pos().x;
    es.nextPhaseCnt = 30;
    break;
  case 3:
    es.centerPos.x = es.standByPos.x;
    es.centerPos.y = es.standByPos.y;
    es.phase = 0;
    es.nextPhaseCnt = calcStandByTime(es);
    break;
  }
  es.nextPhaseCnt /= rank;
  es.phaseCnt = 0;
  return true;
}

bool SE2Spec::gotoNextPhase(EnemyState &es) {
  if (es.phase < 0)
    return gotoNextPhaseInAppearing(es);
  switch (es.phase) {
  case 1:
    if (!player->hasCollision() ||
        enemies->numInAttack() > stage->attackSmallEnemyNum()) {
      es.phase = 0;
      es.nextPhaseCnt = calcStandByTime(es);
      break;
    }
    SoundImpl::playSe("flying_down.wav");
    es.centerPos.y = -field->size().y * 0.3f;
    es.centerPos.x = (es.standByPos.x + player->pos().x) / 2;
    es.baseSpeed = es.baseBaseSpeed;
    es.angVel = es.baseAngVel;
    es.nextPhaseCnt = 30 + rand.nextInt(60);
    break;
  case 2:
    es.centerPos.y = -field->size().y * 1.3f;
    es.centerPos.x *= -1;
    es.nextPhaseCnt = 30;
    break;
  case 3:
    es.centerPos.y = -field->size().y * 1.0f;
    if (es.centerPos.x < 0)
      es.centerPos.x = -field->size().x * 1.5f;
    else
      es.centerPos.x = field->size().x * 1.5f;
    es.baseSpeed = es.baseBaseSpeed * 1.5f;
    es.angVel = es.baseAngVel * 1.5f;
    es.nextPhaseCnt = 30;
    break;
  case 4:
    es.centerPos.x = es.standByPos.x;
    es.centerPos.y = es.standByPos.y;
    es.phase = 0;
    es.nextPhaseCnt = calcStandByTime(es);
    break;
  }
  es.nextPhaseCnt /= rank;
  es.phaseCnt = 0;
  return true;
}

EnemyState::EnemyState() {
  for (size_t i = 0; i < TURRET_MAX_NUM; i++) {
    turretStates[i] = new TurretState();
  }

  for (size_t i = 0; i < TRAIL_NUM; i++) {
    trails[i] = new Trail();
  }
}

void EnemyState::clear() {
  for (auto ts : turretStates)
    ts->clear();
  vel.x = vel.y = 0;
  centerPos.x = centerPos.y = 0;
  centerVel.x = centerVel.y = 0;
  standByPos.x = standByPos.y = 0;
  baseBaseSpeed = baseSpeed = 0;
  baseAngVel = angVel = 0;
  waitCnt = 0;
  cnt = 0;
  ellipseRatio = 0;
  ellipseDeg = 0;
  shield = 0;
  phase = 0;
  phaseCnt = nextPhaseCnt = 0;
  captureState = 0;
  captureIdx = 0;
  isGoingDownBeforeStandBy = false;
  size.x = size.y = 1;
  targetSize.x = targetSize.y = 1;
  sizeVel.x = sizeVel.y = 0;
  trailIdx = 0;
  trailLooped = false;
  isFirstEnemy = false;
  anger = 0;
  TokenState::clear();
}

void EnemyState::recordTrail() {
  trails[trailIdx]->set(pos.x, pos.y, deg, cnt);
  trailIdx++;
  if (trailIdx >= TRAIL_NUM) {
    trailIdx = 0;
    trailLooped = true;
  }
}

void EnemyState::drawTrails(EnemyShape *s, float r, float g, float b,
                            Vector size, Field *field) const {
  int ti = trailIdx;
  float a = 1.0f;
  for (int i = 0; i < TRAIL_NUM / TRAIL_INTERVAL; i++) {
    ti -= TRAIL_INTERVAL;
    if (ti < 0) {
      if (trailLooped)
        ti += TRAIL_NUM;
      else
        break;
    }
    Trail *t = trails[ti];
    Screen3D::setColor(r * a, g * a, b * a, a * 0.66f);
    Vector3 p = field->calcCircularPos(t->pos);
    float cd = field->calcCircularDeg(t->pos.x);
    s->draw(p, cd, t->deg, t->cnt, size);
    a *= 0.7f;
  }
}
