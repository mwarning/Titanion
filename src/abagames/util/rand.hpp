/*
 * $Id: rand.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#ifndef RAND_H
#define RAND_H

class MersenneTwister;

class Rand {
  MersenneTwister *mt;

public:
  Rand();

  void setRandSeed(long n);

  void setSeed(long n);

  int nextInt32();

  int nextInt(int n);

  int nextSignedInt(int n);

  float nextFloat(float n = 1);

  float nextSignedFloat(float n = 1);
};

#endif // RAND_H
