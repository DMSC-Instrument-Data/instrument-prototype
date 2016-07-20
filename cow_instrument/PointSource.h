#ifndef POINTSOURCE_H
#define POINTSOURCE_H

#include "PointPathComponent.h"

/**
 * A point source. Behaves as a point from a neutronic perspective.
 */
class PointSource : public PointPathComponent<PointSource> {
public:
  using PointPathComponent<PointSource>::PointPathComponent;

  std::string getname() const { return "PointSource"; }

  virtual bool isSource() const override { return true; }

  virtual bool isSample() const override { return false; }

  virtual ~PointSource(){};
};

#endif
