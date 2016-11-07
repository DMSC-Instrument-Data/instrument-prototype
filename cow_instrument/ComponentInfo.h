#ifndef COMPONENTINFO_H
#define COMPONENTINFO_H

#include <vector>
#include <cstddef>
#include "ComponentProxy.h"

class Detector;
class PathComponent;
class CompositeComponent;

/**
 * Collection of ComponentProxies. Which are the flattened tree representation
 * of the InstrumentTree. Other key information also stored here in the
 *flatterned floor.
 *
 * Methods for registering components (Flattened tree construction) and
 * navigating the subtrees have been provided.
 */
class ComponentInfo {
public:
  ComponentInfo() = default;
  void registerDetector(Detector const *const comp);
  void registerPathComponent(PathComponent const *const comp);
  size_t registerComposite(CompositeComponent const *const comp);
  void registerDetector(Detector const *const comp, size_t parentIndex);
  void registerPathComponent(PathComponent const *const comp,
                             size_t parentIndex);
  size_t registerComposite(CompositeComponent const *const comp,
                           size_t parentIndex);

  std::vector<ComponentProxy> proxies();
  const ComponentProxy &proxyAt(size_t index) const;
  size_t componentSize() const;
  std::vector<size_t> subTreeIndexes(size_t proxyIndex) const;

  std::vector<const Detector *> detectorComponents() const;
  std::vector<const PathComponent *> pathComponents() const;
  std::vector<size_t> pathComponentIndexes() const;
  std::vector<size_t> detectorComponentIndexes() const;

  std::vector<ComponentProxy>::const_iterator begin() const;
  std::vector<ComponentProxy>::const_iterator end() const;
  std::vector<ComponentProxy>::const_iterator cbegin() const;
  std::vector<ComponentProxy>::const_iterator cend() const;

  bool operator==(const ComponentInfo &other) const;
  bool operator!=(const ComponentInfo &other) const;

private:
  size_t updateProxies(Component const *const comp);
  size_t updateProxies(Component const *const comp, size_t previousIndex);

  std::vector<ComponentProxy> m_proxies;
  std::vector<const Detector *> m_detectorComponents;
  std::vector<const PathComponent *> m_pathComponents;
  std::vector<size_t> m_pathComponentIndexes;
  std::vector<size_t> m_detectorComponentIndexes;
};

#endif
