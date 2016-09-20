#include "DetectorComponentMapper.h"
#include <stdexcept>

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