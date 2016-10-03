#include <gtest/gtest.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <sstream>
#include "CompositeComponentMapper.h"

TEST(composite_component_mapper_test,
     test_mapper_create_throws_without_composite) {

  CompositeComponentMapper mapper;
  EXPECT_THROW(mapper.create(), std::invalid_argument)
      << "No vector provided. Mapper should throw.";
}

TEST(composite_component_mapper_test, test_non_default_constructor) {

  CompositeComponent original(ComponentIdType(1), "Some component");
  CompositeComponentMapper mapper(original);
}

TEST(composite_component_mapper_test, test_load_save) {

  CompositeComponent original(ComponentIdType(1), "Some component");
  original.addComponent(
      std::unique_ptr<DetectorComponent>(new DetectorComponent(
          ComponentIdType(1), DetectorIdType(2), Eigen::Vector3d{0, 0, 0})));
  original.addComponent(
      std::unique_ptr<DetectorComponent>(new DetectorComponent(
          ComponentIdType(2), DetectorIdType(3), Eigen::Vector3d{1, 1, 1})));

  CompositeComponentMapper serializerIn(original);

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  out << serializerIn;

  boost::archive::text_iarchive in(s);

  CompositeComponentMapper serializerOut;
  in >> serializerOut;

  CompositeComponent *created = serializerOut.create();

  EXPECT_TRUE(created->equals(original));
  delete created;
}

TEST(composite_component_mapper_test, experiment) {

  CompositeComponent original(ComponentIdType(1), "Some component");
  original.addComponent(
      std::unique_ptr<DetectorComponent>(new DetectorComponent(
          ComponentIdType(1), DetectorIdType(2), Eigen::Vector3d{0, 0, 0})));

  auto serializerIn = std::make_shared<CompositeComponentMapper>(original);

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  out << serializerIn;

  boost::archive::text_iarchive in(s);

  auto serializerOut = std::make_shared<CompositeComponentMapper>();
  in >> serializerOut;

  CompositeComponent *created = serializerOut->create();

  EXPECT_TRUE(created->equals(original));
  delete created;
}
