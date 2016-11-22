#include "InstrumentTreeMapper.h"
#include "InstrumentTree.h"
#include "CompositeComponent.h"
#include "DetectorComponent.h"
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

  auto root = std::make_shared<CompositeComponent>(ComponentIdType(0));
  auto composite = std::unique_ptr<CompositeComponent>(
      new CompositeComponent(ComponentIdType(1)));
  composite->addComponent(
      std::unique_ptr<DetectorComponent>(new DetectorComponent(
          ComponentIdType(1), DetectorIdType(1), Eigen::Vector3d{1, 1, 1})));
  composite->addComponent(
      std::unique_ptr<DetectorComponent>(new DetectorComponent(
          ComponentIdType(1), DetectorIdType(2), Eigen::Vector3d{1, 1, 1})));

  root->addComponent(std::move(composite));

  root->addComponent(std::unique_ptr<PointSource>(
      (new PointSource(Eigen::Vector3d{0, 0, 0},
                       ComponentIdType(3))))); // Add C
  root->addComponent(std::unique_ptr<PointSample>(
      (new PointSample(Eigen::Vector3d{0, 0, 10},
                       ComponentIdType(4))))); // Add D

  return InstrumentTree(root);
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

  EXPECT_EQ(product.nDetectors(), original.nDetectors());
  EXPECT_EQ(product.nPathComponents(), original.nPathComponents());

  EXPECT_TRUE(product.rootComponent()->equals(*original.rootComponent()))
      << "InstrumentTrees look different.";
}

