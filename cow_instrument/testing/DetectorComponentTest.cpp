#include "DetectorComponent.h"
#include "cow_ptr.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <array>
#include "MockTypes.h"
#include "PathComponent.h"

using namespace testing;

namespace {

TEST(detector_component_test, test_construction) {

  V3D input{1, 2, 3};
  DetectorIdType det_id(1);
  ComponentIdType comp_id(1);
  DetectorComponent det(comp_id, det_id, input);
  EXPECT_EQ(det.getPos(), input);
  EXPECT_EQ(det.detectorId(), det_id);
  EXPECT_EQ(det.componentId(), comp_id);
}

TEST(detector_component_test, test_equals) {

  V3D input{1, 2, 3};
  DetectorComponent a(ComponentIdType(1), DetectorIdType(1), input);
  DetectorComponent b(ComponentIdType(2), DetectorIdType(2),
                      input); // Different id.

  EXPECT_FALSE(a.equals(b));

  MockComponent c;
  EXPECT_FALSE(a.equals(c));

  DetectorComponent d(ComponentIdType(3), DetectorIdType(1), input);
  EXPECT_TRUE(a.equals(d));
}

TEST(detector_component_test, test_clone) {
  V3D input{1, 2, 3};
  DetectorComponent det(ComponentIdType(1), DetectorIdType(1), input);
  DetectorComponent *clone = det.clone();

  EXPECT_TRUE(det.equals(*clone));
  EXPECT_NE(&det, clone); // different objects
  delete clone;
}

TEST(detector_component_test, test_copy) {
  ComponentIdType compId(1);
  DetectorIdType detId(2);
  V3D pos{1, 2, 3};
  DetectorComponent det(compId, detId, pos);
  DetectorComponent copy(det);

  EXPECT_EQ(det.detectorId(), copy.detectorId());
  EXPECT_EQ(det.componentId(), copy.componentId());
  EXPECT_EQ(det.getPos(), copy.getPos());
}

TEST(detector_component_test, test_not_path_component) {
  DetectorComponent detector(ComponentIdType{1}, DetectorIdType{1},
                             V3D{1, 1, 1});
  EXPECT_EQ(dynamic_cast<PathComponent *>(&detector), nullptr)
      << "DetectorComponent should not have public base PathComponent";
}

} // namespace
