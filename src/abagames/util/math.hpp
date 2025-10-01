/*
 * $Id: math.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef MATH_H
#define MATH_H

static constexpr float PI = 3.1415926535897932384626433832795;

/**
 * Math utility methods.
 */
namespace Math {
float normalizeDeg(float d);

float normalizeDeg360(float d);
} // namespace Math

#endif // MATH_H
