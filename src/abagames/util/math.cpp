/*
 * $Id: math.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include "math.hpp"

#include <cmath>

namespace Math {
float normalizeDeg(float d) {
  float rd = d;
  if (rd < -PI)
    rd = PI * 2 - fmod(-rd, PI * 2);
  return fmod(rd + PI, PI * 2) - PI;
}

float normalizeDeg360(float d) {
  float rd = d;
  if (rd < -180)
    return 360 - fmod(-rd, 360);
  return fmod(rd + 180, 360) - 180;
}
} // namespace Math
