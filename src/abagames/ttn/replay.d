/*
 * $Id: replay.d,v 1.4 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */
module abagames.ttn.replay;

private import std.stdio;
private import abagames.util.sdl.pad;
private import abagames.util.sdl.recordableinput;
private import abagames.ttn.preference;

/**
 * Save/Load a replay data.
 */
public class ReplayData {
 public:
  static const int VERSION_NUM = 30;
  InputRecord!(PadState) inputRecord;
  long seed;
  int score = 0;
  int mode;
  bool stageRandomized;
 private:

  public void save(string fileName) {
    File fd = File(Preference.pref_dir() ~ "/" ~ fileName, "w");
    fd.rawWrite((&VERSION_NUM)[0..1]);
    fd.rawWrite((&seed)[0..1]);
    fd.rawWrite((&score)[0..1]);
    fd.rawWrite((&mode)[0..1]);
    byte sr = stageRandomized;
    fd.rawWrite((&sr)[0..1]);
    inputRecord.save(fd);
    fd.close();
  }

  public void load(string fileName) {
    File fd = File(Preference.pref_dir() ~ "/" ~ fileName);
    int ver;
    fd.rawRead((&ver)[0..1]);
    if (ver != VERSION_NUM)
      throw new Error("Wrong version num");
    fd.rawRead((&seed)[0..1]);
    fd.rawRead((&score)[0..1]);
    fd.rawRead((&mode)[0..1]);
    byte sr;
    fd.rawRead((&sr)[0..1]);
    stageRandomized = cast(bool) sr;
    inputRecord = new InputRecord!(PadState);
    inputRecord.load(fd);
    fd.close();
  }
}
