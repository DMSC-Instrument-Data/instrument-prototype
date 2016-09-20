#include <gtest/gtest.h>
#include "Component.h"
#include "PolymorphicSerializer.h"

#include "ComponentVisitor.h"
#include "ComponentMapperFactory.h"
#include "DetectorComponent.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <sstream>
#include <vector>
#include <memory>

BOOST_CLASS_EXPORT(DetectorComponentMapper);

TEST(vector_of_component_mapper_test, test_vector_items) {

  auto a = std::make_shared<DetectorComponent>(
      ComponentIdType(1), DetectorIdType(2), Eigen::Vector3d{0, 0, 0});
  auto b = std::make_shared<DetectorComponent>(
      ComponentIdType(2), DetectorIdType(3), Eigen::Vector3d{0, 0, 0});
  auto c = std::make_shared<DetectorComponent>(
      ComponentIdType(3), DetectorIdType(4), Eigen::Vector3d{0, 0, 0});

  std::vector<std::shared_ptr<Component>> inputs{a, b, c};

  std::vector<PolymorphicSerializer<ComponentMapperFactory>> serializersIn =
      make_and_initialize_vec_serializers<ComponentMapperFactory>(inputs);

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  out << serializersIn;

  auto serializersOut =
      make_vec_serializers<ComponentMapperFactory>(inputs.size());
  boost::archive::text_iarchive in(s);

  in >> serializersOut;

  EXPECT_EQ(serializersOut.size(), 3);
  Component *itemA = serializersOut[0].create();
  Component *itemB = serializersOut[1].create();
  Component *itemC = serializersOut[2].create();

  auto detectorComponentA = dynamic_cast<DetectorComponent *>(itemA);
  auto detectorComponentB = dynamic_cast<DetectorComponent *>(itemB);
  auto detectorComponentC = dynamic_cast<DetectorComponent *>(itemC);

  EXPECT_TRUE(detectorComponentA != nullptr);
  EXPECT_TRUE(detectorComponentB != nullptr);
  EXPECT_TRUE(detectorComponentC != nullptr);

  EXPECT_TRUE(detectorComponentA->equals(*a));
  EXPECT_TRUE(detectorComponentB->equals(*b));
  EXPECT_TRUE(detectorComponentC->equals(*c));
}
