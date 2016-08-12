#ifndef COMPONENTVISITOR_H
#define COMPONENTVISITOR_H

#include "DetectorComponent.h"
#include "ParabolicGuide.h"
#include "PointSample.h"
#include "PointSource.h"

class ComponentVisitor {
public:
  virtual void visit(DetectorComponent const *const component) = 0;
  virtual void visit(ParabolicGuide const *const component) {}
  virtual void visit(PointSample const *const component) {}
  virtual void visit(PointSource const *const component) {}
  virtual ~ComponentVisitor() {}
};

#endif
