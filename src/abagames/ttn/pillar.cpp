/*
 * $Id: pillar.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */

#include "pillar.hpp"

static bool opCmpPillar(const Pillar *a, const Pillar *b) {
  return (fabs(b->pos().y) > fabs(a->pos().y));
}

void PillarPool::drawCenter() {
  std::vector<Pillar *> sas = actors;

  std::sort(sas.begin(), sas.end(), opCmpPillar);
  for (auto a : sas)
    if (a->exists() && !a->state->isOutside)
      a->draw();
}

bool PillarSpec::move(PillarState &ps) {
  if (!ps.isOutside) {
    ps.vy += VELOCITY_Y;
    ps.vy *= 0.98f;
    ps.pos.y += ps.vy;
    if (ps.vy > 0) {
      float ty;
      if (ps.previousPillar && ps.previousPillar->exists())
        ty = ps.previousPillar->pos().y - PillarShape::TICKNESS;
      else
        ty = ps.maxY;
      ty -= PillarShape::TICKNESS;
      if (!ps.isEnded && ps.pos.y > ty) {
        ps.vy *= -0.5f;
        ps.pos.y += (ty - ps.pos.y) * 0.5f;
        if (ps.previousPillar)
          ps.previousPillar->state->vy -= ps.vy * 0.5f;
      }
      if (ps.pos.y > 100)
        return false;
    }
  } else {
    ps.pos.y -= 0.2f;
    if (ps.pos.y < -50)
      return false;
  }
  ps.deg += ps.vdeg;
  return true;
}
