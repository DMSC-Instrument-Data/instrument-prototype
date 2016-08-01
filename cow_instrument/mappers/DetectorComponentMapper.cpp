#include "DetectorComponentMapper.h"

DetectorComponent DetectorComponentMapper::create() {

  // Check that everything required has been specified.
  if (componentIdMapper.initalized() && detectorIdMapper.initalized() &&
      pos.is_initialized()) {
    // Make the item we want.

    return DetectorComponent(componentIdMapper.create(),
                             detectorIdMapper.create(), pos.get());
  } else {
    throw std::invalid_argument("Cannot be deserialized. Not all mandatory "
                                "construction fields have been provided for "
                                "DetectorComponent");
  }
}

void DetectorComponentMapper::store(const DetectorComponent &source) {

  detectorIdMapper.store(source.detectorId());
  componentIdMapper.store(source.componentId());
  pos = source.getPos();
}
