/*
 * $Id: frame.d,v 1.5 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include "frame.hpp"

#include "abagames/ttn/sound.hpp"

bool GameState::stageRandomized;

void FrameImpl::init() {
  SoundImpl::load();
  preference = dynamic_cast<PreferenceImpl *>(abstractPreference);
  preference->load();
  Letter::init();
  pad = dynamic_cast<Pad *>(abstractInput);
  pad->openJoystick();
  screen = dynamic_cast<ScreenImpl *>(abstractScreen);
  field = new Field(this, screen);
  enemies = new EnemyPool();
  enemies->field(field);
  bullets = new BulletPool();
  particles = new ParticlePool();
  bonusParticles = new ParticlePool();
  pillars = new PillarPool();
  enemies->init(128);
  bullets->init(1024);
  auto triangleParticleSpec = new TriangleParticleSpec(field);
  auto lineParticleSpec = new LineParticleSpec(field);
  auto quadParticleSpec = new QuadParticleSpec(field);
  auto bonusParticleSpec = new BonusParticleSpec(field);
  particles->init(1024);
  for (auto &a : particles->actors) {
    a->initParticle(triangleParticleSpec, lineParticleSpec, quadParticleSpec,
                    bonusParticleSpec);
  }
  bonusParticles->init(256);
  for (auto &a : bonusParticles->actors) {
    a->initParticle(triangleParticleSpec, lineParticleSpec, quadParticleSpec,
                    bonusParticleSpec);
  }
  triangleParticleSpec->setParticles(particles);
  pillars->init(48);
  gameState = new GameState(this, preference);
  title = new Title(preference, pad, this);
  title->setMode(preference->lastMode());
  title->init();
  playerSpec =
      new PlayerSpec(pad, gameState, field, enemies, bullets, particles);
  player = new Player(playerSpec);
  triangleParticleSpec->setPlayer(player);
  lineParticleSpec->setPlayer(player);
  quadParticleSpec->setPlayer(player);
  bonusParticleSpec->setPlayer(player);
  stage = new Stage(field, enemies, bullets, player, particles, bonusParticles,
                    pillars, gameState);
  gameState->setStage(stage);
  rand = new Rand();
  loadLastReplay();
}

void FrameImpl::quit() {
  title->close();
  playerSpec->close();
  gameState->close();
  stage->close();
  Letter::close();
}

void FrameImpl::start() { startTitle(); }

void FrameImpl::startInGame(int mode) {
  gameState->startInGame((GameState::Mode)mode);
  player->replayMode(false);
  RecordablePad *rp = dynamic_cast<RecordablePad *>(pad);
  rp->startRecord();
  replayData = new ReplayData();
  replayData->inputRecord = rp->inputRecord;
  replayData->seed = rand->nextInt32();
  clearAll();
  field->set();
  player->set();
  stage->start(replayData->seed);
  SoundImpl::clearMarkedSes();
  SoundImpl::playBgm();
}

void FrameImpl::startTitle() {
  startReplay();
  title->start();
}

void FrameImpl::startReplay() {
  gameState->startTitle();
  if (replayData) {
    player->replayMode(true);
    RecordablePad *rp = dynamic_cast<RecordablePad *>(pad);
    rp->startReplay(replayData->inputRecord);
  }
  clearAll();
  field->set();
  if (replayData) {
    gameState->mode((GameState::Mode)replayData->mode);
    gameState->setExtendScore();
    gameState->inReplay(true);
    player->set();
    stage->start(replayData->seed);
  } else {
    field->setEyePos(Vector(0, 0));
  }
  SoundImpl::clearMarkedSes();
  SoundImpl::haltBgm();
}

void FrameImpl::move() {
  gameState->move();
  field->move();
  if (gameState->isInGame() || replayData) {
    if (!gameState->paused()) {
      stage->move();
      pillars->move();
      player->move();
      enemies->move();
      bullets->move();
      particles->move();
      bonusParticles->move();
    }
  }
  if (gameState->isTitle())
    title->move();
}

void FrameImpl::draw() {
  SDL_Event *e = &mainLoop->event;
  if (e->type == SDL_VIDEORESIZE) {
    SDL_ResizeEvent re = e->resize;
    if (re.w > 150 && re.h > 100)
      screen->resized(re.w, re.h);
  }
  field->setLookAt();
  if (gameState->isInGame() || replayData) {
    pillars->drawOutside();
    field->drawBack();
    enemies->drawPillarBack();
    pillars->drawCenter();
    enemies->drawBack();
    field->drawFront();
    particles->draw();
    bonusParticles->draw();
    enemies->drawFront();
    player->draw();
    bullets->draw();
    field->beginDrawingFront();
    gameState->draw();
    if (gameState->isTitle())
      title->draw();
    player->drawState();
    field->resetLookAt();
    gameState->drawLeft();
  } else {
    pillars->drawOutside();
    field->drawBack();
    field->drawFront();
    field->beginDrawingFront();
    if (gameState->isTitle())
      title->draw();
  }
}

void FrameImpl::saveLastReplay() {
  try {
    replayData->score = gameState->score;
    replayData->mode = gameState->mode();
    replayData->stageRandomized = stage->randomized;
    saveReplay(LAST_REPLAY_FILE_NAME);
  } catch (std::exception &e) {
  }
}

void FrameImpl::loadLastReplay() {
  try {
    loadReplay(LAST_REPLAY_FILE_NAME);
    gameState->lastGameScore(replayData->score);
    gameState->lastGameMode(replayData->mode);
    stage->randomized = replayData->stageRandomized;
  } catch (std::exception &e) {
    resetReplay();
  }
}

void FrameImpl::saveReplay(const char *fileName) { replayData->save(fileName); }

void FrameImpl::loadReplay(const char *fileName) {
  replayData = new ReplayData();
  replayData->load(fileName);
}
