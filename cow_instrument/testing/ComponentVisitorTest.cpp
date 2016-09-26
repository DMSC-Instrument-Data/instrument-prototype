#include <gtest/gtest.h>
#include "PolymorphicSerializer.h"
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <sstream>
#include <vector>
#include <initializer_list>
#include <memory>
#include <cow_ptr.h>

namespace boost {
namespace serialization {}
}

class Item {

public:
  virtual bool accept(class ItemVisitor *visitor) const = 0;
  virtual int getId() const = 0;
  virtual ~Item() {}
};

class Item1 : public Item {
public:
  Item1(int value);
  virtual bool accept(ItemVisitor *visitor) const;
  int getId() const;

private:
  int m_value;
};

class Item2 : public Item {
public:
  Item2(int value);
  virtual bool accept(ItemVisitor *visitor) const;
  int getId() const;

private:
  int m_value;
};

class ItemVisitor {
public:
  typedef Item ProductType;
  virtual bool visit(const Item1 *visitee) = 0;
  virtual bool visit(const Item2 *visitee) = 0;
  virtual ProductType *create() const = 0;
  virtual ~ItemVisitor() {}
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {}

private:
};

class Mapper1 : public ItemVisitor {
public:
  int m_id;

  virtual bool visit(const Item1 *visitee) {

    std::cout << "Mapper 1 visiting Item1\n";
    m_id = visitee->getId();
    return true;
  }
  virtual bool visit(const Item2 *visitee) { return false; }
  virtual ~Mapper1() {}

  virtual Item1 *create() const { return new Item1(m_id); }

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(ItemVisitor);
    ar &m_id;
  }
};
BOOST_CLASS_EXPORT(Mapper1);

class Mapper2 : public ItemVisitor {
public:
  int m_id;

  virtual bool visit(const Item2 *visitee) {

    std::cout << "Mapper 2 visiting Item2\n";
    m_id = visitee->getId();
    return true;
  }
  virtual bool visit(const Item1 *visitee) { return false; }
  virtual ~Mapper2() {}

  virtual ProductType *create() const { return new Item2(m_id); }

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(ItemVisitor);
    ar &m_id;
  }
};
BOOST_CLASS_EXPORT(Mapper2);

bool Item1::accept(ItemVisitor *visitor) const { return visitor->visit(this); }

bool Item2::accept(ItemVisitor *visitor) const { return visitor->visit(this); }

Item1::Item1(int value) : m_value(value) {}

Item2::Item2(int value) : m_value(value) {}

int Item1::getId() const { return m_value; }
int Item2::getId() const { return m_value; }

class ItemMapperFactory {

public:
  using MapperFamily = ItemVisitor;

  static std::vector<std::shared_ptr<ItemVisitor>> createMappers() {

    return std::vector<std::shared_ptr<ItemVisitor>>{
        std::make_shared<Mapper1>(), std::make_shared<Mapper2>()};
  }
};

TEST(component_visitor_test, test_serialization_1) {

  PolymorphicSerializer<ItemMapperFactory> serializerA;

  Item1 item(1);
  serializerA.store(&item);

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  out << serializerA;

  boost::archive::text_iarchive in(s);

  PolymorphicSerializer<ItemMapperFactory> serializerB;

  in >> serializerB;

  Item *product = serializerB.create();
  EXPECT_TRUE(dynamic_cast<Item1 *>(product) != nullptr);
  EXPECT_EQ(product->getId(), 1);
}

TEST(component_visitor_test, test_serialization_2) {

  PolymorphicSerializer<ItemMapperFactory> serializerA;
  ;

  Item2 item(2);
  serializerA.store(&item);

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  out << serializerA;

  boost::archive::text_iarchive in(s);

  PolymorphicSerializer<ItemMapperFactory> serializerB;

  in >> serializerB;

  Item *product = serializerB.create();
  EXPECT_TRUE(dynamic_cast<Item2 *>(product) != nullptr);
  EXPECT_EQ(product->getId(), 2);
}



TEST(component_visitor_test, test_vector_items) {

  std::vector<std::shared_ptr<Item>> items{std::make_shared<Item1>(1),
                                           std::make_shared<Item2>(2),
                                           std::make_shared<Item1>(3)};

  std::vector<PolymorphicSerializer<ItemMapperFactory>> serializersIn =
      make_and_initialize_vec_serializers<ItemMapperFactory>(items);

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  out << serializersIn;


  auto serializersOut = make_vec_serializers<ItemMapperFactory>(items.size());
  boost::archive::text_iarchive in(s);

  in >> serializersOut;

  EXPECT_EQ(serializersOut.size(), 3);
  Item *itemA = serializersOut[0].create();
  Item *itemB = serializersOut[1].create();
  Item *itemC = serializersOut[2].create();

  EXPECT_TRUE(dynamic_cast<Item1 *>(itemA) != nullptr);
  EXPECT_TRUE(dynamic_cast<Item2 *>(itemB) != nullptr);
  EXPECT_TRUE(dynamic_cast<Item1 *>(itemC) != nullptr);

  EXPECT_EQ(itemA->getId(), 1);
  EXPECT_EQ(itemB->getId(), 2);
  EXPECT_EQ(itemC->getId(), 3);
}

