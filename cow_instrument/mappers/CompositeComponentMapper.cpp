#include "CompositeComponentMapper.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(CompositeComponentMapper);

CompositeComponentMapper::CompositeComponentMapper(
    const CompositeComponent &source) {
  store(source);
}

CompositeComponent *CompositeComponentMapper::create() {
  // Check that everything required has been specified.
  if (componentIdMapper.initalized() && itemMapper.initalized() &&
      nameMapper.initalized()) {

    auto component =
        new CompositeComponent(componentIdMapper.create(), nameMapper.create());
    auto items = itemMapper.create();
    for (auto &item : items) {
      component->addComponent(
          std::move(std::unique_ptr<Component>(item.create())));
    }
    return component;
  } else {
    throw std::invalid_argument("Cannot be deserialized. Not all mandatory "
                                "construction fields have been provided for "
                                "CompositeComponent");
  }
}

void CompositeComponentMapper::store(const CompositeComponent &source) {

  componentIdMapper = source.componentId();
  itemMapper.store(make_and_initialize_vec_serializers<ComponentMapperFactory>(
      source.children()));
  nameMapper = source.name();
}

bool CompositeComponentMapper::visit(const DetectorComponent *const) {
  return false;
}

bool CompositeComponentMapper::visit(const ParabolicGuide *const) {
  return false;
}

bool CompositeComponentMapper::visit(const PointSample *const) { return false; }

bool CompositeComponentMapper::visit(const PointSource *const) { return false; }

bool
CompositeComponentMapper::visit(const CompositeComponent *const component) {
  store(*component);
  return true;
}
