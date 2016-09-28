#ifndef _POLYMORPHIC_SERIALIZER_H_
#define _POLYMORPHIC_SERIALIZER_H_

#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <sstream>
#include <vector>
#include <initializer_list>
#include <memory>
#include <cow_ptr.h>

template <typename MapperFactory> class PolymorphicSerializer {
private:
  std::vector<std::shared_ptr<typename MapperFactory::MapperFamily>>
      m_itemVisitors;

public:
  using MapperFamily = typename MapperFactory::MapperFamily;
  using ProductType = typename MapperFamily::ProductType;

  ProductType *m_thing; // TODO. Resolve ownership issue.

  PolymorphicSerializer()
      : m_thing(nullptr), m_itemVisitors(MapperFactory::createMappers()) {}

  void store(ProductType *thing) { m_thing = thing; }

  template <class Archive>
  void save(Archive &ar, const unsigned int version) const {

    bool accepted = false;
    for (auto &visitor : m_itemVisitors) {
      accepted |= m_thing->accept(visitor.get());
      if (accepted) {
        ar << BOOST_SERIALIZATION_NVP(visitor);
        break;
      }
    }
    if (!accepted) {
      throw std::runtime_error("No mapper found");
    }
  }

  template <class Archive> void load(Archive &ar, const unsigned int version) {

    std::shared_ptr<MapperFamily> target;
    ar >> BOOST_SERIALIZATION_NVP(target);

    m_thing = target->create();
  }

  ProductType *create() const {
    if (m_thing) {
      return m_thing;
    } else {
      throw std::invalid_argument("PolymorphicSerializer item never set");
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

namespace {
template <typename MapperFactory>
std::vector<PolymorphicSerializer<MapperFactory>>
make_vec_serializers(size_t size) {
  return std::vector<PolymorphicSerializer<MapperFactory>>(size);
}

template <typename MapperFactory>
std::vector<PolymorphicSerializer<MapperFactory>>
make_and_initialize_vec_serializers(const std::vector<std::shared_ptr<
    typename MapperFactory::MapperFamily::ProductType>> &items) {
  std::vector<PolymorphicSerializer<MapperFactory>> serializers(items.size());
  for (size_t i = 0; i < items.size(); ++i) {
    serializers[i].store(items[i].get()); // Hack unsafe. Fix this.
  }
  return serializers;
}
}

#endif
