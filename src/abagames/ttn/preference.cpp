/*
 * $Id: preference.d,v 1.4 2006/12/04 16:04:26 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include "preference.hpp"

/**
 * Load/Save/Record a high score table.
 */
void PreferenceImpl::load() {
  std::ifstream fd;
  try {
    fd.open(pref_dir() + "/" + PREF_FILE_NAME);
    int ver;
    fd >> ver;
    if (ver != VERSION_NUM)
      throw std::runtime_error("Wrong version num");
    fd >> _lastMode;
    for (int j = 0; j < MODE_NUM; j++)
      for (int i = 0; i < RANKING_NUM; i++)
        fd >> _highScore[j][i];
  } catch (const std::exception &e) {
    init();
  }

  fd.close();
  /*
    finally {
      if (fd)
        if (fd->isOpen())
          fd->close();
    }
  */
}
