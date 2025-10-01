/*
 * $Id: preference.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef PREFERENCES_H
#define PREFERENCES_H

/**
 * Save/load the preference (e.g. high-score).
 */
struct Preference {
  virtual void save() = 0;
  virtual void load() = 0;
};

#endif // PREFERENCES_H
