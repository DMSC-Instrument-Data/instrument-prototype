#include "Component.h"

class Detector : public Component {
public:
  Detector(const V3D &pos);
  V3D getPos() const override;
  void setPos(V3D &pos) override;
  ~Detector();

private:
  V3D m_pos;
};
