#include "gtest/gtest.h"
#include <boost/serialization/serialization.hpp>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "ParabolicGuideMapper.h"

TEST(parabolic_guide_mapper_test, cannot_load_without_component_id) {

  ParabolicGuideMapper mapper;
  mapper.posMapper = V3D{1, 1, 1};
  mapper.aMapper = 1;
  mapper.hMapper = 2;
  EXPECT_THROW(mapper.create(), std::invalid_argument)
      << "No component id. Should throw";
}

TEST(parabolic_guide_mapper_test, cannot_load_without_position) {

  ParabolicGuideMapper mapper;
  mapper.aMapper = 1;
  mapper.hMapper = 2;
  mapper.componentIdMapper = ComponentIdType(1);
  EXPECT_THROW(mapper.create(), std::invalid_argument)
      << "No position. Should throw";
}

TEST(parabolic_guide_mapper_test, cannot_load_without_a_parameter) {

  ParabolicGuideMapper mapper;
  mapper.posMapper = V3D{1, 1, 1};
  mapper.hMapper = 2;
  mapper.componentIdMapper = ComponentIdType(1);
  EXPECT_THROW(mapper.create(), std::invalid_argument)
      << "No position. Should throw";
}

TEST(parabolic_guide_mapper_test, cannot_load_without_h_parameter) {

  ParabolicGuideMapper mapper;
  mapper.aMapper = 1;
  mapper.posMapper = V3D{1, 1, 1};
  mapper.componentIdMapper = ComponentIdType(1);
  EXPECT_THROW(mapper.create(), std::invalid_argument)
      << "No position. Should throw";
}

TEST(parabolic_guide_mapper_test, test_load) {

  ParabolicGuideMapper mapper;
  ParabolicGuide component(ComponentIdType(1), 1, 2, V3D{1, 1, 1});
  mapper.componentIdMapper = component.componentId();
  mapper.posMapper = component.getPos();
  mapper.aMapper = component.a();
  mapper.hMapper = component.h();
  auto loaded = mapper.create();
  EXPECT_TRUE(loaded.equals(component));
}

TEST(parabolic_guide_mapper_test, test_save_load) {

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  ParabolicGuide guide(ComponentIdType(1), 1, 2, V3D{1, 1, 1});

  {
    ParabolicGuideMapper mapperA;
    mapperA.store(guide);
    out << mapperA;
  }
  {
    boost::archive::text_iarchive in(s);
    ParabolicGuideMapper mapperB;
    in >> mapperB;

    EXPECT_TRUE(guide.equals(mapperB.create()));
  }
}
