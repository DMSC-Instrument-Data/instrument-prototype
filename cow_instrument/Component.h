#include <array>

using V3D = std::array<double, 3>;

class Component {
public:
  virtual V3D getPos() const = 0;
  virtual void setPos(V3D& pos) = 0;
  virtual ~Component(){}
};
