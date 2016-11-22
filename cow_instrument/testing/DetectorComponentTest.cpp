
#include "cow_ptr.h"
#include "DetectorComponent.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"
#include "PathComponent.h"

#include <array>
#include <type_traits>

using namespace testing;

namespace {

TEST(detector_component_test, test_construction) {

  Eigen::Vector3d input{1, 2, 3};
  DetectorIdType det_id(1);
  ComponentIdType comp_id(1);
  DetectorComponent det(comp_id, det_id, input);
  EXPECT_EQ(det.getPos(), input);
  EXPECT_EQ(det.detectorId(), det_id);
  EXPECT_EQ(det.componentId(), comp_id);
}

TEST(detector_component_test, test_equals) {

  Eigen::Vector3d input{1, 2, 3};
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
  Eigen::Vector3d input{1, 2, 3};
  DetectorComponent det(ComponentIdType(1), DetectorIdType(1), input);
  DetectorComponent *clone = det.clone();

  EXPECT_TRUE(det.equals(*clone));
  EXPECT_NE(&det, clone); // different objects
  delete clone;
}

TEST(detector_component_test, test_copy) {
  ComponentIdType compId(1);
  DetectorIdType detId(2);
  Eigen::Vector3d pos{1, 2, 3};
  DetectorComponent det(compId, detId, pos);
  DetectorComponent copy(det);

  EXPECT_EQ(det.detectorId(), copy.detectorId());
  EXPECT_EQ(det.componentId(), copy.componentId());
  EXPECT_EQ(det.getPos(), copy.getPos());
}

TEST(detector_component_test, test_not_path_component) {
  DetectorComponent detector(ComponentIdType{1}, DetectorIdType{1},
                             Eigen::Vector3d{1, 1, 1});
  bool isPathComponent =
      std::is_base_of<PathComponent, DetectorComponent>::value;
  EXPECT_FALSE(isPathComponent)
      << "DetectorComponent should not have public base PathComponent";
}

TEST(detector_component_test, test_register_contents) {

  DetectorComponent detector(ComponentIdType(1), DetectorIdType{1},
                             Eigen::Vector3d{1, 0, 0} /*Detector position*/);

  // Registers
  ComponentInfo info;

  detector.registerContents(info);

  EXPECT_EQ(info.detectorSize(), 1) << "Detector pointer list should grow";
  EXPECT_EQ(info.pathComponents().size(), 0)
      << "Path lookup list should NOT grow. These are detectors";
  EXPECT_EQ(info.pathComponentIndexes().size(), 0)
      << "Path indexes should NOT grow. These are detectors";
  EXPECT_EQ(info.detectorComponentIndexes().size(), 1)
      << "Detector indexes should grow";
  EXPECT_EQ(info.proxies().size(), 1) << "Proxies should grow";

  EXPECT_FALSE(info.proxies()[0].hasParent());
  EXPECT_FALSE(info.proxies()[0].hasChildren());
  EXPECT_EQ(&info.proxies()[0].const_ref(), &detector);
  EXPECT_EQ(info.detectorComponentIndexes()[0], 0)
      << "Should be pointing to the zeroth index of proxies";
}

} // namespace
