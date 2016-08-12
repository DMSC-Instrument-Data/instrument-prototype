#include <gtest/gtest.h>

#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <sstream>
#include <vector>

#include "CompositeComponentMapper.h"
#include "MockTypes.h"

TEST(vector_component_mapper_test, test_mapper_create_throws_without_vector) {

  VectorOfComponentMapper mapper;
  EXPECT_THROW(mapper.create(), std::invalid_argument)
      << "No vector provided. Mapper should throw.";
}

TEST(vector_component_mapper_test, test_non_default_constructor) {
  auto input = std::vector<std::unique_ptr<Component>>{};
  input.push_back(std::unique_ptr<Component>(new MockComponent{}));

  VectorOfComponentMapper mapper(input);
  std::vector<Component *> product = mapper.create();
  // EXPECT_EQ(input, product);
}

TEST(vector_component_mapper_test, test_load_save) {

  auto values = std::vector<std::unique_ptr<Component>>{};
  values.push_back(std::unique_ptr<Component>(new MockComponent{}));
  // values.push_back(std::unique_ptr<Component>(new MockComponent{}));

  std::stringstream ss;
  boost::archive::text_oarchive out(ss);

  {
    VectorOfComponentMapper mapperA;
    mapperA.store(values);
    out << mapperA;
  }

  {
    boost::archive::text_iarchive in(ss);
    VectorOfComponentMapper mapperB;
    in >> mapperB;

    auto product = mapperB.create();
    EXPECT_EQ(3, product.size());
    EXPECT_TRUE(values[0]->equals(*product[0]));
  }
}
