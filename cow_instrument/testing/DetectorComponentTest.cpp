
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

TEST(detector_component_test, test_single_rotation_around_detector_origin) {

  const Eigen::Vector3d rotationAxis{0, 0, 1};
  const double rotationAngle = M_PI / 2;
  const Eigen::Vector3d rotationCenter{0, 0, 0};

  DetectorComponent detector(
      ComponentIdType(1), DetectorIdType{1},
      rotationCenter /*I make this the position of the detector*/);
  detector.rotate(rotationAxis, rotationAngle, rotationCenter);
  // Check that the position has the identity rotation applied.
  EXPECT_TRUE(detector.getPos().isApprox(rotationCenter, 1e-14))
      << "Position should not change";

  // Check that the internal rotation gets updated. i.e component is rotated
  // around its own centre.
  Eigen::Matrix3d rotMatrix = detector.getRotation().toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal detector rotation not updated correctly";
}

TEST(detector_component_test, test_multiple_rotation_around_detector_origin) {

  const Eigen::Vector3d rotationAxis{0, 0, 1};
  const double rotationAngle = M_PI / 4;
  const Eigen::Vector3d rotationCenter{0, 0, 0};

  DetectorComponent detector(
      ComponentIdType(1), DetectorIdType{1},
      rotationCenter /*I make this the position of the detector*/);

  // Rotate once by 45 degrees
  detector.rotate(rotationAxis, rotationAngle, rotationCenter);
  // Rotate again by 45 degrees
  detector.rotate(rotationAxis, rotationAngle, rotationCenter);

  // Check that the position has the identity rotation applied.
  EXPECT_TRUE(detector.getPos().isApprox(rotationCenter, 1e-14))
      << "Position should not change";

  // Check that the internal rotation gets updated. i.e component is rotated
  // around its own centre.
  Eigen::Matrix3d rotMatrix = detector.getRotation().toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal detector rotation not updated correctly";
}

TEST(detector_component_test, test_single_rotation_around_arbitrary_center) {

  const Eigen::Vector3d rotationAxis{0, 0, 1};
  const double rotationAngle = M_PI / 2;
  const Eigen::Vector3d rotationCenter{0, 0, 0};

  DetectorComponent detector(ComponentIdType(1), DetectorIdType{1},
                             Eigen::Vector3d{1, 0, 0} /*Detector position*/);
  detector.rotate(rotationAxis, rotationAngle, rotationCenter);
  // Check that the position has the rotation applied.
  EXPECT_TRUE(detector.getPos().isApprox(Eigen::Vector3d(0, 1, 0), 1e-14));

  // Check that the internal rotation gets updated
  Eigen::Matrix3d rotMatrix = detector.getRotation().toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal detector rotation not updated correctly";
}

TEST(detector_component_test, test_multiple_rotation_arbitrary_center) {

  const Eigen::Vector3d rotationAxisZ{0, 0, 1};
  const Eigen::Vector3d rotationAxisX{1, 0, 0};
  const double rotationAngle = M_PI / 2;
  const Eigen::Vector3d rotationCenter{0, 0, 0};

  DetectorComponent detector(ComponentIdType(1), DetectorIdType{1},
                             Eigen::Vector3d{1, 0, 0} /*Detector position*/);

  // Rotate once by 90 degrees around z should put detector at 0,1,0
  detector.rotate(rotationAxisZ, rotationAngle, rotationCenter);
  // Rotate again by 90 degrees around x should put detector at 0,0,1
  detector.rotate(rotationAxisX, rotationAngle, rotationCenter);

  // Check that the position has the rotations applied.
  EXPECT_TRUE(detector.getPos().isApprox(Eigen::Vector3d(0, 0, 1), 1e-14));

  // Check that the internal rotation gets updated. i.e component is rotated
  // around its own centre.
  Eigen::Matrix3d rotMatrix = detector.getRotation().toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 0, 1}, 1e-14))
      << "Internal detector rotation not updated correctly";
}

TEST(detector_component_test, test_shift_position_by) {
  const Eigen::Vector3d position{1, 2, 3};
  const Eigen::Vector3d offset{1, 1, 1};

  DetectorComponent detector(ComponentIdType(1), DetectorIdType{1}, position);
  // Do the shift
  detector.shiftPositionBy(offset);
  EXPECT_TRUE(detector.getPos().isApprox(position + offset, 1e-14));
}

TEST(detector_component_test, test_register_contents) {

  DetectorComponent detector(ComponentIdType(1), DetectorIdType{1},
                             Eigen::Vector3d{1, 0, 0} /*Detector position*/);

  // Registers
  std::vector<const Detector *> detectorLookup;
  std::vector<const PathComponent *> pathLookup;
  std::vector<size_t> detectorIndexes;
  std::vector<size_t> pathIndexes;
  std::vector<ComponentProxy> proxies;

  detector.registerContents(detectorLookup, pathLookup, detectorIndexes,
                            pathIndexes, proxies);

  EXPECT_EQ(detectorLookup.size(), 1) << "Detector pointer list should grow";
  EXPECT_EQ(pathLookup.size(), 0)
      << "Path lookup list should NOT grow. These are detectors";
  EXPECT_EQ(pathIndexes.size(), 0)
      << "Path indexes should NOT grow. These are detectors";
  EXPECT_EQ(detectorIndexes.size(), 1) << "Detector indexes should grow";
  EXPECT_EQ(proxies.size(), 1) << "Proxies should grow";

  EXPECT_FALSE(proxies[0].hasParent());
  EXPECT_FALSE(proxies[0].hasChildren());
  EXPECT_EQ(&proxies[0].const_ref(), &detector);
  EXPECT_EQ(detectorIndexes[0], 0)
      << "Should be pointing to the zeroth index of proxies";
}

} // namespace
