#include "gtest/gtest.h"
#include <boost/serialization/serialization.hpp>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "DetectorComponentMapper.h"

TEST(detector_component_mapper_test, cannot_load_without_detector_id) {

  DetectorComponentMapper mapper;
  mapper.componentIdMapper = ComponentIdType(1);
  mapper.posMapper = Eigen::Vector3d{1, 1, 1};
  EXPECT_THROW(mapper.create(), std::invalid_argument)
      << "No detector id. Should throw";
}

TEST(detector_component_mapper_test, cannot_load_without_component_id) {

  DetectorComponentMapper mapper;
  mapper.detectorIdMapper = DetectorIdType(1);
  mapper.posMapper = Eigen::Vector3d{1, 1, 1};
  EXPECT_THROW(mapper.create(), std::invalid_argument)
      << "No component id. Should throw";
}

TEST(detector_component_mapper_test, cannot_load_without_position) {

  DetectorComponentMapper mapper;
  mapper.detectorIdMapper = DetectorIdType(1);
  mapper.componentIdMapper = ComponentIdType(1);
  EXPECT_THROW(mapper.create(), std::invalid_argument)
      << "No position. Should throw";
}

TEST(detector_component_mapper_test, test_load) {

  DetectorComponentMapper mapper;
  DetectorComponent component(ComponentIdType(1), DetectorIdType(1),
                              Eigen::Vector3d{1, 1, 1});
  mapper.detectorIdMapper = component.detectorId();
  mapper.componentIdMapper = component.componentId();
  mapper.posMapper = component.getPos();
  auto loaded = mapper.create();
  EXPECT_TRUE(loaded->equals(component));
}

TEST(detector_component_mapper_test, test_save_load) {

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  DetectorComponent detector(ComponentIdType{1}, DetectorIdType{1},
                             Eigen::Vector3d{1, 1, 1});

  {
    DetectorComponentMapper mapperA;
    mapperA.store(detector);
    out << mapperA;
  }
  {
    boost::archive::text_iarchive in(s);
    DetectorComponentMapper mapperB;
    in >> mapperB;

    EXPECT_TRUE(detector.equals(*mapperB.create()));
  }
}
