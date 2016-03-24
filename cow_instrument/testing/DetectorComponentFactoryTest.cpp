#include "gtest/gtest.h"
#include "DetectorComponentFactory.h"
#include "DetectorComponent.h"

TEST(detector_component_factory_test, test_basic_product) {

  DetectorComponentFactory factory;
  ComponentIdType compId(1);
  DetectorIdType detId(1);
  V3D pos{1, 1, 1};

  auto product = factory.create_unique(compId, detId, pos);

  EXPECT_TRUE(dynamic_cast<const DetectorComponent *>(product.get()))
      << "Not a DetectorComponent produced";

  EXPECT_EQ(compId, product->componentId());
  EXPECT_EQ(detId, product->detectorId());
  EXPECT_EQ(pos, product->getPos());
}

TEST(detector_component_factory_test, test_index_increment) {

  DetectorComponentFactory factory;
  size_t expectedIndex = 1;

  auto product1 =
      factory.create_unique(ComponentIdType(1), DetectorIdType(1), V3D{1, 1, 1});

  EXPECT_TRUE(dynamic_cast<const DetectorComponent *>(product1.get()))
      << "Not a DetectorComponent produced";

  EXPECT_EQ(expectedIndex, product1->detectorIndex());

  auto product2 =
      factory.create(ComponentIdType(1), DetectorIdType(1), V3D{1, 1, 1});

  EXPECT_EQ(++expectedIndex, product2->detectorIndex())
      << "Detector index should have been incremented for us.";
}
