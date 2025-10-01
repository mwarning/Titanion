/*
 * $Id: vector.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2004 Kenta Cho. Some rights reserved.
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <string>

/**
 * 2D vector.
 */
class Vector {
public:
  float x, y;

  /*
    static this() {
      rsl = new Vector();
    }
  */

  Vector() : x(0), y(0) {}

  Vector(float x, float y) : x(x), y(y) {}

  void clear() {
    x = 0.f;
    y = 0.f;
  }

  float operator*(const Vector &v) const { return x * v.x + y * v.y; }

  Vector getElement(const Vector &v);
  Vector getElement(const Vector &v, float min, float max);

  Vector &operator+=(const Vector &v) {
    x += v.x;
    y += v.y;
    return *this;
  }

  Vector &operator-=(const Vector &v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  Vector &operator*=(float a) {
    x *= a;
    y *= a;
    return *this;
  }

  Vector &operator/=(float a) {
    x /= a;
    y /= a;
    return *this;
  }

  float checkSide(const Vector &pos1, const Vector &pos2) const;
  float checkSide(const Vector &pos1, const Vector &pos2,
                  const Vector &ofs) const;
  bool checkCross(const Vector &p, const Vector &p1, const Vector &p2,
                  float width) const;
  bool checkHitDist(const Vector &p, const Vector &pp, float dist) const;

  float vctSize() const { return sqrt(x * x + y * y); }

  float dist(const Vector &v) const { return dist(v.x, v.y); }

  float dist(float px = 0, float py = 0) const;
  float distAcc(const Vector &v) const;
  bool contains(const Vector &p, float r = 1) const;
  bool contains(float px, float py, float r = 1) const;
  void roll(float d);

  std::string toString() const;
};

/**
 * 3D vector.
 */
class Vector3 {
public:
  float x, y, z;

  /*
    static this() {
      rsl = new Vector3();
    }
  */

  Vector3() : x(0), y(0), z(0) {}

  Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

  void clear() {
    x = 0;
    y = 0;
    z = 0;
  }

  void rollX(float d);
  void rollY(float d);
  void rollZ(float d);

  void blend(const Vector3 &v1, const Vector3 &v2, float ratio);

  float vctSize() const { return sqrt(x * x + y * y + z * z); }

  float dist(const Vector3 &v) const { return dist(v.x, v.y, v.z); }

  float dist(float px = 0, float py = 0, float pz = 0) const;

  Vector3 getElement(const Vector3 &v);

  float operator*(const Vector3 &v) const {
    return x * v.x + y * v.y + z * v.z;
  }

  Vector3 &operator+=(const Vector3 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  Vector3 &operator-=(const Vector3 &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  Vector3 &operator*=(const Vector3 &v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }

  Vector3 &operator*=(float a) {
    x *= a;
    y *= a;
    z *= a;
    return *this;
  }

  std::string toString() const;
};

#endif
