/*
 * $Id: frame.d,v 1.5 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef FRAMEIMPL_H
#define FRAMEIMPL_H

#include <cmath>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "abagames/ttn/bullet.hpp"
#include "abagames/ttn/enemy.hpp"
#include "abagames/ttn/field.hpp"
#include "abagames/ttn/letter.hpp"
#include "abagames/ttn/particle.hpp"
#include "abagames/ttn/pillar.hpp"
#include "abagames/ttn/player.hpp"
#include "abagames/ttn/preference.hpp"
#include "abagames/ttn/replay.hpp"
#include "abagames/ttn/screen.hpp"
#include "abagames/ttn/shape.hpp"
#include "abagames/ttn/stage.hpp"
#include "abagames/ttn/title.hpp"
#include "abagames/ttn/token.hpp"
#include "abagames/util/rand.hpp"
#include "abagames/util/sdl/frame.hpp"
#include "abagames/util/sdl/pad.hpp"
#include "abagames/util/vector.hpp"

class Title;

/**
 * Game frame and actor pools.
 */
class FrameImpl : public Frame /*abagames.util.sdl.frame.Frame*/ {
private:
  static constexpr const char *LAST_REPLAY_FILE_NAME = "last.rpl";
  Pad *pad;
  ScreenImpl *screen;
  Field *field;
  Player *player;
  PlayerSpec *playerSpec;
  EnemyPool *enemies;
  BulletPool *bullets;
  ParticlePool *particles, *bonusParticles;
  PillarPool *pillars;
  Stage *stage;
  Title *title;
  PreferenceImpl *preference;
  GameState *gameState;
  ReplayData *replayData;
  Rand *rand;

public:
  void init() override;

  void quit() override;

  void start() override;

  void startInGame(int mode);

  void startTitle();

  void startReplay();

private:
  void clearAll() {
    enemies->clear();
    bullets->clear();
    particles->clear();
    bonusParticles->clear();
    pillars->clear();
  }

public:
  void breakLoop() { mainLoop->breakLoop(); }

  void move() override;

  void handleSound() { SoundImpl::playMarkedSes(); }

  void addSlowdownRatio(float sr) { mainLoop->addSlowdownRatio(sr); }

  void draw() override;

  const Uint8 *keys() { return pad->keys; }

  // Handle a replay data.
  void saveLastReplay();

  void loadLastReplay();

  void saveReplay(const char *fileName);

  void loadReplay(const char *fileName);

  void resetReplay() { replayData = NULL; }
};

class GameState {
public:
  enum Mode {
    CLASSIC,
    BASIC,
    MODERN,
  };
  static constexpr int MODE_NUM = 3;
  static constexpr const char *MODE_NAME[] = {"CLASSIC", " BASIC ", "MODERN"};
  static bool stageRandomized;

private:
  enum Scene {
    TITLE,
    IN_GAME,
  };
  static constexpr int MAX_LEFT = 4;
  FrameImpl *frame;
  PreferenceImpl *preference;
  Scene scene;
  Stage *stage;

public:
  int score;

private:
  int _lastGameScore;
  int _lastGameMode;
  int nextExtendScore;
  float _multiplier;
  int left;
  bool escPressed, pPressed;
  bool _paused;
  int pauseCnt;
  bool _isGameOver;
  int gameOverCnt;
  PlayerShape *playerShape;
  PlayerLineShape *playerLineShape;
  bool _inReplay;
  Mode _mode;
  int extendScore;
  int proximityMultiplier, pmDispCnt;
  /*
    invariant {
      assert(_multiplier >= 1.0f);
    }
  */
public:
  GameState(FrameImpl *frame, PreferenceImpl *preference) {
    this->frame = frame;
    this->preference = preference;
    playerShape = new PlayerShape();
    playerLineShape = new PlayerLineShape();
    clear();
    _lastGameScore = -1;
  }

  void setStage(Stage *stage) { this->stage = stage; }

  void close() {
    playerShape->close();
    playerLineShape->close();
  }

  void startInGame(Mode m) {
    scene = Scene::IN_GAME;
    clear();
    _mode = m;
    left = 2;
    setExtendScore();
    _lastGameScore = -1;
    preference->setMode(_mode);
    stage->randomized = stageRandomized;
  }

  void setExtendScore() {
    switch (_mode) {
    case Mode::CLASSIC:
      extendScore = 100000;
      break;
    case Mode::BASIC:
    case Mode::MODERN:
      extendScore = 1000000;
      break;
    }
    nextExtendScore = extendScore;
  }

  void startTitle() {
    scene = Scene::TITLE;
    clear();
    left = 2;
  }

private:
  void clear() {
    score = 0;
    _multiplier = 1.0f;
    left = 0;
    gameOverCnt = 0;
    _isGameOver = _paused = false;
    _inReplay = false;
    pmDispCnt = 0;
  }

public:
  void startGameOver() {
    if (!isInGameAndNotGameOver())
      return;
    _isGameOver = true;
    gameOverCnt = 0;
    SoundImpl::fadeBgm();
    _lastGameScore = score;
    _lastGameMode = mode();
    preference->recordResult(score, _mode);
    preference->save();
  }

  void startGameOverWithoutRecording() {
    if (_isGameOver)
      return;
    _isGameOver = true;
    gameOverCnt = 0;
    SoundImpl::fadeBgm();
  }

  void backToTitle() {
    if (isTitle()) {
      frame->startReplay();
      return;
    }
    if (gameOverCnt > 120) {
      frame->saveLastReplay();
      frame->startTitle();
    }
  }

  void move() {
    handleEscKey();
    if (isInGameAndNotGameOver()) {
      handlePauseKey();
      if (_paused) {
        pauseCnt++;
        return;
      }
    }
    if (isInGame()) {
      if (!_isGameOver) {
        frame->handleSound();
      } else {
        gameOverCnt++;
        if (gameOverCnt < 60)
          frame->handleSound();
        if (gameOverCnt > 1000)
          backToTitle();
      }
    } else {
      if (_inReplay)
        frame->handleSound();
      if (_isGameOver) {
        gameOverCnt++;
        if (_inReplay && gameOverCnt < 60)
          frame->handleSound();
        if (gameOverCnt > 120)
          backToTitle();
      }
    }
    if (pmDispCnt > 0)
      pmDispCnt--;
  }

private:
  void handleEscKey() {
    if (frame->keys()[SDLK_ESCAPE] == SDL_PRESSED) {
      if (!escPressed) {
        escPressed = true;
        if (scene == Scene::IN_GAME) {
          frame->loadLastReplay();
          frame->startTitle();
        } else {
          frame->breakLoop();
        }
      }
    } else {
      escPressed = false;
    }
  }

  void handlePauseKey() {
    if (frame->keys()[SDLK_p] == SDL_PRESSED) {
      if (!pPressed) {
        pPressed = true;
        _paused = !_paused;
        pauseCnt = 0;
      }
    } else {
      pPressed = false;
    }
  }

public:
  void addScore(int sc, bool noMultiplier = false) {
    if (!_isGameOver) {
      if (noMultiplier)
        score += sc;
      else
        score += sc * _multiplier;
      if (score >= nextExtendScore) {
        if (left < MAX_LEFT) {
          left++;
          SoundImpl::playSe("extend.wav");
        }
        nextExtendScore += extendScore;
        if (_mode == Mode::MODERN)
          extendScore += 1000000;
      }
    }
  }

  void addMultiplier(float mp) {
    if (!_isGameOver)
      _multiplier += mp;
  }

  void mulMultiplier(float mp) {
    if (!_isGameOver) {
      _multiplier *= mp;
      if (_multiplier < 1)
        _multiplier = 1;
    }
  }

  void setProximityMultiplier(int pm) {
    proximityMultiplier = pm;
    pmDispCnt = 60;
  }

  void destroyedPlayer() {
    left--;
    if (left < 0) {
      if (isInGame())
        startGameOver();
      else
        startGameOverWithoutRecording();
    }
  }

  void countShotFired() { stage->countShotFired(); }

  void countShotHit() { stage->countShotHit(); }

  void draw() {
    Letter::drawNum(score, 132, 5, 7);
    Letter::drawNum(nextExtendScore, 134, 25, 5);
    if (_lastGameScore >= 0) {
      Letter::drawNum(_lastGameScore, 360, 5, 7);
      // Letter::drawString(GameState::MODE_NAME[_lastGameMode], 292, 24, 5);
    }
    Letter::drawNum((int)(_multiplier * 100), 626, 4, 9, 3, 33, 2);
    if (pmDispCnt > 0)
      Letter::drawNum(proximityMultiplier, 626, 30, 7, 0, 33);
    stage->drawPhaseNum();
    if (isInGame()) {
      if (!_isGameOver)
        stage->draw();
      if (_isGameOver) {
        if (gameOverCnt > 60) {
          Letter::drawString("GAME OVER", 214, 200, 12);
          stage->drawGameover();
        }
      } else if (_paused) {
        if (pauseCnt % 120 < 60)
          Letter::drawString("PAUSE", 290, 420, 7);
      }
      Letter::drawString(GameState::MODE_NAME[mode()], 540, 400, 5);
    }
  }

  void drawLeft() {
    for (int i = 0; i < left; i++) {
      glPushMatrix();
      glTranslatef(-10.2f + i, -7.5f, -10);
      glScalef(0.6f, 0.6f, 0.6f);
      playerShape->draw();
      Screen3D::setColor(0, 0, 0);
      playerLineShape->draw();
      glPopMatrix();
    }
  }

  bool isInGame() { return (scene == Scene::IN_GAME); }

  bool isInGameAndNotGameOver() {
    return (scene == Scene::IN_GAME && !_isGameOver);
  }

  bool isTitle() { return (scene == Scene::TITLE); }

  bool isGameOver() { return _isGameOver; }

  bool paused() { return _paused; }

  float multiplier() { return _multiplier; }

  bool inReplay(bool v) { return _inReplay = v; }

  int lastGameScore(int v) { return _lastGameScore = v; }

  int lastGameMode(int v) { return _lastGameMode = v; }

  Mode mode() { return _mode; }

  Mode mode(Mode v) { return _mode = v; }
};

#endif // FRAMEIMPL_H
