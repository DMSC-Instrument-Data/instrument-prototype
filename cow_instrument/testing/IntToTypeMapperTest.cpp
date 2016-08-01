#include "gtest/gtest.h"
#include "IntToType.h"
#include "IntToTypeMapper.h"

#include <boost/serialization/serialization.hpp>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

using MyIntToType = IntToType<10, std::size_t>;
using MyIntToTypeMapper = IntToTypeMapper<MyIntToType>;

TEST(int_to_type_mapper_test, test_default_construction) {
  MyIntToTypeMapper mapper;
  EXPECT_FALSE(mapper.initalized());
}

TEST(int_to_type_mapper_test, test_value_construction) {

  MyIntToType value(1);
  MyIntToTypeMapper mapper(value);

  EXPECT_TRUE(mapper.initalized());
  EXPECT_EQ(mapper.create(), value);
}

TEST(int_to_type_mapper_test, test_throw_when_creating_with_uninitialized) {

  MyIntToTypeMapper mapper;
  EXPECT_FALSE(mapper.initalized());
  EXPECT_THROW(mapper.create(), std::runtime_error);
}

TEST(int_to_type_mapper_test, test_save_load) {

  std::stringstream ss;
  boost::archive::text_oarchive out(ss);

  MyIntToType value(3);

  {
    MyIntToTypeMapper mapperA;
    mapperA.store(value);
    out << mapperA;
  }
  {
    boost::archive::text_iarchive in(ss);
    MyIntToTypeMapper mapperB;
    in >> mapperB;

    EXPECT_EQ(value, mapperB.create());
  }
}
