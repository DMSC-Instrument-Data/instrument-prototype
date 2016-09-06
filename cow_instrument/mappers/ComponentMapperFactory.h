#include "ComponentVisitor.h"
#include "DetectorComponentMapper.h"
#include <vector>

class ComponentMapperFactory {

public:

    std::vector<std::shared_ptr<ComponentVisitor>> createMappers() const {

    return std::vector<std::shared_ptr<ComponentVisitor> >{
            std::make_shared<DetectorComponentMapper>()};
}

};
