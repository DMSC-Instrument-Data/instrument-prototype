#include <gtest/gtest.h>

#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <sstream>
#include <vector>

class Item {

public:
  virtual bool accept(class ItemVisitor *visitor) = 0;

  virtual ~Item() {}
};

class Item1 : public Item {
public:
  virtual bool accept(ItemVisitor *visitor);
};

class Item2 : public Item {
public:
  virtual bool accept(ItemVisitor *visitor);
};

class ItemVisitor {
public:
  virtual bool visit(Item1 *visitee) = 0;
  virtual bool visit(Item2 *visitee) = 0;
  virtual ~ItemVisitor() {}
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {}

private:
};

class Mapper1 : public ItemVisitor {
public:
  virtual bool visit(Item1 *visitee) {

    std::cout << "Mapper 1 visiting Item1\n";
    return true;
  }
  virtual bool visit(Item2 *visitee) { return false; }
  virtual ~Mapper1() {}

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &boost::serialization::base_object<ItemVisitor>(*this);
  }
};

class Mapper2 : public ItemVisitor {
public:
  virtual bool visit(Item2 *visitee) {

    std::cout << "Mapper 2 visiting Item2\n";
    return true;
  }
  virtual bool visit(Item1 *visitee) { return false; }
  virtual ~Mapper2() {}
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &boost::serialization::base_object<ItemVisitor>(*this);
  }
};

bool Item1::accept(ItemVisitor *visitor) { return visitor->visit(this); }

bool Item2::accept(ItemVisitor *visitor) { return visitor->visit(this); }

template
<typename Family>
class PolymorphicSerializer {
private:
  Family *m_item;
  std::vector<std::shared_ptr<ItemVisitor>> m_itemVisitors;

public:
  PolymorphicSerializer(Family *to_serialize) : m_item(to_serialize) {

    m_itemVisitors.push_back(std::make_shared<Mapper1>());
    m_itemVisitors.push_back(std::make_shared<Mapper2>());
  }

  template <class Archive>
  void save_it(Archive &ar, const unsigned int version) {

    for (auto &visitor : m_itemVisitors) {
      if (m_item->accept(visitor.get())) {
        ar << visitor;
      }
    }
  }

  template <class Archive>
  const ItemVisitor& load_it(Archive &ar,
                                       const unsigned int version) {

    std::shared_ptr<ItemVisitor> target;

    for (auto &visitor : m_itemVisitors) {
      if (m_item->accept(visitor.get())) {
        ar >> visitor;
        target = visitor;
      }
    }

    return *target;
  }
};

BOOST_CLASS_EXPORT_GUID(Mapper1, "derived_one")
BOOST_CLASS_EXPORT_GUID(Mapper2, "derived_two")

TEST(component_visitor_test, test_serialization) {

  Item1 item;
  PolymorphicSerializer<Item> serializer(&item);

  std::stringstream s;
  boost::archive::text_oarchive out(s);

  serializer.save_it(out, 0);

  boost::archive::text_iarchive in(s);
  const auto & mapper = serializer.load_it(in, 0);

  // I only do this to
  EXPECT_TRUE(dynamic_cast<const Mapper1*>(&mapper) != nullptr);
}
