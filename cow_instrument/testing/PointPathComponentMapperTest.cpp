#include "gtest/gtest.h"

#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <sstream>

#include "PointSampleMapper.h"
#include "PointSourceMapper.h"

template <typename T>
class PointPathComponentMapperTest : public ::testing::Test {
public:
  using MapeeType = typename T::MapeeType;
};

// Register PointPathComponentMapper sub types for individual testing
typedef ::testing::Types<PointSourceMapper, PointSampleMapper> AllTypes;
TYPED_TEST_CASE(PointPathComponentMapperTest, AllTypes);

TYPED_TEST(PointPathComponentMapperTest, cannot_load_without_component_id) {

  TypeParam mapper;
  mapper.posMapper = Eigen::Vector3d{1, 1, 1};
  EXPECT_THROW(mapper.create(), std::invalid_argument)
      << "No component id. Should throw";
}

TYPED_TEST(PointPathComponentMapperTest, cannot_load_without_position) {

  TypeParam mapper;
  mapper.componentIdMapper = ComponentIdType(1);
  EXPECT_THROW(mapper.create(), std::invalid_argument)
      << "No position. Should throw";
}

TYPED_TEST(PointPathComponentMapperTest, test_load) {
  using MapeeType = typename TestFixture::MapeeType;

  TypeParam mapper;
  MapeeType source(Eigen::Vector3d{1, 1, 1}, ComponentIdType(1));

  mapper.componentIdMapper = source.componentId();
  mapper.posMapper = source.getPos();
  auto *product = mapper.create();
  EXPECT_TRUE(product->equals(source));
  delete product;
}

TYPED_TEST(PointPathComponentMapperTest, test_sourced_constructor) {
  using MapeeType = typename TestFixture::MapeeType;

  MapeeType source(Eigen::Vector3d{1, 1, 1}, ComponentIdType(1));
  TypeParam mapper(source);
  auto *product = mapper.create();
  EXPECT_TRUE(product->equals(source));
  delete product;
}

TYPED_TEST(PointPathComponentMapperTest, test_save_load) {
  using MapeeType = typename TestFixture::MapeeType;

  std::stringstream ss;
  boost::archive::text_oarchive out(ss);

  MapeeType pointComponent(Eigen::Vector3d{1, 1, 1}, ComponentIdType{1});

  {
    TypeParam mapperA;
    mapperA.store(pointComponent);
    out << mapperA;
  }
  {
    boost::archive::text_iarchive in(ss);
    TypeParam mapperB;
    in >> mapperB;

    auto *product = mapperB.create();
    EXPECT_TRUE(pointComponent.equals(*product));
    delete product;
  }
}
