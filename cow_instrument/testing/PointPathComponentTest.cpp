#include "gtest/gtest.h"
#include "PointPathComponent.h"

/*
 * Concrete tyep for testing purposes.
 */
class ToyPointPathComponent : public PointPathComponent<ToyPointPathComponent> {
public:
  using PointPathComponent<ToyPointPathComponent>::PointPathComponent;
  std::string getname() const { return "Toy point path component"; }
  virtual bool accept(ComponentVisitor *) const override { return false; }

  virtual ~ToyPointPathComponent() {}
};

TEST(point_path_component_test, test_equals) {
  ToyPointPathComponent a(Eigen::Vector3d{1, 1, 1}, ComponentIdType(1));
  ToyPointPathComponent b(Eigen::Vector3d{1, 1, 1}, ComponentIdType(1));
  EXPECT_EQ(a, b);
  EXPECT_TRUE(a.equals(b));
}

TEST(point_path_component_test, test_not_equals) {
  ToyPointPathComponent reference(Eigen::Vector3d{1, 1, 1}, ComponentIdType(1));

  ToyPointPathComponent differentId(Eigen::Vector3d{1, 1, 1}, ComponentIdType(2));
  EXPECT_NE(reference, differentId);
  EXPECT_FALSE(reference.equals(differentId));

  ToyPointPathComponent differentPos(Eigen::Vector3d{1, 1, 2}, ComponentIdType(1));
  EXPECT_NE(reference, differentPos);
  EXPECT_FALSE(reference.equals(differentPos));
}

TEST(point_path_component_test, clone) {
  ToyPointPathComponent original(Eigen::Vector3d{1, 1, 1}, ComponentIdType(1));
  auto clone = original.clone();
  EXPECT_TRUE(clone->equals(original));
}

TEST(point_path_component_test, zero_length) {
  ToyPointPathComponent component(Eigen::Vector3d{1, 1, 1}, ComponentIdType(1));
  EXPECT_EQ(component.length(), 0);
}

TEST(point_path_component_test, point_entry_exit) {
  ToyPointPathComponent component(Eigen::Vector3d{1, 1, 1}, ComponentIdType(1));
  EXPECT_EQ(component.getPos(), component.exitPoint());
  EXPECT_EQ(component.getPos(), component.entryPoint());
}

TEST(point_path_component_test, test_register_contents) {
  ToyPointPathComponent component(Eigen::Vector3d{1, 0, 0}, ComponentIdType(1));

  // Registers
  ComponentInfo info;

  component.registerContents(info);

  EXPECT_EQ(info.detectorSize(), 0);
  EXPECT_EQ(info.pathComponents().size(), 1);
  EXPECT_EQ(info.pathComponentIndexes().size(), 1);
  EXPECT_EQ(info.detectorComponentIndexes().size(), 0);
  EXPECT_EQ(info.proxies().size(), 1) << "Proxies should grow";

  EXPECT_FALSE(info.proxies()[0].hasParent());
  EXPECT_FALSE(info.proxies()[0].hasChildren());
  EXPECT_EQ(&info.proxies()[0].const_ref(), &component);
  EXPECT_EQ(info.pathComponentIndexes()[0], 0)
      << "Should be pointing to the zeroth index of proxies";
}
