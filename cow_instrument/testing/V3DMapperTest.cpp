#include "V3DMapper.h"
#include <gtest/gtest.h>
#include <sstream>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

TEST(v3d_mapper_test, test_create_after_default_construction_throws) {

  V3DMapper mapper;
  EXPECT_THROW(mapper.create(), std::invalid_argument);
}

TEST(v3d_mapper_test, test_create) {

  V3DMapper mapper;
  Eigen::Vector3d item{1, 2, 3};
  // Store the item to later recreate
  mapper.store(item);
  auto product = mapper.create();
  EXPECT_TRUE(product.isApprox(item, 1e-14));
}

TEST(v3d_mapper_test, test_create_via_non_default_constructor) {
  Eigen::Vector3d item{1, 2, 3};
  V3DMapper mapper(item);
  auto product = mapper.create();
  EXPECT_TRUE(product.isApprox(item, 1e-14));
}

TEST(v3d_mapper_test, test_save_load) {

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  Eigen::Vector3d item{1, 2, 3};

  {
    V3DMapper mapperA;
    mapperA.store(item);
    out << mapperA;
  }
  {
    boost::archive::text_iarchive in(s);
    V3DMapper mapperB;
    in >> mapperB;

    EXPECT_TRUE(item.isApprox(mapperB.create(), 1e-14));
  }
}
