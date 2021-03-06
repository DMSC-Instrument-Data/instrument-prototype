#ifndef COMPONENTVISITOR_H
#define COMPONENTVISITOR_H

class Component;
class CompositeComponent;
class DetectorComponent;
class ParabolicGuide;
class PointSample;
class PointSource;
class NullComponent;

class ComponentVisitor {
public:
  using ProductType = Component;
  virtual bool visit(DetectorComponent const *const component) = 0;
  virtual bool visit(ParabolicGuide const *const component) = 0;
  virtual bool visit(PointSample const *const component) = 0;
  virtual bool visit(PointSource const *const component) = 0;
  virtual bool visit(CompositeComponent const *const component) = 0;
  virtual bool visit(NullComponent const * const component) = 0;
  virtual ProductType *create() = 0;
  virtual ~ComponentVisitor() {}

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {}
};

#endif
