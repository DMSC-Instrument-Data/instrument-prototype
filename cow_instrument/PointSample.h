#ifndef POINTSAMPLE_H
#define POINTSAMPLE_H

#include "PointPathComponent.h"

/**
 * A point sample. Behaves as a point from a neutronic perspective.
 */
class PointSample : public PointPathComponent<PointSample> {
public:
  using PointPathComponent<PointSample>::PointPathComponent;

  std::string getname() const { return "PointSample"; }

  virtual bool isSource() const override { return false; }

  virtual bool isSample() const override { return true; }
};

#endif
