#include "gtest/gtest.h"

#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <sstream>
#include <vector>

#include "VectorOfMapper.h"

// Create a VectorOfMapper around some element type (element type must be
// serializeable)
using VectorOfIntsMapper = VectorOfMapper<int>;

TEST(vector_of_mapper_test, test_mapper_create_throws_without_vector) {
  VectorOfIntsMapper mapper;
  EXPECT_THROW(mapper.create(), std::invalid_argument)
      << "No vector provided. VectorOfMapper should throw.";
}

TEST(vector_of_mapper_test, test_non_default_constructor) {
  auto input = std::vector<int>{1, 2, 3};
  VectorOfIntsMapper mapper(input);
  auto product = mapper.create();
  EXPECT_EQ(input, product);
}

TEST(vector_of_mapper_test, test_load_save) {

  std::vector<int> values = {1, 2, 3};

  std::stringstream ss;
  boost::archive::text_oarchive out(ss);

  {
    VectorOfIntsMapper mapperA;
    mapperA.store(values);
    out << mapperA;
  }
  {
    boost::archive::text_iarchive in(ss);
    VectorOfIntsMapper mapperB;
    in >> mapperB;

    auto product = mapperB.create();
    EXPECT_EQ(3, product.size());
    EXPECT_EQ(values, product);
  }
}
