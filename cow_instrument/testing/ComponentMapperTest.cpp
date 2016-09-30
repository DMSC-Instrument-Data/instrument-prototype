#include "gtest/gtest.h"
#include "ComponentMapper.h"
#include "CompositeComponent.h"
#include "DetectorComponent.h"
#include "NullComponent.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

TEST(component_mapper_test, test_cannot_create_from_empty) {
  ComponentMapper mapper;
  EXPECT_THROW(mapper.create(), std::invalid_argument)
      << "Cannot create when nothing is stored";
}

TEST(component_mapper_test, test_serialize_deserialize_detector) {

  DetectorComponent detectorIn(ComponentIdType(1), DetectorIdType(2),
                               Eigen::Vector3d{1, 2, 3});

  ComponentMapper toWrite;
  toWrite.store(&detectorIn);

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  out << toWrite;

  boost::archive::text_iarchive in(s);
  ComponentMapper toRead;

  in >> toRead;
  Component *detectorOut = toRead.create();

  EXPECT_TRUE(detectorOut->equals(detectorIn));

  delete detectorOut;
}

TEST(component_mapper_test, test_serialize_deserialize_null_component) {

  NullComponent original;

  ComponentMapper toWrite;
  toWrite.store(&original);

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  out << toWrite;

  boost::archive::text_iarchive in(s);
  ComponentMapper toRead;

  in >> toRead;
  Component *componentOut = toRead.create();

  EXPECT_TRUE(componentOut->equals(original));

  delete componentOut;
}


TEST(component_mapper_test, test_serialize_deserialize_composite_component) {

  CompositeComponent original(ComponentIdType(1), "composite component");
  original.addComponent(
      std::unique_ptr<DetectorComponent>(new DetectorComponent(
          ComponentIdType(1), DetectorIdType(2), Eigen::Vector3d{0, 0, 0})));
  original.addComponent(std::unique_ptr<NullComponent>(new NullComponent));

  ComponentMapper toWrite;
  toWrite.store(&original);

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  out << toWrite;

  boost::archive::text_iarchive in(s);
  ComponentMapper toRead;

  in >> toRead;
  Component *componentOut = toRead.create();

  EXPECT_TRUE(componentOut->equals(original));

  delete componentOut;
}

