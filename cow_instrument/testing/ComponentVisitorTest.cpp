#include <gtest/gtest.h>

#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <sstream>
#include <vector>
#include <initializer_list>

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
  typedef Item Family;
  virtual bool visit(const Item1 *visitee) = 0;
  virtual bool visit(const Item2 *visitee) = 0;
  virtual Family *create() const = 0;
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
    ar &boost::serialization::base_object<ItemVisitor>(*this);
    ar & m_id;
  }
};

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

  virtual Family *create() const { return new Item2(m_id); }

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &boost::serialization::base_object<ItemVisitor>(*this);
    ar & m_id;
  }
};

bool Item1::accept(ItemVisitor *visitor) const { return visitor->visit(this); }

bool Item2::accept(ItemVisitor *visitor) const { return visitor->visit(this); }

Item1::Item1(int value) : m_value(value){}

Item2::Item2(int value) : m_value(value){}

int Item1::getId() const {return m_value;}
int Item2::getId() const {return m_value;}

template <typename MapperFamily> class PolymorphicSerializer {
private:
  std::vector<std::shared_ptr<ItemVisitor>> m_itemVisitors;

public:
  PolymorphicSerializer(
      std::initializer_list<std::shared_ptr<MapperFamily>> mappers)
      :  m_itemVisitors(mappers) {}

  template <class Archive>
  void save_it(const typename MapperFamily::Family& to_serialize, Archive &ar,
               const unsigned int version) {

    for (auto &visitor : m_itemVisitors) {
      if (to_serialize.accept(visitor.get())) {
        ar << visitor;
      }
    }
  }

  template <class Archive>
  typename MapperFamily::Family *load_it(Archive &ar,
                                         const unsigned int version) {

    std::shared_ptr<ItemVisitor> target;

    ar >> target;

    return target->create();
  }
};

BOOST_CLASS_EXPORT_GUID(Mapper1, "derived_one")
BOOST_CLASS_EXPORT_GUID(Mapper2, "derived_two")

TEST(component_visitor_test, test_serialization_1) {


  PolymorphicSerializer<ItemVisitor> serializer(
      {std::make_shared<Mapper1>(), std::make_shared<Mapper2>()});

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  Item1 item(1);
  serializer.save_it(item, out, 0);

  boost::archive::text_iarchive in(s);
  auto * product = serializer.load_it(in, 0);
  EXPECT_TRUE(dynamic_cast<Item1*>(product) != nullptr);
  EXPECT_EQ(product->getId(), 1);
}

TEST(component_visitor_test, test_serialization_2) {


  PolymorphicSerializer<ItemVisitor> serializer(
      {std::make_shared<Mapper1>(), std::make_shared<Mapper2>()});

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  Item2 item(2);
  serializer.save_it(item, out, 0);

  boost::archive::text_iarchive in(s);
  auto * product = serializer.load_it(in, 0);
  EXPECT_TRUE(dynamic_cast<Item2*>(product) != nullptr);
  EXPECT_EQ(product->getId(), 2);
}
