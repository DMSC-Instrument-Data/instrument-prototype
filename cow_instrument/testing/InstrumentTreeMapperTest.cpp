#include "InstrumentTreeMapper.h"
#include "InstrumentTree.h"
#include "CompositeComponent.h"
#include "DetectorComponent.h"
#include "Node.h"
#include "PointSample.h"
#include "PointSource.h"
#include <Eigen/Core>
#include <vector>
#include <gtest/gtest.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <sstream>

namespace {

InstrumentTree make_instrument() {

  /*
        A (Root)
        |
 --------------------------------
 |                 |            |
 B (Composite/Bank)   C (source)     D(Sample)

  */

  auto composite = std::make_shared<CompositeComponent>(ComponentIdType(1));
  composite->addComponent(
      std::unique_ptr<DetectorComponent>(new DetectorComponent(
          ComponentIdType(1), DetectorIdType(1), Eigen::Vector3d{1, 1, 1})));
  composite->addComponent(
      std::unique_ptr<DetectorComponent>(new DetectorComponent(
          ComponentIdType(1), DetectorIdType(2), Eigen::Vector3d{1, 1, 1})));

  std::vector<Node> nodes;

  nodes.push_back(Node());                                // Add A
  nodes.push_back(Node(0, CowPtr<Component>(composite))); // Add B
  nodes.push_back(
      Node(0, CowPtr<Component>(new PointSource(Eigen::Vector3d{0, 0, 0},
                                                ComponentIdType(3))))); // Add C
  nodes.push_back(
      Node(0, CowPtr<Component>(new PointSample(Eigen::Vector3d{0, 0, 10},
                                                ComponentIdType(4))))); // Add D

  nodes[0].addChild(1); // add composite
  nodes[0].addChild(2); // add source
  nodes[0].addChild(3); // add sample

  return InstrumentTree(std::move(nodes));
}
}

TEST(instrument_tree_mapper_test, test_cannot_create_without_instrument) {
  InstrumentTreeMapper mapper;
  EXPECT_THROW(mapper.create(), std::invalid_argument) << "Should throw. No instrument set.";
}

TEST(instrument_tree_mapper_test, test_create) {
  InstrumentTree original = make_instrument();
  InstrumentTreeMapper originalMapper(original);

  std::stringstream s;
  boost::archive::text_oarchive out(s);
  out << originalMapper;

  boost::archive::text_iarchive in(s);
  InstrumentTreeMapper outputMapper;
  in >> outputMapper;

  InstrumentTree product = outputMapper.create();

  EXPECT_EQ(product.nodeSize(), original.nodeSize());
  EXPECT_EQ(product.nDetectors(), original.nDetectors());
  EXPECT_EQ(product.nPathComponents(), original.nPathComponents());
  EXPECT_TRUE(product.getDetector(0).equals(original.getDetector(0)))
      << "No detector match. InstrumentTrees look different.";
  EXPECT_TRUE(product.getPathComponent(0).equals(original.getPathComponent(0)))
      << "No path component match. InstrumentTrees look different.";
  EXPECT_TRUE(product.getPathComponent(1).equals(original.getPathComponent(1)))
      << "No path component match. InstrumentTrees look different.";
}

