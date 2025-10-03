/*
 * $Id: player.d,v 1.4 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include "player.hpp"

#include "abagames/ttn/frame.hpp"

Rand PlayerSpec::rand;

bool Player::checkEnemyHit(const Vector &p, const Vector &v, Vector size) {
  if (spec->gameState->mode() == GameState::Mode::MODERN)
    return false;
  if (!state->hasCollision())
    return false;
  if (fabs(state->pos.x - p.x) < size.x && fabs(state->pos.y - p.y) < size.y) {
    switch (spec->gameState->mode()) {
    case GameState::Mode::CLASSIC:
      destroy();
      break;
    case GameState::Mode::BASIC:
      hitOffset.x = state->pos.x - p.x;
      hitOffset.y = state->pos.y - p.y;
      spec->addVelocity(*state, v, hitOffset);
      break;
    case GameState::Mode::MODERN:
      assert(0); // not reachable
    }
    return true;
  }
  return false;
}

void Player::drawState() {
  if (spec->gameState->mode() == GameState::Mode::CLASSIC)
    spec->drawState(*state);
}

bool Player::enemiesHasCollision() {
  switch (spec->gameState->mode()) {
  case GameState::Mode::CLASSIC:
    return state->hasCollision();
  case GameState::Mode::BASIC:
    return true;
  case GameState::Mode::MODERN:
    return false;
  default:
    // should never happen
    assert(0);
  }
}

bool Player::checkBulletHit(const Vector &p, const Vector &pp) {
  if (!state->hasCollision())
    return false;
  if (spec->field->checkHitDist(state->pos, p, pp, spec->bulletHitWidth)) {
    destroy();
    return true;
  }
  return false;
}

bool TractorBeam::contains(const Vector &p) {
  if (length <= 0)
    return false;
  return (p.x > playerState->pos.x - WIDTH / 2 &&
          p.x < playerState->pos.x + WIDTH / 2 && p.y > playerState->pos.y &&
          p.y < playerState->pos.y + length + WIDTH);
}

void TractorBeam::draw() {
  if (length <= 0)
    return;
  float y = SHAPE_INTERVAL_LENGTH - fmod(cnt, SHAPE_INTERVAL_TIME) *
                                        SHAPE_INTERVAL_LENGTH /
                                        SHAPE_INTERVAL_TIME;
  int c = (int)(cnt / SHAPE_INTERVAL_TIME);
  for (;;) {
    if (y > length)
      break;
    glPushMatrix();
    Vector3 p =
        field->calcCircularPos(playerState->pos.x, playerState->pos.y + y);
    Screen3D::glTranslate(p);
    float s = y;
    if (s > 1)
      s = 1;
    glScalef(s, s, s);
    switch (gameState->mode()) {
    case GameState::Mode::CLASSIC:
    case GameState::Mode::BASIC:
      shapes[c % 3]->draw();
      break;
    case GameState::Mode::MODERN:
      if (playerState->midEnemyProvacated)
        shapes[c % 3]->draw();
      else
        shapes[c % 3 + 3]->draw();
      break;
    }
    c++;
    glPopMatrix();
    y += SHAPE_INTERVAL_LENGTH;
  }
}

PlayerState::PlayerState() {
  for (int i = 0; i < PlayerState::MAX_CAPTURED_ENEMIES_NUM; ++i) {
    capturedEnemies.push_back(new Enemy());
  }
}

void PlayerState::destroyCapturedEnemies(int idx) {
  for (int i = idx; i < capturedEnemyNum; i++)
    if (capturedEnemies[i]->exists())
      capturedEnemies[i]->destroyed();
  capturedEnemyNum = idx;
}

void PlayerState::reset() {
  float x = pos.x;
  clear();
  pos.x = x;
  pos.y = -10.0f;
  speed = PlayerSpec::BASE_SPEED;
  invincibleCnt = INVINCIBLE_INTERVAL_RESPAWN;
  isInvincible = true;
  isFirstShot = true;
  captureBeamEnergy = 1;
  spec->respawn(*this);
}

PlayerSpec::PlayerSpec(Pad *pad, GameState *gameState, Field *field,
                       EnemyPool *enemies, BulletPool *bullets,
                       ParticlePool *particles) {
  this->pad = dynamic_cast<RecordablePad *>(pad);
  this->gameState = gameState;
  this->field = field;
  this->enemies = enemies;
  this->bullets = bullets;
  this->particles = particles;
  shots = new ShotPool();
  shots->init(16);
  capturedEnemiesShots = new ShotPool();
  capturedEnemiesShots->init(64);
  shotSpec = new ShotSpec(field, enemies, bullets, gameState);
  shape = new PlayerShape();
  lineShape = new PlayerLineShape();
  ghostEnemyShape = new Enemy1TrailShape();
  ghostEnemySpec = new GhostEnemySpec(field, ghostEnemyShape);
}

void PlayerSpec::start() {
  clear();
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
    bulletHitWidth = 0.4f;
    shotMaxNum = 3;
    break;
  case GameState::Mode::BASIC:
    bulletHitWidth = 0.2f;
    shotMaxNum = 3;
    break;
  case GameState::Mode::MODERN:
    bulletHitWidth = 0.1f;
    shotMaxNum = 16;
    break;
  }
}

bool PlayerSpec::move(PlayerState &ps) {
  PadState *input;
  if (!ps.replayMode) {
    input = pad->getState();
  } else {
    try {
      input = pad->replay();
    } catch (NoRecordDataException &e) {
      gameState->startGameOverWithoutRecording();
      input = pad->getNullState();
    }
  }
  shots->move();
  capturedEnemiesShots->move();
  capturedEnemiesShots->checkParent();
  if (gameState->isGameOver()) {
    if (input->button & PadState::Button::A) {
      if (!ps.aPressed) {
        ps.aPressed = true;
        if (!ps.replayMode)
          gameState->backToTitle();
      }
    } else {
      ps.aPressed = false;
    }
    return true;
  }
  ps.move();
  if (!ps.isActive())
    return true;
  float vx = 0, vy = 0;
  if (input->dir & PadState::Dir::RIGHT)
    vx = 1;
  else if (input->dir & PadState::Dir::LEFT)
    vx = -1;
  if (input->dir & PadState::Dir::UP)
    vy = 1;
  else if (input->dir & PadState::Dir::DOWN)
    vy = -1;
  if (vx != 0 && vy != 0) {
    vx *= 0.7f;
    vy *= 0.7f;
  }
  // float px = ps.pos.x;
  ps.pos.x += (vx * ps.speed);
  if (gameState->mode() == GameState::Mode::CLASSIC)
    vy *= 0.5f;
  ps.pos.y += (vy * ps.speed);
  if (!(input->button & PadState::Button::B))
    ps.deg += (-TILT_DEG * (vx * ps.speed) - ps.deg) * 0.1f;
  // assert(!isNaN(ps.deg));
  ps.pos += ps.vel;
  ps.vel *= 0.9f;
  if (gameState->mode() == GameState::Mode::MODERN) {
    float d = ps.ghostCnt * 0.05f;
    for (int i = 0; i < ps.capturedEnemyNum; i++) {
      Enemy *e = ps.capturedEnemies[i];
      e->setGhostEnemyState(ps.pos.x + sin(d) * ps.capturedEnemyWidth * 2,
                            ps.pos.y, ps.deg, (int)(d * 180 / PI / 3));
      d += PI / 2;
    }
  }
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
    /*if (input->button & PadState::Button::A) {
      if (!aPressed) {
        aPressed = true;
        if (!captureBeamReleased)
          fireShot(ps);
      }
    } else {
      aPressed = false;
    }*/
    if ((input->button & PadState::Button::A) && !ps.captureBeamReleased) {
      if (ps.shotCnt <= 0)
        fireShot(ps);
    } else {
      ps.isFirstShot = true;
    }
    break;
  case GameState::Mode::BASIC:
    if ((input->button & PadState::Button::A) &&
        !(input->button & PadState::Button::B)) {
      if (ps.shotCnt <= 0)
        fireShot(ps);
    } else {
      ps.isFirstShot = true;
    }
    break;
  case GameState::Mode::MODERN:
    if (input->button & PadState::Button::A) {
      if (ps.shotCnt <= 0)
        fireShot(ps);
    } else {
      ps.isFirstShot = true;
    }
    break;
  }
  if (input->button & PadState::Button::B) {
    ps.speed += (BASE_SPEED * 1.2f - ps.speed) * 0.33f;
    ps.deg *= 0.9f;
    if (gameState->mode() == GameState::Mode::MODERN) {
      ps.capturedEnemyWidth -= 0.05f;
      if (ps.capturedEnemyWidth < 0.2f)
        ps.capturedEnemyWidth = 0.2f;
    }
  } else {
    ps.speed += (BASE_SPEED * 2.0f - ps.speed) * 0.33f;
    if (gameState->mode() == GameState::Mode::MODERN) {
      ps.capturedEnemyWidth += 0.05f;
      if (ps.capturedEnemyWidth > 1)
        ps.capturedEnemyWidth = 1;
    }
  }
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
    if (input->button & PadState::Button::B && !ps.captureBeamReleased &&
        ps.captureBeamEnergy >= 1 &&
        ps.capturedEnemyNum < PlayerState::MAX_CAPTURED_ENEMIES_NUM) {
      ps.captureBeamReleased = true;
      ps.isInvincible = true;
      ps.invincibleCnt = 99999;
    }
    if (ps.captureBeamReleased) {
      if (ps.captureBeamEnergy <= 0 ||
          ps.capturedEnemyNum >= PlayerState::MAX_CAPTURED_ENEMIES_NUM) {
        ps.captureBeamEnergy = 0;
        if (tractorBeam->reduceLength(0.5f)) {
          ps.captureBeamReleased = false;
          ps.invincibleCnt = 120;
        }
      } else {
        tractorBeam->extendLength(0.5f);
        ps.captureBeamEnergy -= 0.005f;
      }
    }
    break;
  case GameState::Mode::BASIC:
    if (input->button & PadState::Button::B &&
        ps.capturedEnemyNum < PlayerState::MAX_CAPTURED_ENEMIES_NUM)
      tractorBeam->extendLength();
    else
      tractorBeam->reduceLength();
    break;
  case GameState::Mode::MODERN:
    if ((input->button & PadState::Button::B) &&
        !(input->button & PadState::Button::A))
      tractorBeam->extendLength();
    else
      tractorBeam->reduceLength();
    break;
  }
  tractorBeam->move();
  if (ps.shotCnt > 0)
    ps.shotCnt--;
  if (ps.capturedEnemyShotCnt > 0)
    ps.capturedEnemyShotCnt--;
  switch (gameState->mode()) {
  case GameState::Mode::CLASSIC:
  case GameState::Mode::BASIC:
    if (ps.pos.y > 0)
      ps.pos.y = 0;
    break;
  case GameState::Mode::MODERN:
    if (ps.pos.y > field->size().y)
      ps.pos.y = field->size().y;
    break;
  }
  if (ps.pos.y < -field->size().y)
    ps.pos.y = -field->size().y;
  if (ps.pos.x > field->size().x)
    ps.pos.x = field->size().x;
  else if (ps.pos.x < -field->size().x)
    ps.pos.x = -field->size().x;
  ps.pos.x = field->normalizeX(ps.pos.x);
  field->setEyePos(ps.pos);
  return true;
}

void PlayerSpec::fireShot(PlayerState &ps) {
  if (shots->num() >= shotMaxNum)
    return;
  Shot *s = shots->getInstance();
  if (s) {
    s->set(shotSpec, ps.pos, ps.deg, 0.66f);
    if (ps.isFirstShot) {
      ps.isFirstShot = false;
      ps.shotCnt += FIRST_SHOT_INTERVAL;
    } else {
      ps.shotCnt += SHOT_INTERVAL;
    }
    gameState->countShotFired();
    addShotParticle(ps.pos, ps.deg);
    SoundImpl::playSe("shot.wav");
    for (int i = 0; i < ps.capturedEnemyNum; i++) {
      if (gameState->mode() == GameState::Mode::MODERN &&
          ((i + ps.ghostShotCnt) % 4 == 0))
        continue;
      if (ps.capturedEnemies[i]->isCaptured()) {
        Shot *ces = capturedEnemiesShots->getInstance();
        if (!ces)
          break;
        float d = ps.deg;
        if (gameState->mode() == GameState::Mode::MODERN)
          d -= (ps.capturedEnemies[i]->pos().x - ps.pos.x) * 0.3f;
        ces->set(shotSpec, ps.capturedEnemies[i]->pos(), d, 0.66f);
        if (gameState->mode() != GameState::Mode::MODERN)
          ces->setParent(s);
        else
          gameState->countShotFired();
        addShotParticle(ps.capturedEnemies[i]->pos(), ps.deg);
      }
    }
    if (gameState->mode() == GameState::Mode::MODERN)
      ps.ghostShotCnt++;
  }
}

void PlayerSpec::addShotParticle(Vector p, float d) {
  for (int i = 0; i < 5; i++) {
    Particle *pt;
    pt = particles->getInstanceForced();
    pt->set(Particle::Shape::LINE, p.x - 0.5f, p.y,
            -d + rand.nextSignedFloat(0.5f), 0.25f + rand.nextFloat(0.75f), 1,
            1.0f, 0.25f, 0.5f, 10);
    pt = particles->getInstanceForced();
    pt->set(Particle::Shape::LINE, p.x + 0.5f, p.y,
            -d + rand.nextSignedFloat(0.5f), 0.25f + rand.nextFloat(0.75f), 1,
            1.0f, 0.25f, 0.5f, 10);
  }
}

void PlayerSpec::addVelocity(PlayerState &ps, Vector v, Vector o) {
  Vector rv = v.getElement(o, 0.05f, 0.25f);
  rv *= 5;
  ps.vel += rv;
  float d = atan2(rv.x, -rv.y);
  float sp = rv.vctSize();
  for (int i = 0; i < 36; i++) {
    Particle *pt;
    pt = particles->getInstanceForced();
    float r, g, b;
    r = 0.5f + rand.nextFloat(0.5f);
    g = 0.3f + rand.nextFloat(0.3f);
    b = 0.8f + rand.nextFloat(0.2f);
    pt->set(Particle::Shape::LINE, ps.pos.x, ps.pos.y,
            d + rand.nextSignedFloat(0.3f), sp * (1 + rand.nextFloat(2)), 1, r,
            g, b, 30 + rand.nextInt(30));
  }
  SoundImpl::playSe("flick.wav");
}

void PlayerSpec::respawn(PlayerState &ps) {
  if (gameState->mode() == GameState::Mode::MODERN) {
    for (int i = 0; i < 4; i++) {
      Enemy *e = enemies->getInstance();
      if (!e)
        break;
      e->set(ghostEnemySpec, ps.pos.x, ps.pos.y, 0, 0);
      playerState->addCapturedEnemy(e);
    }
  }
}

void PlayerSpec::clear() {
  tractorBeam->clear();
  shots->clear();
  capturedEnemiesShots->clear();
}

void PlayerSpec::destroyed(PlayerState &ps) {
  if (!ps.isActive())
    return;
  ps.destroyed();
  tractorBeam->clear();
  gameState->destroyedPlayer();
  float r, g, b;
  r = 0.5f + rand.nextFloat(0.5f);
  g = 0.3f + rand.nextFloat(0.3f);
  b = 0.8f + rand.nextFloat(0.2f);
  for (int i = 0; i < 100; i++) {
    Particle *p = particles->getInstanceForced();
    p->set(Particle::Shape::QUAD, ps.pos.x, ps.pos.y, rand.nextFloat(PI * 2),
           0.01f + rand.nextFloat(1.0f), 1 + rand.nextFloat(4), r, g, b,
           10 + rand.nextInt(200));
  }
  r = 0.5f + rand.nextFloat(0.5f);
  g = 0.3f + rand.nextFloat(0.3f);
  b = 0.8f + rand.nextFloat(0.2f);
  for (int i = 0; i < 30; i++) {
    Particle *p = particles->getInstanceForced();
    p->set(Particle::Shape::TRIANGLE, ps.pos.x, ps.pos.y,
           rand.nextFloat(PI * 2), 0.03f + rand.nextFloat(0.3f), 3, r, g, b,
           50 + rand.nextInt(150));
  }
  r = 0.5f + rand.nextFloat(0.5f);
  g = 0.3f + rand.nextFloat(0.3f);
  b = 0.8f + rand.nextFloat(0.2f);
  for (int i = 0; i < 300; i++) {
    Particle *p = particles->getInstanceForced();
    p->set(Particle::Shape::LINE, ps.pos.x, ps.pos.y, rand.nextFloat(PI * 2),
           0.07f + rand.nextFloat(0.7f), 1, r, g, b, 100 + rand.nextInt(100));
  }
  SoundImpl::playSe("player_explosion.wav");
}

void PlayerSpec::addScore(int sc) { gameState->addScore(sc); }

void PlayerSpec::addMultiplier(float mp) { gameState->addMultiplier(mp); }

float PlayerSpec::multiplier() { return gameState->multiplier(); }

void PlayerSpec::draw(const PlayerState &ps) {
  shots->draw();
  capturedEnemiesShots->draw();
  tractorBeam->draw();
  if (!ps.isActive())
    return;
  Vector3 p = field->calcCircularPos(ps.pos);
  float cd = field->calcCircularDeg(ps.pos.x);
  if (ps.hasShape())
    shape->draw3(p, cd, ps.deg);
  int c = ps.colorCnt % 60;
  float a;
  if (c < 30)
    a = (float)c / 30;
  else
    a = 1 - (float)(c - 30) / 30;
  Screen3D::setColor(a, a, a);
  lineShape->draw3(p, cd, ps.deg);
}

void PlayerSpec::drawState(PlayerState &ps) {
  Screen3D::setColor(1, 1, 1, 0.5f);
  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(15, 400, 0);
  glVertex3f(15 + ps.captureBeamEnergy * 100, 400, 0);
  glVertex3f(25 + ps.captureBeamEnergy * 100, 420, 0);
  glVertex3f(25, 420, 0);
  glEnd();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  float a;
  if (ps.captureBeamEnergy < 1) {
    a = ps.captureBeamEnergy;
  } else {
    int c = ps.colorCnt % 60;
    if (c < 30)
      a = (float)c / 30;
    else
      a = 1 - (float)(c - 30) / 30;
  }
  Screen3D::setColor(1, 1, 1, a);
  glBegin(GL_LINE_LOOP);
  glVertex3f(15, 400, 0);
  glVertex3f(115, 400, 0);
  glVertex3f(125, 420, 0);
  glVertex3f(25, 420, 0);
  glEnd();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  if (ps.captureBeamEnergy >= 1)
    Letter::drawString("READY", 50, 390, 4);
}

bool ShotSpec::move(ShotState &ss) {
  if (ss.parent)
    if (ss.parent->exists() == false)
      return false;
  ss.stepForward();
  ss.pos.x = field->normalizeX(ss.pos.x);
  if (!field->containsOuterY(ss.pos.y)) {
    return false;
  }
  if (enemies->checkShotHit(ss.pos, ss.deg, 2)) {
    if (ss.parent)
      ss.parent->remove();
    gameState->countShotHit();
    playerState->countShotHit();
    return false;
  }
  ss.cnt++;
  return true;
}

bool ShotSpec::checkParent(ShotState &ss) {
  if (ss.parent)
    if (ss.parent->exists() == false)
      return false;
  return true;
}

void ShotPool::checkParent() {
  for (auto a : actors)
    if (a->exists())
      if (!a->spec->checkParent(*a->state))
        a->remove();
}

int ShotPool::num() {
  int n = 0;
  for (auto a : actors)
    if (a->exists())
      n++;
  return n;
}
