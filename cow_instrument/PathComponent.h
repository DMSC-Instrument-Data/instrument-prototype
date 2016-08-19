#ifndef PATHCOMPONENT_H
#define PATHCOMPONENT_H

#include "Component.h"
#include <Eigen/Core>

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
  virtual Eigen::Vector3d entryPoint() const;

  /// Point at which neutrons exit the component
  virtual Eigen::Vector3d exitPoint() const;

  /// Is this the source
  virtual bool isSource() const;

  /// Is this the sample
  virtual bool isSample() const;

protected:
  virtual ~PathComponent();
};

#endif
