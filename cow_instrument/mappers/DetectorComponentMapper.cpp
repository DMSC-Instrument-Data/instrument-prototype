#include "DetectorComponentMapper.h"
#include <stdexcept>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(DetectorComponentMapper);

DetectorComponentMapper::DetectorComponentMapper(
    const DetectorComponent &source) {
  store(source);
}

DetectorComponent *DetectorComponentMapper::create() {

  // Check that everything required has been specified.
  if (componentIdMapper.initalized() && detectorIdMapper.initalized() &&
      posMapper.initalized()) {
    // Make the item we want.

    return new DetectorComponent(componentIdMapper.create(),
                                 detectorIdMapper.create(), posMapper.create());
  } else {
    throw std::invalid_argument("Cannot be deserialized. Not all mandatory "
                                "construction fields have been provided for "
                                "DetectorComponent");
  }
}

void DetectorComponentMapper::store(const DetectorComponent &source) {
  detectorIdMapper.store(source.detectorId());
  componentIdMapper.store(source.componentId());
  posMapper.store(source.getPos());
}

bool DetectorComponentMapper::visit(DetectorComponent const *const component) {
  store(*component);
  return true;
}

bool DetectorComponentMapper::visit(const ParabolicGuide *const) {
  return false;
}

bool DetectorComponentMapper::visit(const PointSample *const) { return false; }

bool DetectorComponentMapper::visit(const PointSource *const) { return false; }

bool DetectorComponentMapper::visit(const CompositeComponent *const) {
  return false;
}
