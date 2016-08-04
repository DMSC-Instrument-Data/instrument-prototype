#include "ParabolicGuideMapper.h"

void ParabolicGuideMapper::store(const ParabolicGuide &source) {
  componentIdMapper.store(source.componentId());
  aMapper.store(source.a());
  hMapper.store(source.h());
  posMapper.store(source.getPos());
}

ParabolicGuideMapper::ParabolicGuideMapper(const ParabolicGuide &source) {
  store(source);
}

ParabolicGuide ParabolicGuideMapper::create() {

  // Check that everything required has been specified. Do not partially create.
  if (componentIdMapper.initalized() && aMapper.initalized() &&
      hMapper.initalized() && posMapper.initalized()) {
    // Make the item we want.

    return ParabolicGuide(componentIdMapper.create(), aMapper.create(),
                          hMapper.create(), posMapper.create());
  } else {
    throw std::invalid_argument("Cannot be deserialized. Not all mandatory "
                                "construction fields have been provided for "
                                "ParabolicGuideMapper");
  }
}
