/*
 * $Id: vector.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2004 Kenta Cho. Some rights reserved.
 */

#include <cmath>
#include <string>

#include "vector.hpp"

static Vector vector_rsl;
static Vector3 vector3_rsl;

Vector Vector::getElement(const Vector &v) {
  float ll = v * v;
  if (ll > 0.1f) {
    float mag = *this * v;
    vector_rsl.x = mag * x / ll;
    vector_rsl.y = mag * y / ll;
  } else {
    vector_rsl.x = 0;
    vector_rsl.y = 0;
  }
  return vector_rsl;
}

Vector Vector::getElement(const Vector &v, float min, float max) {
  float ll = v * v;
  if (ll > 0.1f) {
    float mag = *this * v;
    mag /= ll;
    vector_rsl.x = mag * x;
    vector_rsl.y = mag * y;
  } else {
    vector_rsl.x = 0;
    vector_rsl.y = 0;
  }
  float d = vector_rsl.vctSize();
  if (d > 0.1f && d < min)
    vector_rsl *= (min / d);
  else if (d > max)
    vector_rsl *= (max / d);
  return vector_rsl;
}

float Vector::checkSide(const Vector &pos1, const Vector &pos2) const {
  float xo = pos2.x - pos1.x;
  float yo = pos2.y - pos1.y;
  if (xo == 0) {
    if (yo == 0)
      return 0;
    if (yo > 0)
      return x - pos1.x;
    else
      return pos1.x - x;
  } else if (yo == 0) {
    if (xo > 0)
      return pos1.y - y;
    else
      return y - pos1.y;
  } else {
    if (xo * yo > 0)
      return (x - pos1.x) / xo - (y - pos1.y) / yo;
    else
      return -(x - pos1.x) / xo + (y - pos1.y) / yo;
  }
}

float Vector::checkSide(const Vector &pos1, const Vector &pos2,
                        const Vector &ofs) const {
  float xo = pos2.x - pos1.x;
  float yo = pos2.y - pos1.y;
  float mx = x + ofs.x;
  float my = y + ofs.y;
  if (xo == 0) {
    if (yo == 0)
      return 0;
    if (yo > 0)
      return mx - pos1.x;
    else
      return pos1.x - mx;
  } else if (yo == 0) {
    if (xo > 0)
      return pos1.y - my;
    else
      return my - pos1.y;
  } else {
    if (xo * yo > 0)
      return (mx - pos1.x) / xo - (my - pos1.y) / yo;
    else
      return -(mx - pos1.x) / xo + (my - pos1.y) / yo;
  }
}

bool Vector::checkCross(const Vector &p, const Vector &p1, const Vector &p2,
                        float width) const {
  float a1x, a1y, a2x, a2y;
  if (x < p.x) {
    a1x = x - width;
    a2x = p.x + width;
  } else {
    a1x = p.x - width;
    a2x = x + width;
  }
  if (y < p.y) {
    a1y = y - width;
    a2y = p.y + width;
  } else {
    a1y = p.y - width;
    a2y = y + width;
  }
  float b1x, b1y, b2x, b2y;
  if (p2.y < p1.y) {
    b1y = p2.y - width;
    b2y = p1.y + width;
  } else {
    b1y = p1.y - width;
    b2y = p2.y + width;
  }
  if (a2y >= b1y && b2y >= a1y) {
    if (p2.x < p1.x) {
      b1x = p2.x - width;
      b2x = p1.x + width;
    } else {
      b1x = p1.x - width;
      b2x = p2.x + width;
    }
    if (a2x >= b1x && b2x >= a1x) {
      float a = y - p.y;
      float b = p.x - x;
      float c = p.x * y - p.y * x;
      float d = p2.y - p1.y;
      float e = p1.x - p2.x;
      float f = p1.x * p2.y - p1.y * p2.x;
      float dnm = b * d - a * e;
      if (dnm != 0) {
        float x = (b * f - c * e) / dnm;
        float y = (c * d - a * f) / dnm;
        if (a1x <= x && x <= a2x && a1y <= y && y <= a2y && b1x <= x &&
            x <= b2x && b1y <= y && y <= b2y)
          return true;
      }
    }
  }
  return false;
}

bool Vector::checkHitDist(const Vector &p, const Vector &pp, float dist) const {
  float bmvx, bmvy, inaa;
  bmvx = pp.x;
  bmvy = pp.y;
  bmvx -= p.x;
  bmvy -= p.y;
  inaa = bmvx * bmvx + bmvy * bmvy;
  if (inaa > 0.00001) {
    float sofsx, sofsy, inab, hd;
    sofsx = x;
    sofsy = y;
    sofsx -= p.x;
    sofsy -= p.y;
    inab = bmvx * sofsx + bmvy * sofsy;
    if (inab >= 0 && inab <= inaa) {
      hd = sofsx * sofsx + sofsy * sofsy - inab * inab / inaa;
      if (hd >= 0 && hd <= dist)
        return true;
    }
  }
  return false;
}

float Vector::dist(float px, float py) const {
  float ax = fabs(x - px);
  float ay = fabs(y - py);
  if (ax > ay)
    return ax + ay / 2;
  else
    return ay + ax / 2;
}

float Vector::distAcc(const Vector &v) const {
  return sqrt((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y));
}

bool Vector::contains(const Vector &p, float r) const {
  return contains(p.x, p.y, r);
}

bool Vector::contains(float px, float py, float r) const {
  if (px >= -x * r && px <= x * r && py >= -y * r && py <= y * r)
    return true;
  else
    return false;
}

void Vector::roll(float d) {
  float tx = x * cos(d) - y * sin(d);
  y = x * sin(d) + y * cos(d);
  x = tx;
}

std::string Vector::toString() const {
  return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

/**
 * 3D vector.
 */

void Vector3::rollX(float d) {
  float ty = y * cos(d) - z * sin(d);
  z = y * sin(d) + z * cos(d);
  y = ty;
}

void Vector3::rollY(float d) {
  float tx = x * cos(d) - z * sin(d);
  z = x * sin(d) + z * cos(d);
  x = tx;
}

void Vector3::rollZ(float d) {
  float tx = x * cos(d) - y * sin(d);
  y = x * sin(d) + y * cos(d);
  x = tx;
}

void Vector3::blend(const Vector3 &v1, const Vector3 &v2, float ratio) {
  x = v1.x * ratio + v2.x * (1 - ratio);
  y = v1.y * ratio + v2.y * (1 - ratio);
  z = v1.z * ratio + v2.z * (1 - ratio);
}

float Vector3::dist(float px, float py, float pz) const {
  float ax = fabs(x - px);
  float ay = fabs(y - py);
  float az = fabs(z - pz);
  float axy;
  if (ax > ay)
    axy = ax + ay / 2;
  else
    axy = ay + ax / 2;
  if (axy > az)
    return axy + az / 2;
  else
    return az + axy / 2;
}

Vector3 Vector3::getElement(const Vector3 &v) {
  float ll = v * v;
  if (ll != 0) {
    float mag = *this * v;
    vector3_rsl.x = mag * v.x / ll;
    vector3_rsl.y = mag * v.y / ll;
    vector3_rsl.z = mag * v.z / ll;
  } else {
    vector3_rsl.x = 0;
    vector3_rsl.y = 0;
    vector3_rsl.z = 0;
  }
  return vector3_rsl;
}

std::string Vector3::toString() const {
  return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " +
         std::to_string(z) + ")";
}
