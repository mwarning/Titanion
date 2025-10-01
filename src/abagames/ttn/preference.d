/*
 * $Id: preference.d,v 1.4 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */
module abagames.ttn.preference;

private import std.stdio;
private import std.file;
private import core.stdc.stdlib;
private import std.string;
private import std.conv;
private import abagames.util.preference;

/**
 * Load/Save/Record a high score table.
 */
public class Preference: abagames.util.preference.Preference {
 public:
  static const int RANKING_NUM = 10;
  static const int MODE_NUM = 3;
 private:
  static const int VERSION_NUM = 30;
  static const string PREF_FILE_NAME = "ttn.prf";
  int[RANKING_NUM][MODE_NUM] _highScore;
  int _lastMode;

  public static string pref_dir()
  {
    const(char)* home = getenv("HOME");
    if (home is null)
      throw new Error("HOME environment variable not defined");
    string dir = to!string(home) ~ "/.titanion";
    try {
      mkdir(dir);
    } catch (FileException e) {
    }
    return dir;
  }


  public void load() {
    try {
      File fd = File(pref_dir() ~ "/" ~ PREF_FILE_NAME);
      int ver;
      fd.rawRead((&ver)[0..1]);
      if (ver != VERSION_NUM)
        throw new Error("Wrong version num");
      fd.rawRead((&_lastMode)[0..1]);
      for(int j = 0; j < MODE_NUM; j++)
        for(int i = 0; i < RANKING_NUM; i++)
          fd.rawRead((&_highScore[j][i])[0..1]);
    } catch (Throwable e) {
      init();
    }
  }

  private void init() {
    _lastMode = 2;
    for(int j = 0; j < MODE_NUM; j++)
      for(int i = 0; i < RANKING_NUM; i++)
        _highScore[j][i] = (10 - i) * 10000;
  }

  public void save() {
    File fd = File(pref_dir() ~ "/" ~ PREF_FILE_NAME, "w");
    fd.rawWrite((&VERSION_NUM)[0..1]);
    fd.rawWrite((&_lastMode)[0..1]);
    for(int j = 0; j < MODE_NUM; j++)
      for(int i = 0; i < RANKING_NUM; i++)
        fd.rawWrite((&_highScore[j][i])[0..1]);
  }

  public void setMode(int mode) {
    _lastMode = mode;
  }

  public void recordResult(int score, int mode) {
    setMode(mode);
    for (int i = 0; i < RANKING_NUM; i++) {
      if (score > _highScore[mode][i]) {
        for (int j = RANKING_NUM - 1; j >= i + 1; j--) {
          _highScore[mode][j] = _highScore[mode][j - 1];
        }
        _highScore[mode][i] = score;
        return;
      }
    }
  }

  public int[RANKING_NUM][] highScore() {
    return _highScore;
  }

  public int lastMode() {
    return _lastMode;
  }
}
