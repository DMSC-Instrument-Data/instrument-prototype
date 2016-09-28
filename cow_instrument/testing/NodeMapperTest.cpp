#include <gtest/gtest.h>
#include "NodeMapper.h"
#include "DetectorComponent.h"
#include <boost/serialization/serialization.hpp>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

TEST(node_mapper_test, test_cannot_load_without_version) {
  NodeMapper mapper;
  mapper.nameMapper = std::string("node");
  mapper.nextIndexMapper = std::vector<size_t>{2, 3, 4};
  mapper.previousMapper = 0;
  mapper.contentsMapper->store(new DetectorComponent(
      ComponentIdType(1), DetectorIdType(2), Eigen::Vector3d{0, 0, 0}));
  EXPECT_THROW(mapper.create(), std::invalid_argument);
}

TEST(node_mapper_test, test_cannot_load_without_next_index) {
  NodeMapper mapper;
  mapper.nameMapper = std::string("node");
  mapper.previousMapper = 0;
  mapper.contentsMapper->store(new DetectorComponent(
      ComponentIdType(1), DetectorIdType(2), Eigen::Vector3d{0, 0, 0}));
  mapper.versionMapper = 1;
  EXPECT_THROW(mapper.create(), std::invalid_argument);
}

TEST(node_mapper_test, test_cannot_load_without_name) {
  NodeMapper mapper;
  mapper.nextIndexMapper = std::vector<size_t>{2, 3, 4};
  mapper.previousMapper = 0;
  mapper.contentsMapper->store(new DetectorComponent(
      ComponentIdType(1), DetectorIdType(2), Eigen::Vector3d{0, 0, 0}));
  mapper.versionMapper = 1;
  EXPECT_THROW(mapper.create(), std::invalid_argument);
}

TEST(node_mapper_test, test_cannot_load_without_contents) {
  NodeMapper mapper;
  mapper.nameMapper = std::string("node");
  mapper.nextIndexMapper = std::vector<size_t>{2, 3, 4};
  mapper.previousMapper = 0;
  mapper.versionMapper = 1;
  EXPECT_THROW(mapper.create(), std::invalid_argument);
}

TEST(node_mapper_test, test_create) {
  NodeMapper mapper;
  mapper.nameMapper = std::string("node");
  mapper.nextIndexMapper = std::vector<size_t>{2, 3, 4};
  mapper.previousMapper = 0;
  mapper.contentsMapper->store(new DetectorComponent(
      ComponentIdType(1), DetectorIdType(2), Eigen::Vector3d{0, 0, 0}));
  mapper.versionMapper = 1;
  Node product = mapper.create();
  EXPECT_EQ(product.version(), 1);
  EXPECT_EQ(product.name(), "node");
  EXPECT_EQ(product.children(), (std::vector<size_t>{2, 3, 4}));
  EXPECT_EQ(product.parent(), 0);
}

TEST(node_mapper_test, test_serialize_deserialize) {

  auto detector = std::make_shared<DetectorComponent>(
      ComponentIdType(1), DetectorIdType(2), Eigen::Vector3d{0, 0, 0});
  Node original(0, CowPtr<Component>(detector), "node", 1,
                std::vector<size_t>{2, 3, 4});

  NodeMapper toWrite(original);

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  out << toWrite;

  boost::archive::text_iarchive in(s);
  NodeMapper toRead;
  in >> toRead;
  Node copy = toRead.create();

  EXPECT_EQ(copy.version(), copy.version());
  EXPECT_EQ(copy.name(), copy.name());
  EXPECT_EQ(copy.children(), copy.children());
  EXPECT_EQ(copy.parent(), copy.parent());
  EXPECT_TRUE(copy.const_ref().equals(original.const_ref()));
}
