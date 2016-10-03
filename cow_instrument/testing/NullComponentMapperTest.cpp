#include <gtest/gtest.h>
#include "NullComponentMapper.h"
#include "NullComponent.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

TEST(null_component_mapper_test, test_save_load) {

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  NullComponent original;

  {
    NullComponentMapper mapperA(original);
    out << mapperA;
  }
  {
    boost::archive::text_iarchive in(s);
    NullComponentMapper mapperB;
    in >> mapperB;
    auto * component = mapperB.create();

    EXPECT_TRUE(dynamic_cast<NullComponent*>(component) != NULL);
    delete component;
  }
}
