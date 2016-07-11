#ifndef PATHCOMPONENT_H
#define PATHCOMPONENT_H

#include "Component.h"
#include "V3D.h"

/**
 * Abstract type for a non-terminating component that can be used to form a
 *Beam-Path.
 *
 * All Component subtypes save Detectors can be PathComponents
 */
class PathComponent : public Component {

public:
  /// Calculated internal scattering length
  virtual double length() const;

  /// Point at which neutrons enter the component
  virtual V3D entryPoint() const;

  /// Point at which neutrons exit the component
  virtual V3D exitPoint() const;

protected:
  virtual ~PathComponent();
};

#endif
